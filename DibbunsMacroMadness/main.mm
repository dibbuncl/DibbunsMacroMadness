//
//  main.m
//  DibbunsMacroMadnessApp
//
//  Created by Dibbun on 8/1/26.
//

#import <Cocoa/Cocoa.h>
#include "Macros_cl.h"

int main(int argc, const char * argv[]) {
    bDebugMessages = true;
    
//  Test initing macros the CL way. Note: may add some default files to your Macros directory.
//    InitMacros();

    @autoreleasepool {
        // Setup code that might create autoreleased objects goes here.
    }
    return NSApplicationMain(argc, argv);
}
