//
//  File_DMM.cpp
//  DibbunsMacroMadness
//
//  Created by Dibbun on 7/29/26.
//
//  This is a replacement for the File_

#include "File_DMM.hpp"
#include <iconv.h>
#include <sys/stat.h>
#include <libgen.h>
#include <ctime>

/*
**    DTSFileSpecPriv
**
**    implement this class as quickly and as quietly as possible.
*/
DTSDefineImplementFirm(DTSFileSpecPriv)


DTSError DTSFileSpec::SetFileName( const char * fname )
{
    // Assign data to the internal priv struct
    priv.p->SetFileName(fname);
    
    return noErr;
}

static inline void ConvertTimespecToDTSDate(const struct timespec& ts, DTSDate& outDate)
{
    // 1. Shift Unix seconds to Mac OS 1904 epoch seconds
    const unsigned long UNIX_TO_MAC_EPOCH_OFFSET = 2082844800UL;
    outDate.dateInSeconds = (ulong)ts.tv_sec + UNIX_TO_MAC_EPOCH_OFFSET; // BUG: ulong is uint32 here. May fail in 2039

    // 2. Break down the standard Unix time into components
    struct tm timeComponents;
    // Use gmtime_r for UTC/GMT. Swap to localtime_r(ts.tv_sec, &timeComponents) if required by the app
    gmtime_r(&ts.tv_sec, &timeComponents);

    // 3. Map tm components to the custom struct, adjusting standard C offsets
    outDate.dateYear   = static_cast<int16_t>(timeComponents.tm_year + 1900); // tm_year is years since 1900
    outDate.dateMonth  = static_cast<int16_t>(timeComponents.tm_mon + 1);    // tm_mon is 0-11, target wants 1-12
    outDate.dateDay    = static_cast<int16_t>(timeComponents.tm_mday);       // 1-31 matches directly
    outDate.dateHour   = static_cast<int16_t>(timeComponents.tm_hour);       // 0-23 matches directly
    outDate.dateMinute = static_cast<int16_t>(timeComponents.tm_min);        // 0-59 matches directly
    outDate.dateSecond = static_cast<int16_t>(timeComponents.tm_sec);        // 0-59 matches directly

    // 4. Map the Day of the Week
    // standard tm_wday is: 0 (Sunday) to 6 (Saturday)
    // Target wants:       1 (Monday) to 7 (Sunday)
    if (timeComponents.tm_wday == 0) {
        outDate.dateDayOfWeek = 7; // Sunday
    } else {
        outDate.dateDayOfWeek = static_cast<int16_t>(timeComponents.tm_wday);
    }
}

void
DTSDate::Get()
{
    dateInSeconds = 0;
    dateYear = 0;            // 1904 to 2040
    dateMonth = 0;            // 1 to 12
    dateDay = 0;            // 1 to 31
    dateHour = 0;            // 0 to 23
    dateMinute = 0;            // 0 to 59
    dateSecond = 0;            // 0 to 59
    dateDayOfWeek = 0;        // 1 (monday) to 7 (sunday)

    struct timespec now;
    if(0 == ::clock_gettime(CLOCK_REALTIME, &now) )
        ConvertTimespecToDTSDate( now, *this);

}


// returns the modification date
DTSError DTSFileSpec::GetModifiedDate( DTSDate * date, bool bJustWantSeconds )
{
    *date = (DTSDate){}; // Init to 0s
    const char * fileName = priv.p->GetFileName();
    if(fileName[0] == '\0')
        return fnfErr;
        
    struct stat  statBuf;
    memset( &statBuf, 0, sizeof(statBuf));
    
    int err = stat( fileName, &statBuf );
    if(err)
        return GetFileErrorCode(errno);

    ConvertTimespecToDTSDate( statBuf.st_mtimespec, *date);

    return noErr;
}

// calls fopen(3)
// perms:  "rb", "wb", "r", "w" etc.
std::FILE * DTSFileSpec::fopen( const char * perms )
{
    return priv.p->fopen(perms);
}

// deletes the file
DTSError DTSFileSpec::Delete()
{
    const char * fileName = GetFileName();
    if( fileName[0] == '\0')
        return fnfErr;
    
    if( ::unlink(fileName) )
    {
        switch( errno )
        {
            case EACCES:
            case EPERM:
                return wrPermErr;
            default:
                return fnfErr;
        }
    }
    
    return noErr;
}


