# Dibbun's Macro Madness

## Abstract 
This provides a set of conformance tests which document old client behavior and help make 
sure your new client under development is doing what the old one did.

## Rationale
Since the old clanlord client is stuck in the '90s using MacOS 9 / Carbon APIs and isnt
runnable on any recent macOS, we've had a bunch of private clients to port the functionality 
forward to PC / macOS X / personal OS _du jour_.  This has the potential to lead to a lot 
of macro incompatibilities between platforms making it harder to pick your favorite client. 
Not everyone wants to be a E1i7e Macro HaXXor testing on half a dozen clients.  Users should 
be able to expect each implementation behaves the same. If you are writing a client, this
is for you. You may think of this as:

* A set of correctness tests which capture old client behavior  (See: TestMacros)
* A runtime to answer any further questions you may have about the old behavior
* A central location where multiple client authors can collaborate on defining
    what the behavior should be, and if necessary help drive changes back into
    the original client.

## LICENSE
This work (not including the Clanlord(TM) submodule owned by Delta Tao Software, Inc) is covered
by the MIT open source license, with additions to grant trademark license for Dibbun's 
Macro Madness (DMM) as needed. This allows for the broadest possible use of this software,
without worry that it will be encumbered with IP restrictions. See LICENSE.md.

## Contents
1. Clanlord     

> The original clanlord source as a git submodule. 
> **Clarification:** _We DO NOT republish or fork CL source here.
    Think of the git submodule as a link back to the original
    YappyGM repo here https://github.com/YappyGM/ClanLordClient
    It is downloaded as part of the git checkout as a convenience
    since several of the original files are used in this project._
    
2. DibbunsMacroMadness (DMM)

>   A harness which reimplements portions of the ClanLord code (particularly
    for file access and printing out strings) to use POSIX and macOS APIs as 
    required. 
    
No original ClanLord files are altered as part of this work. In certain cases, 
DMM .cp files include the Clanlord cp files as a way of breaking encapsulation
so necessary static code and data can be accessed.

## What if I just want to test MY client?
You, my friend, are just looking for the files in the TestMacros subdirectory. It will define 
a bunch of tests, wired up to hotkeys in the numpad. Should be pretty easy to run. 

### Tests
If you want to run them all, you may use the /runalltests command. There are also /runtest1, /runtest2, 
etc. wired up to approprate Numpad-\<test number\> hot keys.

### Test0
Test 0 is a special test to make sure you can test. It verifies basic if end if is working 
and you can call message. Since if basic test machinery is broken, it can't really verify itself,
you will need to look to the comments in that test and make sure the output of the test is 
what it should be. If test0 is not working, then the rest of the tests are invalid.

### Results reporting
The /runalltests should sum up results and provide a summary string, which you may
report in Clanlord discord or other places as desired for marketing purposes.

## To Use
### MacOS X with Xcode:

a. **IMPORTANT!!** You probably didn't clone with --recurse-submodules. 
    	To fix, do this in the terminal:
        
> `cd` _\<path/to/DibbunsMacroMadness\>_

> `git submodule update --init --recursive`
        
> This will cause YappyGM's Clanlord code to be downloaded and put in the Clanlord 
        directory. It is needed to build the app.
        
b. click build-and-run (cmd-R) after opening the DimmunsMacroMadness.xcodeproj in Xcode. 
c. Make sure you can see the text spew from the debugger. There are some little squares
    	at the bottom right of the Window. If you can only see `[_]` click that to make 
    	the debugger display visible. Next, make sure `[ |]` is blue. Whether or not `[| ]` 
    	is also on is up to you. It would allow you to see variables stepping through the 
    	C++ code. It doesn't show variables in the macro code.
d. The current UI is a 3 step process.

> 1. (Optional) Set the path to the top level macro file to use. Normally this would be the 
        	file with your character name. We are using TestMacros/Default by default. If 
        	that is fine, then you need do nothing for this step. The Default file points
        	to a bunch of correctness tests for the macro language.
> 2. Click the `[Load <filename>]` button. This will load the macro file, anything 
        	it includes and run @login
> 3. You can run any macro in the file using the menubutton at bottom followed by 
        	the "Go" button. If your code is wired up to `Numpad-[0,9]` then you can click 
        	the appropriate buttons at bottom right or use the keyboard.  I haven't added 
        	all the f-keys yet.
            
d. Handy Environment variables:
```
	        @login
  		    {
            	// Turns off execution timeouts. This might be valuable if you are 
            	//  stepping through macro source and don't want it timing out on you
            	setglobal @env.unfriendly true                     
        
            	// turns on debug spew. Makes it easier to tell what happend
            	//      Can be turned on and off at will to just instrument regions of code
            	setglobal @env.debug true       
        
        	}
```    
e. Key Controls:

* ctrl-esc:    abort running macro     // Macros_cl.cp:3213 (needs to be hooked up)
* Numpad 0-9:	run macros called Numpad-_\[0,9\]_
* Numpad Enter: calls /RunAllTests
* Return:		triggers the default button, "Go" drawn in blue per usual MacUI
    
    
### Other operating Systems:
    * may be missing a build script
    * not yet tested.
    * The original code has dependencies on macOS only APIs, specifically CoreFoundation 
    	and CoreGraphics.  Fixing this would require modifying the original DT code.


## Reporting Bugs / Contacting the Author
Please file bugs against [DibbunsMacroMadness](https://github.com/dibbuncl/DibbunsMacroMadness)
Contact: DibbunCL@icloud.com

See also the MacroGotchas.md in this repository for some commentary for places where macros go wrong and how your client can help make sure they dont


## Other Resources
[Telta Tao Macro Manual](https://www.deltatao.com/clanlord/macros/manual.html)
[Gorvin's Macros](http://gorvin.50webs.com/macros/index.html)

## TODO:
1. Write makefile; update To Use instructions above
2. Tests 3-9

## KNOWN BUGS
1. Reloading in a macros file seems to trigger some caching down in CL somewhere that prevents changes from being observed.
    I haven't taken the time to track this down yet. 


