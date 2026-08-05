//
//  DTSFileSpecExtended.hpp
//  DibbunsMacroMadness
//
//  Created by Dibbun on 8/2/26.
//

#ifndef DTSFileSpecExtended_hpp
#define DTSFileSpecExtended_hpp 1

#include "File_DMM.hpp"
#include "File_dts.h"

class DTSFileSpecExtended : public DTSFileSpec
{
    
public:
    DTSError GetPath( char path[PATH_MAX]);
};



#endif // DTSFileSpecExtended_hpp