// This is an unfortunate contrivance of the original code to track the
// working directory on the side. Since I can't be sure there weren't two
// working directories, the application wide one and this one, we track it separately here.
char gCurrentDirPath[PATH_MAX];


// used in ResolvePaths()
enum
{
    kNameInSpec,    // basename is in the DTSFileSpecPriv
    kNameInPath,    // basename is in local buffer
    kNameInFSS        // basename is in the FSRef [implicitly]
};


static inline bool IsDirectory( const struct stat & statBuf)
{
    return 0 != S_ISDIR(statBuf.st_mode);
}

DTSError    ResolvePathsPlus( DTSFileSpecPriv * p, bool bResolveLastAlias = true )
{
    const char * fileName = p->GetFileName();
    int dirID = p->GetDirectory();
    int err = 0;
    int loc = kNameInSpec;
    char fullPath[PATH_MAX] = "";
    if( NULL == fileName )
        fileName = "";
    bool bIsAbsolute = (fileName[0] == '/');    // True if the path starts from root
    
    if( bIsAbsolute )
        fullPath[0] = '\0'; // Root override: Ignore starting dirID / gCurrentDirPath
    else if( DTSFileSpecPriv::kNoDirectory == dirID )
    {
        strncpy(fullPath, gCurrentDirPath, sizeof(fullPath));
        if( '\0' == fileName[0] )
        {
            dirID = ::open(gCurrentDirPath, O_RDONLY | O_DIRECTORY);
            if( -1 == dirID )
                return GetFileErrorCode(errno);
            p->SetVolDir( DTSFileSpecPriv::kDefaultVolume, dirID);
            p->SetResolved(true);
            return noErr;
        }
    }
    else
    {
        if( '\0' == fileName[0] )
        {
            p->SetResolved( true );
            return noErr;
        }
        
        err = fcntl(dirID, F_GETPATH, fullPath);
        if(err)
            return GetFileErrorCode(errno);
    }
    
    // Make sure we don't have any legacy HFS paths with colons. Assuming we don't need to convert those.
    char fileNameScratch[PATH_MAX]; strncpy(fileNameScratch, fileName, sizeof(fileNameScratch));
    const char * separators = "/:";
    char * lastP = fileNameScratch;
    char * word;
    size_t offset = strlen( fullPath);
    struct stat statBuf = {0};
    char storage[PATH_MAX];

    // Deal with edge case of path is root
    word = strtok_r(fileNameScratch, separators, &lastP);
    if( NULL == word && bIsAbsolute)
    {
        strncpy(fullPath, "/", sizeof(fullPath));
        loc = kNameInPath;
    }
    
    // Iterate through path
    for(; word; word = strtok_r(NULL, separators, &lastP) )
    {
        // Skip double // or empty tokens
        if( '/' == word[0] || '\0' == word[0] )
            continue;
        
        offset += snprintf( fullPath + offset, PATH_MAX - min(offset, size_t(PATH_MAX)), "/%s", word);
        fullPath[sizeof(fullPath)-1] = '\0';
        
        //Check to see if it exists
        err = stat(fullPath, &statBuf);
        if( err )
        {
            // stop if the file does not exist
            // this is not an error condition
            if( ENOENT == errno)
            {
                loc = kNameInPath;
                break;
            }
            else
                return  GetFileErrorCode(errno);
        }
        
        if( IsDirectory(statBuf))
        {
            loc = kNameInPath;
            continue;
        }
        
        if( not bResolveLastAlias)
            continue;
        
        char * p = realpath( fullPath, storage);
        if(p)
            strncpy(fullPath, p, sizeof(fullPath));
        
        loc = kNameInFSS;
        break;
    }

    switch(loc)
    {
        case kNameInSpec:
            break; // nothing happened
        case kNameInPath:
            //directory
            dirID = ::open( fullPath, O_RDONLY | O_DIRECTORY);
            if( -1 == dirID)
            {
                dirID = DTSFileSpecPriv::kNoDirectory;
                p->SetVolDir( DTSFileSpecPriv::kDefaultVolume, dirID);
                return GetFileErrorCode(errno);
            }
            
            p->SetFileName((char*) NULL);
            break;
        case kNameInFSS:
            word = basename_r(fullPath, storage);
            if(word)
                p->SetFileName(word);
            word = dirname_r(fullPath, storage);
            if( word )
            {
                dirID = ::open( word, O_RDONLY | O_DIRECTORY);
                if( -1 == dirID)
                {
                    dirID = DTSFileSpecPriv::kNoDirectory;
                    p->SetVolDir( DTSFileSpecPriv::kDefaultVolume, dirID);
                    return GetFileErrorCode(errno);
                }
            }
            break;
    }

    p->SetVolDir( DTSFileSpecPriv::kDefaultVolume, dirID);
    p->SetResolved(true);
    return noErr;
}

