# Dibbun's Macro Gotchas

In no particular order, here are some things that tripped me up:

## 1. Local variables aren't
At least in Clieunk, local variables have global scope. (This might be a bug just in Clieunk.) 
That is, if you write:

```
	setglobal Result 0        		// The global version of Result has value 0
	MyFunction
	{
		set Result 1           		// The local version of Result has value 1
      	call MyFunction2       		// This changes local version of Result to 2 -- THIS 
      								//		Result. They are not different!
      	set Result + 10        		// Result is now 12, not 11
      	setglobal Result Result 	// The global version of Result has value 12
   	}
   
	MyFunction
   	{
    	set Result 2               	// The Local version of Result has value 2
      	setglobal Result Result    	// copy local Result (2) to global Result
   	}
```
   
You'll need to study that a bit. The secret decoder ring is that MyFunction::Result and 
MyFunction2::Result are not different variables. So, when MyFunction calls MyFunction2,
the callee and reach back up and change what you probably thought were the caller's 
"private" variables. 

### problems with this situation
1. **Local variables have global scope:** If a global and a local variable share the same 
name, then the global variable is hidden, POTENTIALLY FOR THE WHOLE FILE, and unreadable,
except specifically with the `setglobal <global> .op. <var>` construction is used, and
then only for the first argument. So all your attempts to debug with message or commands
will be feeding you the wrong information.

2. **namespaces:** You have to be careful to add some namespace to the name for local 
variables in subroutines, so they don't accidentally modify the caller's logic. I
personally used this convention `<varName>::<functionName>` but underscores work fine too.
This is like C++ namespaces, but the order of namespace::varName is reversed. While I use
namespaces in C++ fine, having simply everything tagged with namespace first made the
macro code difficult to read, because all the variables looked the same. Swapping the
order helped. Abbreviating the function name also helps, at risk of creating an accidental
namespace duplication with another function.

3. **Recursion doesn't work the way you expect**
Since local variable names reach back and change local variables of the same name
in the caller, any recursion that declares local variables will have this behavior.
It is as if all variables used in the recursion are passed by reference. It is up to 
you to either regard this behavior as cursed, and never use recursion, or (hope your 
client got scope right AND) use it to provide an unexpected replacement for the 
missing loop operator.

```
        // Call using the iterator "iter" set to number of iterations. It must be > 0
        MyRecursiveFunction
        {
            set iter - 1	// decrement iter. Iter will range from [0, orignal value-1]

            // Do whatever you would do in the body of a loop
        
            if iter > 0
                call MyRecursiveFunction
            end if
        }
```
>This is functionally equivalent to:
```
        int iter = loopCount;
        // call MyRecursiveFunction
        for( --iter; iter >= 0; iter--)
        {
            // Do whatever you would do in the body of a loop	
        }
```
>>_For the C language tentative, this is the same as `for(int iter = 0; iter < loopCount;
iter++)` except that the loop runs backward and iter is 0 at the end._

**Notes For Implemetors:**
Most compilers warn if a local variable hides a global variable in scope. You probably 
should too. Not only might local variables hide global ones causing the wrong data to 
be used, but local variables can interfere with remote local variables in other functions
possibly causing infinite loops, which are hard to deal with in a language without the 
possibility to break into a debugger.

_**You could be clever about apparent intent.** If the first appearance of a set or
setglobal in a function is a simple assignment, then it is likely to be a
local variable declaration and intended to be local to the function only.  If the first
appearance is a read-modify-write (e.g. `set a + 1`) the intent is to take advantage of
the global nature of the local variable, and warnings are not appropriate.  This should be 
a warning only, since it can certainly be the case that someone writes this function:_

```
	// Set the caller's variable a (if any) to zero
	21  SetAtoZero
	22  {
	23  	set a 0
	24  }
```

```
	MacroFile:23: warning: variable overwrites caller variable of the same name (if any)
```


## 2. +-/\* only work in a couple places
They work in set and setglobal and only once. You can't write a + b + c on
the same line. If you try, some of it may be silently ignored

**Valid forms for set and setglobal**

```
	// Equivalent to:  var = var2;
	set <var> <var2>       		
	setglobal <var> <var2>      

	// Equivalent to:  var .op.= var2; (e.g. var += var2 or var = var + var2 )
	set <var> .op. <var2>
	setglobal <var> .op. <var2>
```

That is it! Anywhere else you use +-/\* it will probably be silently ignored and triger
a logic error.

**Notes for implementors:**

Please warn if multiple +-/\* operators are used in the same statement. 

```
	set a + b * c
	setglobal a + -b
	set a++
	setglobal a b[i+1]
```

Please warn if a +-/\* operator is used without set or setglobal starting the statement.

```
	a + b 			(a += b, but forgot set or setglobal)
	message "1 + 1 = " 1 + 1 "\r"
	"/share " myIntendedShareList[i - 1] "\r"
	message "The negative of " a " is " -a "\r"
```

_Please note that // and /\*\*/ denote comments._

```
	set a + 1	// dont warn for this line 
	setglobal b a 	/* or this one */
```

Also, for maximum irritation, a line break may appear in "expressions" in the middle of 
the statement. From the Delta Tao Macro Reference:

```
"grfem"
{
  "Hello fair lady "
  @text
  "!\r"
}
```

If it starts with `"` you need to scan forward for the `\r`.

## 3. The macro language really is parsed over and over again as you do each new line.
I thought, surely, nobody would do that, and it would break down the code into a 
graph, which would allow the language to check the code for correctness before 
runtime while creating the graph and save a lot of time trying to find each new 
variable over and over and over as we walk the graph each game tick....  But no. 
This is why it is slow. It would also allow for a data structure which would allow 
for debugging, etc. I suppose the graph would use more memory, but these days I 
don't think we are worried much about that. How much code could there be? Well, 
what do I know? A real compiler engineer would just translate this to llvm IR and 
jit some code to run the macros. Not that the Apple App store would care for this
practice much...

