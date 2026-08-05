//
//  AppDelegate.m
//  DibbunsMacroMadnessApp
//
//  Created by Dibbun on 8/1/26.
//

#import "AppDelegate.h"

@interface AppDelegate ()


@end

@implementation AppDelegate

void SetupMultiButtonMouseMonitoring(void) {
    // Combine masks for Left, Right, and "Other" (Buttons 2 through 31)
    NSEventMask mouseMask = NSEventMaskLeftMouseDown   | NSEventMaskLeftMouseUp   |
                            NSEventMaskRightMouseDown  | NSEventMaskRightMouseUp  |
                            NSEventMaskOtherMouseDown  | NSEventMaskOtherMouseUp;

    [NSEvent addGlobalMonitorForEventsMatchingMask:mouseMask handler:^(NSEvent *event) {
        NSInteger button = [event buttonNumber]; // Returns 0, 1, 2, 3, 4, 5...
        BOOL isDown = (event.type == NSEventTypeLeftMouseDown  ||
                       event.type == NSEventTypeRightMouseDown ||
                       event.type == NSEventTypeOtherMouseDown);

        NSPoint location = [NSEvent mouseLocation];

        NSLog(@"Mouse Button %ld %@ at (%.0f, %.0f)",
              (long)button,
              isDown ? @"DOWN" : @"UP",
              location.x,
              location.y);

        // Example: Specific logic per button
        switch (button) {
            case 0: /* Left Click */ break;
            case 1: /* Right Click */ break;
            case 2: /* Middle Click */ break;
            case 3: /* Side Button 1 (e.g. Back) */ break;
            case 4: /* Side Button 2 (e.g. Forward) */ break;
            case 5: /* Button 6 (e.g. Extra Side / DPI) */ break;
            default: break;
        }
    }];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


- (BOOL)applicationSupportsSecureRestorableState:(NSApplication *)app {
    return YES;
}


@end