DTSError
ResolvePaths( DTSFileSpecPriv * p, bool bResolveLastAlias  =true  )
{
    if ( p->IsResolved() )
        return noErr;
    
    return ResolvePathsPlus( p, bResolveLastAlias );
}


// changes the current directory using path in fileName
DTSError DTSFileSpec::SetDir()
{
    DTSFileSpecPriv * p = priv.p;
    if ( not p )
        return -1;
    
    DTSError result = ResolvePaths( p );
    if ( noErr == result )
        {
        // after successful resolution, the volume & directory portions
        // of the filename should now be encoded in the vRefNum and dirID.
        // So, if the filename isn't empty, we have ipso facto failed to
        // fully resolved the desired path, which is an error.
        if ( p->HasFileName() )
            result = -1;
        }
    if ( noErr == result )
        result = SetDirNoPath();
    
    return result;
}

// creates a new folder
DTSError DTSFileSpec::CreateDir()
{
    int err = mkdir( priv.p->GetFileName(), S_IRWXU);
    if( 0 != err)
        return GetFileErrorCode(errno);
    
    return noErr;
}

class InitCurrentPath
{
public:
    InitCurrentPath();
};
InitCurrentPath gCurrentPathInitializer;

InitCurrentPath::InitCurrentPath()
{
    struct stat statBuf = {0};
    int err = stat(PROJECT_DIR, &statBuf);
    if( 0 == err && S_ISDIR(statBuf.st_mode))
        strncpy( gCurrentDirPath, PROJECT_DIR, sizeof(gCurrentDirPath));
    else
    {
        getcwd(gCurrentDirPath, sizeof(gCurrentDirPath));
        LOG_WARNING("Warning: compiler preprocessor macro PROJECT_DIR \"" PROJECT_DIR "\" is not a valid directory.\n\tUsing: \"%s\" instead.", gCurrentDirPath);
    }
}

DTSError SetWorkingDirectory( int dirID, const char * auxPath)
{
    int err = -1;
    char path[PATH_MAX] = "";
    char * dirPath = path;
    struct stat statBuf = {0};
    if( dirID != -1 )
    {
        err = fcntl(dirID, F_GETPATH, path);
        if( err )
        {
            path[0] = '\0';
            err = errno;
            goto pathComplete;
        }
    }
    else
    {
        if( NULL == auxPath )
            goto pathComplete;
        
        err = stat(auxPath, &statBuf);
        if(err)
            goto pathComplete;

        err = NULL == realpath(auxPath, path);
        if( err )
            goto pathComplete;

        if( ! IsDirectory(statBuf))
            dirPath = dirname(path);
    }


pathComplete:
    if( err )
        dirPath = getcwd(path, sizeof(path));
    
    strncpy(gCurrentDirPath, dirPath, sizeof(gCurrentDirPath));
    return noErr;
}

// sets to the current directory including fileName
DTSError DTSFileSpec::GetCurDir()
{
    int dirFd = ::open( gCurrentDirPath, O_RDONLY | O_DIRECTORY);
    if( dirFd == - 1)
        return GetFileErrorCode(errno);

    priv.p->SetVolDir(1, dirFd);
    return noErr;
}


const char * DTSFileSpec::GetFileName() const
{
    return priv.p->GetFileName();
}

/*
**    DTSFileSpec::SetDirNoPath()
**
**    set the current volume and directory
**    ignore anything in fileName
**
**    This function probably owes its existence to the traditional misfeature of GetCurDir(),
**    mentioned above.  Now that that's been fixed, SetDir() probably works in all cases,
**    and this function could very likely be excised as redundant.  But right now,
**    SetDir() is implemented in terms of SetDirNoPath(), so maybe not.
*/
DTSError DTSFileSpec::SetDirNoPath() const
{
    const DTSFileSpecPriv * p = priv.p;
    if ( not p )
        return -1;
    
    FSVolumeRefNum vol = p->GetVolume();
    UInt32 dir = p->GetDirectory();
    
    if ( vol )    // paranoia
    {
        int err = SetWorkingDirectory(dir, NULL);
        if( err )
            return GetFileErrorCode(errno);
    }
    
    return noErr;
}

