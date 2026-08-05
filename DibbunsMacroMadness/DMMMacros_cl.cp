//
//  DMMMacros_cl.cp
//  DibbunsMacroMadness
//
//  Created by Dibbun on 8/2/26.>>>>>>> 49a5730 (Initial commit)
//

// Extend Macros_cl.cp.   This is a device to break file encapsulation without changing the file
#include "Macros_cl.cp"


CFunctionMacro * DMMFindMacroByName( const char * name)
{
    return CFunctionMacro::FindMacro( name, gRootFunctionMacro );
}

CExecutingMacro * DMMStartMacroExecution( CFunctionMacro * macro )
{
    return StartMacroExecution( macro );
}

void DMMContinue1MacroExecution( CExecutingMacro *& on )
{
    Continue1MacroExecution(on);
}

unsigned long IterateMacros( unsigned long (^callback)(const CMacro * macro))
{
    unsigned long count = 0;
    for ( CMacro * macro = gRootFunctionMacro;  macro;  macro = macro->linkNext )
        count += callback(macro);

    return count;
}

CKeyMacro * GetMacroForKey( int key, uint modifiers)
{
    return CKeyMacro::FindMacro( key, modifiers & ~kKeyModRepeat, gRootKeyMacro );
}
