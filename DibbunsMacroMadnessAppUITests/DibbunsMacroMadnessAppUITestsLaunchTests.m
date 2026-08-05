//
//  DibbunsMacroMadnessAppUITestsLaunchTests.m
//  DibbunsMacroMadnessAppUITests
//
<<<<<<< HEAD
//  Created by Dibbun on 8/1/26.
=======
//  Created by Dibbun on 8/1/26.
>>>>>>> 49a5730 (Initial commit)
//

#import <XCTest/XCTest.h>

@interface DibbunsMacroMadnessAppUITestsLaunchTests : XCTestCase

@end

@implementation DibbunsMacroMadnessAppUITestsLaunchTests

+ (BOOL)runsForEachTargetApplicationUIConfiguration {
    return YES;
}

- (void)setUp {
    self.continueAfterFailure = NO;
}

- (void)testLaunch {
    XCUIApplication *app = [[XCUIApplication alloc] init];
    [app launch];

    // Insert steps here to perform after app launch but before taking a screenshot,
    // such as logging into a test account or navigating somewhere in the app
    // XCUIAutomation Documentation
    // https://developer.apple.com/documentation/xcuiautomation

    XCTAttachment *attachment = [XCTAttachment attachmentWithScreenshot:XCUIScreen.mainScreen.screenshot];
    attachment.name = @"Launch Screen";
    attachment.lifetime = XCTAttachmentLifetimeKeepAlways;
    [self addAttachment:attachment];
}

@end