#pragma mark -

DTSFileSpecPriv::DTSFileSpecPriv() :
    fileName(NULL),
    mState(fsUnresolved),
    volume(kDefaultVolume),          // POSIX is a flat file system so just set it to 1 and ignore it
    directory(kNoDirectory)
{
}

DTSFileSpecPriv::~DTSFileSpecPriv()
{
    delete fileName; fileName = NULL;
}

DTSFileSpecPriv::DTSFileSpecPriv( const DTSFileSpecPriv& rhs )
    : DTSFileSpecPriv()
{
    SetFileName(rhs.fileName);
    SetVolDir( rhs.GetVolume(), rhs.GetDirectory());
    mState = rhs.mState;
}

DTSFileSpecPriv&    DTSFileSpecPriv::operator=( const DTSFileSpecPriv& rhs )
{
    if( this != &rhs)
    {
        SetFileName(rhs.fileName);
        SetVolDir(rhs.GetVolume(), rhs.GetDirectory() );
        mState = rhs.mState;
    }
    return *this;
}

// unicodify these!
void DTSFileSpecPriv::SetFileName( const char * newName, bool bStillResolved /*file state allows for unresolved file names */ )
{
    if( newName == fileName)
        return;
    delete fileName;    fileName = NULL;
    if( NULL == newName || newName[0] == '\0')
        return;
        
    size_t len = strlen(newName) + sizeof('\0');
    fileName = new char[len];
    if( fileName)
        memcpy( fileName, newName, len);
    
    mState = bStillResolved ?
                ( fileName[0] ? fsPartial : fsResolved )
                : fsUnresolved;
}

void DTSFileSpecPriv::SetFileName( ConstHFSUniStr255Param name, bool bStillResolved )
{
    static const HFSUniStr255 emptyString = {0};
    if(NULL == name )
        name = &emptyString;
    
    uint16_t scratch[256];
    memset(scratch, 0, sizeof(scratch));
    size_t scratchSize = sizeof(name->unicode[0]) * name->length;
    memcpy(scratch, &name->unicode[0], scratchSize);
    scratch[name->length] = 0;
    
    uint8_t utf8FilePath[PATH_MAX];
    size_t destSize = sizeof(utf8FilePath);
    
    iconv_t cd = iconv_open("UTF-8", "UTF-16LE"); // From 16-bit to 8-bit
    char * src = (char*) scratch;
    char * dest = (char*) utf8FilePath;
    
    size_t err = iconv(cd, &src, &scratchSize, &dest, &destSize);
    iconv_close(cd);
    
    if( size_t(-1L) == err)
        SetFileName( (char*) utf8FilePath );
}

FILE * DTSFileSpecPriv::fopen( const char * permissions)
{
    if( NULL == fileName )
        return NULL;
    
    char fullPath[PATH_MAX] = "";
    if( kNoDirectory == directory )
    {
        directory = ::open(gCurrentDirPath, O_RDONLY | O_DIRECTORY );
        if( -1 == directory )
            return NULL;
    }

    // Get path for directory
    int err = fcntl(directory, F_GETPATH, fullPath);
    if( err )
        return NULL;
    
    // Append filename to it
    size_t where = strlen(fullPath);
    if( where >= PATH_MAX)
        return NULL;
    snprintf( fullPath + where, PATH_MAX - where, "/%s", GetFileName());
    
    // open the file
    return ::fopen( fullPath, permissions);
}


#pragma mark -

// Copied from original DTS file, which had some problems

/*
**    File-unix.cp        dtslib2
**
**    Copyright 2023 Delta Tao Software, Inc.
**
**    Licensed under the Apache License, Version 2.0 (the "License");
**    you may not use this file except in compliance with the License.
**    You may obtain a copy of the License at
**
**        https://www.apache.org/licenses/LICENSE-2.0
**
**    Unless required by applicable law or agreed to in writing, software
**    distributed under the License is distributed on an "AS IS" BASIS,
**    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**    See the License for the specific language governing permissions and
**    imitations under the License.
*/


//#include "File-dts.h" // BUGFix: no such file
#include "File_dts.h"
#define        kOpenMagic          (00400|00200|00040|00020)

