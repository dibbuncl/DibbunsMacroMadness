//
//  DMM_Carbon.h
//  DibbunsMacroMadness
//
//  Created by Dibbun on 7/28/26.
//

#include "DMM_prefix.hpp"

#ifndef DMM_Carbon_h
#   define DMM_Carbon_h 1

typedef uint16_t    UInt16;
typedef UInt16      ThemeFontID;

#ifdef __OBJC__
#   define __MACERRORS__
enum {
    paramErr = -50, /* error in user parameter list */
    memFullErr = -108, /* Not enough room in heap zone */
    fnOpnErr                = -38,
    fnfErr                    = -43,
    dupFNErr                = -48,
    wrPermErr                = -61,
};
#else

#include <MacTypes.h>
// Basic types
typedef uint8_t         uchar;
typedef uint16_t        ushort;
typedef uint32_t        uint;
typedef unsigned long   ulong;      // legacy from 32 bit archs
typedef int32_t         SInt32;
typedef uint32_t        UInt32;
typedef FourCharCode    OSType;
typedef OSType          DescType;
typedef unsigned long   NSUInteger;

//Missing Carbon bits:
struct QE {
    struct QE *qLink;  /* Link to next element */
    int16_t qType;       /* Type of queue element (e.g., vType, ioQType) */
    char8_t qData[1];     /* Beginning of the actual data */
};
typedef struct QE QElem;
typedef QElem * QElemPtr;

struct QHdr {
    int16_t qFlags;      /* Queue flags (e.g., lock status) */
    QElemPtr qHead;    /* Pointer to the first element (QElem) */
    QElemPtr qTail;    /* Pointer to the last element (QElem) */
};
typedef struct QHdr QHdr;

class CarbonEventResponder
{
    
};

typedef long FSVolumeRefNum;
struct HFSUniStr255 {
    uint16_t length;         /* The actual length of the string */
    uint16_t unicode[255];   /* Array of UTF-16 code units holding the characters */
};
typedef struct HFSUniStr255 HFSUniStr255;
typedef const HFSUniStr255 *ConstHFSUniStr255Param;
typedef struct OpaqueNavContext* NavContext;
typedef struct OpaqueWindowPtr* WindowRef;

typedef UInt16 EventKind;
typedef UInt16 EventModifiers;
struct EventRecord {
    EventKind       what;      /* What type of event occurred (e.g., mouseDown, keyDown) */
    UInt32          message;   /* Event-specific payload data (e.g., which key was pressed) */
    UInt32          when;      /* Timestamp in system ticks (1/60th of a second since startup) */
    Point           where;     /* Global mouse position coordinates at the time of the event */
    EventModifiers  modifiers; /* Bitmask of modifier keys (e.g., Shift, Option, Command, mouse button state) */
};
typedef struct EventRecord EventRecord;

struct NavCBRec {
    UInt16          version;        /* Structure version identifier */
    NavContext      context;        /* Opaque reference to the active dialog */
    WindowRef       window;         /* The Window Manager reference to the dialog window */
    Rect            customRect;     /* Bounds of the developer's custom control area */
    Rect            saveFileNameRect;/* Location of the file name text box (for Save dialogs) */
    EventRecord     eventRecord;    /* The low-level operating system event data */
    // ... additional configuration or event parameter fields
};
typedef struct NavCBRec NavCBRec;
typedef NavCBRec *NavCBRecPtr;

typedef SInt32 NavEventCallbackMessage;
enum
{
    kNavCBEvent = 1,
    kNavCBStart,
    kNavCBTerminate,
    kNavCBAdjustRect,
    kNavCBUserAction
};

typedef SInt16 NavFilterModes;
enum
{
    kNavFilteringBrowserList = 1,
    kNavFilteringFavoritesList,
    kNavFilteringRecentList,
    kNavFilteringShortCutList,
};

typedef Handle AEDataStorage;

struct AEDesc {
    DescType        descriptorType; /* A 4-character code (OSType) indicating the data type */
    AEDataStorage   dataHandle;     /* A handle (pointer to a pointer) to the actual data */
};
typedef struct AEDesc AEDesc;

struct FolderInfo {
    Rect    windowBounds; /* The position and dimensions of the folder's window on screen */
    UInt16  finderFlags;  /* Boolean attribute flags (e.g., is it invisible, custom icon, etc.) */
    Point   location;     /* The folder's physical icon coordinates inside its parent window */
    UInt16  reservedField;/* Reserved for system/future OS extensions */
};
typedef struct FolderInfo FolderInfo;

struct FileInfo {
    OSType  fdType;       /* The 4-character File Type code (e.g., 'TEXT', 'APPL') */
    OSType  fdCreator;    /* The 4-character Creator/Application signature code */
    UInt16  finderFlags;  /* Attribute bitmask flags (e.g., hidden, locked, alias) */
    Point   location;     /* The file icon's physical coordinates on the desktop */
    UInt16  reservedField;/* Reserved for internal system extensions */
};
typedef struct FileInfo FileInfo;

struct NavFileOrFolderInfo {
    UInt16       version;         /* Format version identifier */
    Boolean      isFolder;        /* True if the item is a folder/directory; False if a file */
    Boolean      isPackage;       /* True if the item is an OS X bundle/package (like an app) */
    
    union {
        FolderInfo  folderInfo;   /* Macintosh Finder metadata specifically for folders */
        FileInfo    fileInfo;     /* Macintosh Finder metadata specifically for files */
    } fileAndFolder;
};
typedef struct NavFileOrFolderInfo NavFileOrFolderInfo;

#include <CoreFoundation/CoreFoundation.h>

typedef enum : UInt32 {
    kNavDefaultNavDlogOptions = 0x000000E4,
    kNavAllowMultipleFiles = 0x00000080,
    kNavDontConfirmReplacement = 0x00000010,
    kNavNoTypePopup = 0x00000001,
    kNavAllowPreviews = 0x00000040
}NavDialogOptionFlags;
typedef SInt32 WindowModality;

struct NavDialogCreationOptions {
    UInt16               version;           /* Format version identifier */
    NavDialogOptionFlags optionFlags;       /* Feature bitmask flags (e.g., multi-select) */
    Point                location;          /* Screen coordinate placement (v, h) */
    CFStringRef          clientName;        /* Custom application identifier name string */
    CFStringRef          windowTitle;       /* Text displayed in the window header frame */
    CFStringRef          actionButtonLabel; /* Custom text overriding the "Open"/"Save" button */
    CFStringRef          cancelButtonLabel; /* Custom text overriding the "Cancel" button */
    CFStringRef          saveFileName;      /* Default name filled into a Save text box */
    CFStringRef          message;           /* Informative banner/prompt layout string */
    UInt32               preferenceKey;     /* Key used to save the last window state/size */
    CFArrayRef           popupExtension;    /* Custom choices appended to the format pop-up */
    WindowModality       modality;          /* Interaction locking bounds behavior */
    WindowRef            parentWindow;      /* Parent sheet window frame reference */
    char                 reserved[16];      /* Pad fields allocated for future system growth */
};
typedef struct NavDialogCreationOptions NavDialogCreationOptions;

typedef SInt32 NavAskSaveChangesAction;

#endif /* __OBJC__ */

#endif /* DMM_Carbon_h */
