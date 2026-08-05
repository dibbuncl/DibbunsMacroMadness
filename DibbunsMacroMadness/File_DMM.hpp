//
//  File_DMM.hpp
//  DibbunsMacroMadness
//
//  Created by Dibbun on 7/29/26.
//

#ifndef File_DMM_hpp
#define File_DMM_hpp 1

#include "File_dts.h"

// I would name this under the DMM tree, but the original source includes it as such
class DTSFileSpecPriv
{
private:
    char * fileName;
    int  volume;
    int  directory;
    enum RState
        {
        fsUnresolved,        // Filespec status is not known
        fsResolved,            // it definitely points to an object, and the name is empty
        fsPartial            // vol/dir are good; name may or may not point to something
        };
    RState        mState;
    
public:
                        DTSFileSpecPriv();
                        ~DTSFileSpecPriv();
                        DTSFileSpecPriv( const DTSFileSpecPriv& rhs );
    DTSFileSpecPriv&    operator=( const DTSFileSpecPriv& rhs );
    
    // unicodify these!
    inline const char * GetFileName() const { return NULL == fileName ? "" : fileName; }
    inline bool         HasFileName() const { return NULL != fileName && fileName[0] != '\0';}
    void                SetFileName( const char * newName, bool bStillResolved = false );
    void                SetFileName( ConstHFSUniStr255Param, bool bStillResolved = false );
    
    inline void         SetVolDir( FSVolumeRefNum vol, UInt32 dir )
                        {
                            volume = vol;
                            directory = dir;
                        }
    inline UInt32       GetDirectory() const { return directory; }
    inline FSVolumeRefNum   GetVolume() const { return volume; }
    
    inline void         SetResolved( bool isResolved )
                        {
                            mState = isResolved ? fsResolved :
                            (fileName && fileName[0]) ? fsPartial : fsUnresolved;
                        }
    inline bool         IsResolved() const { return mState == fsResolved; }
    
//    DTSError            CopyToRef( FSRef * outRef, HFSUniStr255 * outName, bool bResolve = true );
//    DTSError            CopyToRef( FSRef * outRef, bool bResolve );
//    DTSError            CopyFromRef( const FSRef * ref );
//    CFURLRef            CreateURL();
//    DTSError            SetFromURL( CFURLRef u );
    
    FILE *              fopen( const char * permissions);
    
    
    constexpr static int kNoDirectory = 0;    // strictly speaking this fileDescriptor is stdout. Legacy code used 0 for no directory, so we will do that here.
    constexpr static int kDefaultVolume = 1;  // POSIX is a flat file system so just set it to 1 and ignore it
};

static inline DTSError GetFileErrorCode( int _errno )
{
    switch(_errno)
    {
        case 0:
            return noErr;
        case EBADF:
            return fnOpnErr;
        case EACCES: // Write permission is denied for the parent directory.;  Search permission is denied for a component of the path prefix.
        case EROFS: // The parent directory resides on a read-only file system.
            return wrPermErr;
        case EFAULT:  // Path points outside the process's allocated address space.
            return kEntryDoesNotExist; // could also be Path points outside the process's allocated address space.
        case EIO:
            return kCouldNotRead;
        case ENAMETOOLONG: // A component of a pathname exceeded {NAME_MAX} characters, or an entire path name exceeded {PATH_MAX} characters.
        case ENOSPC:  // The new directory cannot be created because there is no space left on the file system that would contain it.; There are no free inodes on the file system on which the directory is being created.
            return kUnableToAllocMem;
        case ENOENT: //  A component of the path prefix does not exist or path is an empty string.
        case ENOTDIR: // A component of the path prefix is not a directory.
            return fnfErr;
        case EDQUOT:
            return kUnableToCreateFile;
        case EEXIST:
            return dupFNErr;
        case ELOOP:    // Too many symbolic links were encountered in translating the pathname.  This is taken to be indicative of a looping symbolic link.
        case EISDIR:   // The named file is the root directory.
        case EMLINK:   // The parent directory already has {LINK_MAX} links.
        case EILSEQ:  // The filename does not match the encoding rules.
        default:
            LOG_ERROR("GetFileErrorCode: received errno (%d)\n", _errno);
            return -1;
    }
    
/*
 unused as yet

    kCouldNotOpenFile        = -31999,
    kReopenPermissionErr    = -31998,
    kBadHeaderVersion        = -31997,
    kBadSizeParam            = -31994,
    kUnableToCreateFile        = -31993,
    kKeyFileNotOpen            = -31992,
    kCouldNotWrite            = -31990,
    kCouldNotSeek            = -31989,
    kCouldNotGetPos            = -31988,
    kCouldNotTruncate        = -31987
*/
}


#endif /* File_DMM_hpp */
