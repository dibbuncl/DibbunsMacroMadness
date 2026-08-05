//
//  ViewController.h
//  DibbunsMacroMadnessApp
//
//  Created by Dibbun on 8/1/26.
//

#import <Cocoa/Cocoa.h>

@interface ViewController : NSViewController

@property (weak, nonatomic) IBOutlet NSButton * setPathButton;
@property (weak, nonatomic) IBOutlet NSButton * loadButton;
@property (weak, nonatomic) IBOutlet NSPopUpButton * macroList;
@property (weak, nonatomic) IBOutlet NSButton * goButton;
@property (weak, nonatomic) IBOutlet NSButton * runAllTestsButton;
@property (weak, nonatomic) IBOutlet NSTextField * pathLabel;
@property (weak, nonatomic) IBOutlet NSTextField * characterName;

@property (weak, nonatomic) IBOutlet NSButton * numpad0;
@property (weak, nonatomic) IBOutlet NSButton * numpad1;
@property (weak, nonatomic) IBOutlet NSButton * numpad2;
@property (weak, nonatomic) IBOutlet NSButton * numpad3;
@property (weak, nonatomic) IBOutlet NSButton * numpad4;
@property (weak, nonatomic) IBOutlet NSButton * numpad5;
@property (weak, nonatomic) IBOutlet NSButton * numpad6;
@property (weak, nonatomic) IBOutlet NSButton * numpad7;
@property (weak, nonatomic) IBOutlet NSButton * numpad8;
@property (weak, nonatomic) IBOutlet NSButton * numpad9;


-(IBAction) setMacroPath: (id) sender;
-(IBAction) setCharacterName: (id) sender;
-(IBAction) loadMacroFile: (id) sender;

-(IBAction) numpadHit: (id) sender;
-(IBAction) goHit: (id) sender;
-(IBAction) runAllTests: (id) sender;

-(void) runMacro: (NSString *) macroName;

@end

