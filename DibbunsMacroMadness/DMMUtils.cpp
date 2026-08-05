//
//  DMMUtils.cpp
//  DibbunsMacroMadness
//
//  Created by Dibbun on 7/28/26.
//

#include "Utilities_dts.h"
#include <dispatch/dispatch.h>
#include <libgen.h>

#if DEBUG
#else
#   define NDEBUG    1
#endif
#include <assert.h>
#include <os/log.h>

#include "Public_cl.h"

#define BYTE_SWAP(_a) _a = NativeToBigEndian(_a)

void  NativeToBigEndian( PictDef* a)
{
    BYTE_SWAP( a->pdFlags );
    BYTE_SWAP( a->pdVersion);         // see kPictDefVersion
    BYTE_SWAP( a->pdBitsID);        // ID of associated kTypePictureBits record
    BYTE_SWAP( a->pdColorsID);        // ID of associated kTypePictureColors record
    BYTE_SWAP( a->pdChecksum);        // cksum of all image data
    BYTE_SWAP( a->pdFlags);        // enumerated above
    BYTE_SWAP( a->pdUnusedFlags);  // future expansion
    BYTE_SWAP( a->pdUnusedFlags2);    // future expansion
    BYTE_SWAP( a->pdLightingID);    // ID of associated kTypeLightingData record (0 if none)
    BYTE_SWAP( a->pdPlane);        // -15 .. +15, from deepest to shallowest
    BYTE_SWAP( a->pdNumFrames);    // number of distinct animation "cells"
    BYTE_SWAP( a->pdNumAnims);        // number of used entries in frame-table
    
    for( unsigned long i = 0; i < kPictDefAnimTableSize; i++)
        BYTE_SWAP( a->pdAnimFrameTable[i]);
}

void BigToNativeEndian(LightingData*ld)
{
    // skip ilCValue
    BigToNativeEndian( ld->ilPlane );
    BigToNativeEndian( ld->ilRadius );
}

Logger gLog;

Logger::~Logger()
{
    os_release(messages); messages = NULL;
    os_release(errors); errors = NULL;
    os_release(warnings); warnings = NULL;
}
