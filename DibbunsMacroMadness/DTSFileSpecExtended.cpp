//
//  DTSFileSpecExtended.cpp
//  DibbunsMacroMadness
//
//  Created by Dibbun on 8/2/26.
//

#include "DTSFileSpecExtended.hpp"
#include "File_DMM.hpp"

DTSError DTSFileSpecExtended::GetPath( char path[PATH_MAX])
{
    this->SetDir();
    int err = fcntl(this->priv.p->GetDirectory(), F_GETPATH, path);
    if(err)
        return GetFileErrorCode(errno);
    return noErr;
}


