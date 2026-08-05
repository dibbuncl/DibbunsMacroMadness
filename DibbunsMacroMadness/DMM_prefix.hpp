//
//  DMM_prefix.hpp
//  DibbunsMacroMayhem
//
//  Created by Dibbun on 7/28/26.
//

#ifndef DMM_prefix_h
#   define DMM_prefix_h 1

#include <stdint.h>
#include <unistd.h>
#include <limits.h>
#include <cstdarg>
#include <cstdio>
#include <cstring>

#if defined(__APPLE__)
#   include <machine/endian.h>
#   define DTS_CLD_Darwin   1
#   if __DARWIN_BYTE_ORDER == __DARWIN_BIG_ENDIAN
#       define DTS_LITTLE_ENDIAN    0
#       define DTS_BIG_ENDIAN       1
#   elif __DARWIN_BYTE_ORDER == __DARWIN_LITTLE_ENDIAN
#       define DTS_LITTLE_ENDIAN    1
#       define DTS_BIG_ENDIAN       0
#   else
#       error Unhandled Mac endianness
#   endif
#elif defined(__linux__) || defined(__CYGWIN__)
#   include <endian.h>
#   if __BYTE_ORDER == __BIG_ENDIAN
#       define DTS_LITTLE_ENDIAN    0
#       define DTS_BIG_ENDIAN       1
#   elif __BYTE_ORDER == __LITTLE_ENDIAN
#       define DTS_LITTLE_ENDIAN    1
#       define DTS_BIG_ENDIAN       0
#   else
#       error Unhandled Linux endianness
#   endif
#elif defined(_WIN32)
#     define DTS_LITTLE_ENDIAN      1
#     define DTS_BIG_ENDIAN         0
#else
#   error Unhandled operating system
#endif

#include "DMM_Carbon.h"

#include "View_dts.h"
#include "KeyFile_dts.h"
#include "LinkedList_dts.h"
#include "File_dts.h"
#include "Utilities_dts.h"
#include "New_dts.h"
#include "TextField_dts.h"
#include <os/log.h>
#include <libgen.h>


template <typename T> inline T NativeToBigEndian( T a );
template <> inline uint8_t  NativeToBigEndian<uint8_t>( uint8_t a){ return a; }
template <> inline uint16_t NativeToBigEndian<uint16_t>( uint16_t a){ return htons(a); }
template <> inline uint32_t NativeToBigEndian<uint32_t>( uint32_t a){ return htonl(a); }
template <> inline uint64_t NativeToBigEndian<uint64_t>( uint64_t a){ return htonll(a); }
template <> inline int8_t  NativeToBigEndian<int8_t>( int8_t a){ return a; }
template <> inline int16_t NativeToBigEndian<int16_t>( int16_t a){ return htons(a); }
template <> inline int32_t NativeToBigEndian<int32_t>( int32_t a){ return htonl(a); }
template <> inline int64_t NativeToBigEndian<int64_t>( int64_t a){ return htonll(a); }
#if defined __LP64__
    template <> inline unsigned long NativeToBigEndian<unsigned long>( unsigned long a){ return NativeToBigEndian(uint32_t(a)); }
    template <> inline long NativeToBigEndian<long>( long a){ return NativeToBigEndian(int32_t(a)); }
#elif  defined __LP32__
    template <> inline unsigned long NativeToBigEndian<unsigned long>( unsigned long a){ return NativeToBigEndian(uint64_t(a)); }
    template <> inline long NativeToBigEndian<long>( long a){ return NativeToBigEndian(int64_t(a)); }
#else
#   error Unhandled ptr size
#endif

template <typename T> static inline T  BigToNativeEndian( T a){ return NativeToBigEndian(a); }


struct PictDef;
void  NativeToBigEndian( PictDef* a);
struct LightingData;
void BigToNativeEndian(LightingData*);

template <typename T> T min( T a, T b){ return a < b ? a : b;}
template <typename T> T max( T a, T b){ return a > b ? a : b;}


class Logger
{
private:
    os_log_t messages;
    os_log_t errors;
    os_log_t warnings;
    
public:
    Logger() : messages(NULL), errors(NULL), warnings(NULL)
    {
        messages = os_log_create("com.dibbuns.macromadness", "messages");
        errors = os_log_create("com.dibbuns.macromadness", "errors");
        warnings = os_log_create("com.dibbuns.macromadness", "warnings");
    }
    ~Logger();
    
    inline os_log_t GetMessages() const {return messages;}
    inline os_log_t GetErrors() const {return errors;}
    inline os_log_t GetWarnings() const {return warnings;}
};
extern Logger gLog;

// Note: format must be a string literal or this will fail to compile. os_log expects this anyway, but the error message will be misleading here.
#define LOG_ERROR( format, ...) ({ char _temp[PATH_MAX]; const char * _fileName = basename_r(__FILE__, _temp); os_log_error( gLog.GetErrors(), "*** %s:%d ERROR: " format "\n", _fileName, __LINE__ __VA_OPT__(,) __VA_ARGS__);})

// Note: format must be a string literal or this will fail to compile. os_log expects this anyway, but the error message will be misleading here.
#if DEBUG
#   define LOG_DEBUG( format, ...)  ({ char _temp[PATH_MAX]; const char * _fileName = basename_r(__FILE__, _temp); os_log( gLog.GetMessages(), "* %s:%d DEBUG: " format "\n", _fileName, __LINE__ __VA_OPT__(,) __VA_ARGS__);})
#else
#   define LOG_DEBUG(...)   ({(void)0;})
#endif

// Note: format must be a string literal or this will fail to compile. os_log expects this anyway, but the error message will be misleading here.
#define LOG_WARNING( format, ...)  ({ char _temp[PATH_MAX]; const char * _fileName = basename_r(__FILE__, _temp); os_log( gLog.GetWarnings(), "* %s:%d Warning: " format "\n", _fileName, __LINE__ __VA_OPT__(,) __VA_ARGS__);})

// Note: format must be a string literal or this will fail to compile. os_log expects this anyway, but the error message will be misleading here.
#define LOG_MESSAGE(...)  os_log( gLog.GetMessages(), __VA_ARGS__)


#define WARN_UNUSED     __attribute__((__warn_unused_result__))
#define UNUSED          __attribute__((__unused__))



#endif /* DMM_prefix_h */
