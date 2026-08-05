//
//  ViewController.m
//  DibbunsMacroMadnessApp
//
//  Created by Dibbun on 8/1/26.
//

#import "ViewController.h"
#include "DMM_Carbon.h"
#include "Clanlord.h"
#include "MacroDefs_cl.h"
#include "DTSFileSpecExtended.hpp"
#include <sys/stat.h>
#include "File_dts.h"
#include "DMMMacros_cl.hpp"

@implementation ViewController


static DTSError GetMacroPath( char path[PATH_MAX])
{
    // Save current working directory
    DTSFileSpecExtended mainDir;
    (void) mainDir.GetCurDir();
    
    // Figure out where the macro path is
    DTSFileSpecExtended macroDir = mainDir;
    macroDir.SetFileName( kCLMacros_FolderName );
    macroDir.SetDir();
    DTSError err = macroDir.GetPath(path);
    
    // Restore current working directory
    mainDir.SetDirNoPath();
    
    return err;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    // Set the path string
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    // Set up a value transformer for the Load button.
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(userDefaultsDidChange:)
                                                 name:NSUserDefaultsDidChangeNotification
                                               object:nil];

        // Set initial title
    [self updateLoadButtonTitle];
    
    

    // Only set default if the user hasn't saved a custom macroPath yet
    if (![defaults stringForKey:@"macroPath"]) {
        int err = noErr;
        char defaultMacroPath[PATH_MAX] = "";
        snprintf(defaultMacroPath, sizeof(defaultMacroPath), "%s/TestMacros/Default", PROJECT_DIR);

        if (noErr == err) {
            NSString *defaultPath = [NSString stringWithCString: defaultMacroPath
                                                       encoding: NSUTF8StringEncoding];
            
            // Setting NSUserDefaults will automatically update pathLabel via your binding!
            [defaults setObject:defaultPath forKey:@"macroPath"];
        }
    }
}

-(void) userDefaultsDidChange: (id) sender
{
    dispatch_async(dispatch_get_main_queue(), ^{
        [self updateLoadButtonTitle];
    });
}

-(void) updateLoadButtonTitle
{
    NSString *path = [[NSUserDefaults standardUserDefaults] stringForKey:@"macroPath"];
    NSFileManager *fileManager = [NSFileManager defaultManager];
    
    BOOL isDir = NO;
    BOOL fileExists = [fileManager fileExistsAtPath:path isDirectory:&isDir];
    
    // Check that it exists AND is a file (not a folder)
    if (path.length > 0 && fileExists && !isDir)
    {
        self.loadButton.title = [NSString stringWithFormat:@"Load %@", [path lastPathComponent]];
        self.loadButton.enabled = YES;
    }
    else
    {
        self.loadButton.title = @"Load Macros";
        self.loadButton.enabled = NO;
    }
}

- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}

-(IBAction) setMacroPath: (id) sender
{
    NSOpenPanel *openPanel = [NSOpenPanel openPanel];
    [openPanel setCanChooseFiles:YES];
    [openPanel setCanChooseDirectories:YES];
    [openPanel setAllowsMultipleSelection:NO];
    [openPanel setPrompt:@"Select the top level macro file."];
    
    //Set the old path
    NSString *currentPath = [[NSUserDefaults standardUserDefaults] stringForKey:@"macroPath"];
    if (!currentPath || currentPath.length == 0)
        currentPath = self.pathLabel.stringValue;
        
    if (currentPath.length > 0)
    {
        NSURL *initialURL = [NSURL fileURLWithPath: currentPath];
            
        // If currentPath points to a file, point directoryURL to its parent directory
        BOOL isDir = NO;
        if ([[NSFileManager defaultManager] fileExistsAtPath:currentPath isDirectory:&isDir] && !isDir) {
            initialURL = [initialURL URLByDeletingLastPathComponent];
        }
        
        [openPanel setDirectoryURL:initialURL];
    }
    
    NSWindow *window = self.view.window;
    if (!window)
        return;
    
    [openPanel beginSheetModalForWindow:window
                      completionHandler:^(NSModalResponse response)
     {
        if (response == NSModalResponseOK) {
            NSURL *url = [openPanel URL];
            NSString *path = [url path];
            
            // Save to NSUserDefaults (automatically updates your bound UI label)
            [[NSUserDefaults standardUserDefaults] setObject:path forKey:@"macroPath"];
            
            
            [self.loadButton setTitle: [NSString stringWithFormat: @"Load %@", url.lastPathComponent]];
        }
    }];
}


