//
//  DMMMacros_cl.hpp
//  DibbunsMacroMadness
//
//  Created by Dibbun on 8/2/26.
//

#ifndef DMMMacros_cl_hpp
#define DMMMacros_cl_hpp 1

#include "Macros_cl.h"

CFunctionMacro * DMMFindMacroByName( const char * name);
CExecutingMacro * DMMStartMacroExecution( CFunctionMacro * macro );
void DMMContinue1MacroExecution( CExecutingMacro *& on );

/*! @abstract Iterate the root list of macros.
 *  @param  callback        For each macro, call callback. Add the return value of callback to Count
 *  @return Return count */
unsigned long IterateMacros( unsigned long (^callback)(const CMacro * macro));

CKeyMacro * GetMacroForKey( int key, uint modifiers);


#endif // !DMMMacros_cl_hpp