/*
**    DTS_create()
**
**    create a file
**    platform dependent
*/
DTSError
DTS_create( DTSFileSpec * spec, ulong fileType ) // BUGFIX: fix fileType type
{
    fileType = 0;    // not used
    
    DTSError result = noErr;
    
    int fd = open( spec->GetFileName(), O_RDWR|O_CREAT, kOpenMagic );  // BUGFix: spec referenced missing field
    if ( fd < 0 )
        {
        result = kUnableToCreateFile;
        }
    else
        {
        close( fd );
        }
    
    return result;
}


/*
**    DTS_open()
**
**    open a file
**    platform dependent
*/
DTSError
DTS_open( DTSFileSpec * spec, bool bWritePerm, int * fileref )   // BUGFIX: change fileref bWritePerm type
{
    DTSError result = noErr;
    
    int perm = O_RDONLY;
    if ( bWritePerm )
        {
        perm = O_RDWR;
        }
    
    int fd = open( spec->GetFileName(), perm );         // BUGFix: spec referenced missing field
    if ( fd <= 0 )
        {
        extern int errno;
        if ( errno == 2 )
            {
            result = fnfErr;
            }
        else
            {
            result = kCouldNotOpenFile;
            }
        }
    
    *fileref = fd;
    
    return result;
}


/*
**    DTS_close()
**
**    close a file
**    platform dependent
*/
void
DTS_close( int fileref ) // BUGFIX: change fileref type
{
    if ( fileref >= 0 )
        {
        close( fileref );
        }
}


/*
**    DTS_seek()
**
**    seek to the position in the file
**    platform dependent
*/
DTSError
DTS_seek( int fileref, ulong position ) // BUGFIX: change fileref position type
{
    DTSError result = noErr;
    long newpos = lseek( fileref, position, SEEK_SET );
    if ( newpos < 0 )
        {
        result = kCouldNotSeek;
        }
    return result;
}


/*
**    DTS_seteof()
**
**    set the end of the file
**    platform dependent
*/
DTSError
DTS_seteof( int fileref, ulong eof ) // BUGFIX: change fileref eof type
{
    // BUGFIX:  DTS never implemented this
    if ( ::ftruncate(fileref, static_cast<off_t>(eof)) != 0 )
        {
            // Map common POSIX errors to your library's codes
            if (errno == EBADF) {
                return fnOpnErr; // Bad or unopen file descriptor
            }
            if (errno == EFBIG) {
                return kBadSizeParam;   // File too large for filesystem limits
            }
            if (errno == EINVAL) {
                return kBadSizeParam;   // Invalid file descriptor state / size argument
            }
            
            return kCouldNotTruncate;   // Fallback error from your File_dts.h
        }
    return noErr;
}


/*
**    DTS_geteof()
**
**    get the end of the file
**    platform dependent
*/
DTSError
DTS_geteof( int fileref, ulong * peof ) // BUGFIX: change fileref peof type
{
    DTSError result = noErr;
    long eof = lseek( fileref, 0, SEEK_END );
    if ( eof < 0 )
        {
        result = kCouldNotSeek;
        eof = 0x7FFFFFFF;
        }
    *peof = eof;
    return result;
}


/*
**    DTS_read()
**
**    read data
**    platform dependent
*/
DTSError
DTS_read( int fileref, void * buffer, unsigned long size ) // BUGFIX: change fileref size type
{
    // do the fread
    long numread = read( fileref, (char *) buffer, size );
    
    // convert to an error code
    // anything less than the largest negative number
    // and anything where we didn't read the whole record
    DTSError result = noErr;
    if ( numread < (long) 0xFFFF8000 )
        {
        result = kCouldNotRead;
        }
    else
    if ( numread != size )
        {
        result = kCouldNotRead;
        }
    
    return result;
}


/*
**    DTS_write()
**
**    write data
**    platform dependent
*/
DTSError
DTS_write( int fileref, const void * buffer, unsigned long size ) // BUGFIX: change fileref size type
{
    // do the fwrite
    long numwritten = write( fileref, (const char *) buffer, size );
    
    // convert to an error code
    // anything less than the largest negative number
    // and anything where we didn't write the whole record
    DTSError result = noErr;
    if ( numwritten < (long) 0xFFFF8000
    || ( numwritten >= 0
    &&   numwritten != size ) )
        {
        result = kCouldNotWrite;
        }
    
    return result;
}