// The game would launch itself with InitMacro(). If you want to test
// the full pathway including finding the default macro directory, installing
// default macro files in it, and then running the character's macro, you can
// run that.  For testing, this is easier.
-(IBAction) loadMacroFile: (id) sender
{
    DTSError result;
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    const char  * path = [[defaults stringForKey:@"macroPath"] cStringUsingEncoding: NSUTF8StringEncoding];
    int err = -1;
    
    if( path )
    {
        struct stat statBuf;    memset( &statBuf, 0, sizeof(statBuf));
        err = stat( path, &statBuf);
        if( 0 == err &&  S_ISDIR(statBuf.st_mode))
        {
            NSAlert *alert = [[NSAlert alloc] init];
            alert.messageText = @"Error: Macro file was a directory";
            alert.informativeText = [NSString stringWithFormat: @"path: \"%s\"", path ];
            alert.alertStyle = NSAlertStyleCritical;
            
            // Present as a sheet attached to the current window
            if (self.view.window) {
                [alert beginSheetModalForWindow:self.view.window completionHandler:nil];
            }
            return;
        }
    }
    
    if(err)
    {
        NSAlert *alert = [[NSAlert alloc] init];
        alert.messageText = [NSString stringWithFormat: @"Macro file \"%s\" not opened.", path  ];
        alert.alertStyle = NSAlertStyleCritical;

        char errorString[256] = "";
        strerror_r(errno, errorString, sizeof(errorString));
        alert.informativeText = [NSString stringWithFormat: @"Error: %s (errno: %d)", errorString, errno];

        // Present as a sheet attached to the current window
        if (self.view.window) {
            [alert beginSheetModalForWindow:self.view.window completionHandler:nil];
        }
        return;
    }
    
    // Start from a blank slate
    KillMacros();

    // Save the current working directory
    DTSFileSpec mainDir;
    (void) mainDir.GetCurDir();

    // Set our directory path.
    char pathString[PATH_MAX];
    dirname_r(path, pathString);

    // Find the macro directory
    DTSFileSpec macroDir;
    macroDir.SetFileName( pathString );
    result = macroDir.SetDir();
    if( result )
    {
        CMacro::ShowMacroInfoText( /* "Error finding / creating \"%s\" directory" */
                    _(TXTCL_MACROS_ERRORCREATINGFOLDER), pathString );
        return;
    }

    // isolate the macro file name
    basename_r(path, pathString);
    
    // Parse the macro file
    CMacroParser theMacroParser;
    result = theMacroParser.ParseFile( pathString );
    
    // Restore the current working directory
    mainDir.SetDirNoPath();
    
    [self updateMacroList];
    [self runMacro: @"@login"];
}

-(void) runMacro: (NSString *) macroName
{
    if(nil == macroName )
        return;
    
    if ( CFunctionMacro * macro = DMMFindMacroByName( [macroName cStringUsingEncoding: NSUTF8StringEncoding] ) )
        if ( CExecutingMacro * emacro = DMMStartMacroExecution( macro ) )
            DMMContinue1MacroExecution( emacro );
}

-(void) updateMacroList
{
    NSString *previouslySelectedTitle = self.macroList.titleOfSelectedItem;
    
    NSMutableArray * macroNames = [NSMutableArray new];
    __block BOOL found = NO;
    __block BOOL hasLogin = NO;
    IterateMacros(^unsigned long(const CMacro *macro) {
        if( macro->mKind != CMacroKind::mFunction )
            return 0;
        
        const CFunctionMacro * function = static_cast<const CFunctionMacro*>(macro);
        NSString * title = [NSString stringWithCString: function->mName.Get()
                                              encoding: NSASCIIStringEncoding];
        found |= [title isEqualToString: previouslySelectedTitle];
        hasLogin |= [title isEqualToString: @"@login"];
        [macroNames addObject: title];
        return 1;
    });
    
    [macroNames sortUsingSelector:@selector(caseInsensitiveCompare:)];

    [self.macroList removeAllItems];
    if( 0 == macroNames.count )
    {
        self.goButton.enabled = NO;
        self.macroList.enabled = NO;
        self.runAllTestsButton.enabled = NO;
        self.numpad0.enabled = NO;
        self.numpad1.enabled = NO;
        self.numpad2.enabled = NO;
        self.numpad3.enabled = NO;
        self.numpad4.enabled = NO;
        self.numpad5.enabled = NO;
        self.numpad6.enabled = NO;
        self.numpad7.enabled = NO;
        self.numpad8.enabled = NO;
        self.numpad9.enabled = NO;
        return;
    }

    [self.macroList addItemsWithTitles: macroNames];
    if(found)
        [self.macroList selectItemWithTitle: previouslySelectedTitle];
    else if( hasLogin)
        [self.macroList selectItemWithTitle: @"@login" ];
    else
        [self.macroList selectItemAtIndex: 0];
    
    self.macroList.enabled = YES;
    self.goButton.enabled = YES;
    self.runAllTestsButton.enabled = NULL != DMMFindMacroByName( "/RunAllTests");
    self.numpad0.enabled = NULL != GetMacroForKey( '0', kKeyModNumpad);
    self.numpad1.enabled = NULL != GetMacroForKey( '1', kKeyModNumpad);
    self.numpad2.enabled = NULL != GetMacroForKey( '2', kKeyModNumpad);
    self.numpad3.enabled = NULL != GetMacroForKey( '3', kKeyModNumpad);
    self.numpad4.enabled = NULL != GetMacroForKey( '4', kKeyModNumpad);
    self.numpad5.enabled = NULL != GetMacroForKey( '5', kKeyModNumpad);
    self.numpad6.enabled = NULL != GetMacroForKey( '6', kKeyModNumpad);
    self.numpad7.enabled = NULL != GetMacroForKey( '7', kKeyModNumpad);
    self.numpad8.enabled = NULL != GetMacroForKey( '8', kKeyModNumpad);
    self.numpad9.enabled = NULL != GetMacroForKey( '9', kKeyModNumpad);
}

-(IBAction) goHit: (id) sender
{
    [self runMacro: [self.macroList selectedItem].title ];
}

-(IBAction) runAllTests: (id) sender
{
    [self runMacro: @"/RunAllTests"];
}


-(IBAction) numpadHit: (id) sender
{
    if( NO == [sender isKindOfClass: [NSControl class]])
    {
        LOG_ERROR("Invalid control calling numpadHit: %p", sender);
        return;
    }
    
    NSControl * c = (NSControl*) sender;
    NSInteger numPadValue = c.tag;
    
    static const uint8_t keyCodes[10] = {0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5b, 0x5c};
    if( numPadValue >= 0 && numPadValue <= 9)
    {
        int key = '0' + numPadValue;
        DoMacroKey(&key, kKeyModNumpad);
    }
}

@end
