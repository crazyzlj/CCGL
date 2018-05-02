/*!
 * \brief Basic definitions.
 *        Part of the Common Cross-platform Geographic Library (CCGL)
 * \author Liangjun Zhu (crazyzlj)
 * \version 1.0
 * \changelog  2018-05-01 - lj - Initially implementation.\n
 */

#ifndef CCGL_BASIC_H
#define CCGL_BASIC_H

#ifndef NDEBUG
#ifndef _DEBUG
#define _DEBUG
#endif /* _DEBUG */
#endif /* NDEBUG */

/// OpenMP support
#ifdef SUPPORT_OMP
#include <omp.h>
#endif /* SUPPORT_OMP */
/// math and STL headers
#include <cmath>
#include <cfloat>
#include <array>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <cstdint>
#include <memory>
/// time
#include <ctime>
/// string
#include <cstring>
/// IO stream
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <iostream>
#include <fstream>
/// platform
#ifdef windows
#include <io.h>
//#define _WINSOCKAPI_    // stops windows.h including winsock.h // _WINSOCKAPI_ is defined by <winsock2.h>
#include <winsock2.h>
#include <windows.h>
#include <direct.h>
#include <sys/types.h>
#include <sys/stat.h>
#else
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#endif /* windows */
#if (defined macos) || (defined macosold)
#include <libproc.h>
#endif /* macos */
/// assert
#include <cassert>
/// variable arguments
#include <cstdarg>

using namespace std;

/// Architecture
#if defined _WIN64 || __x86_64 || __LP64__
#define CPP_64
#endif

#if defined _MSC_VER
#define CPP_MSVC
#else
#define CPP_GCC
#if defined(__APPLE__)
#define CPP_APPLE
#endif
#endif

// define some macro for string related built-in functions
#ifdef MSVC
#define stringcat strcat_s
#define stringcpy strcpy_s
#define strprintf sprintf_s
#define strtok strtok_s
#define stringscanf sscanf_s
#else
#define stringcat strcat
#define stringcpy strcpy
#define strprintf snprintf
#define strtok strtok_r
#define stringscanf sscanf
#endif /* MSVC */

#if defined(__MINGW32_MAJOR_VERSION) || defined(__MINGW64_VERSION_MAJOR) || defined(_MSC_VER)
#define strcasecmp _stricmp
#endif /* MINGW or MSVC */

/*
* Avoid the compile error on MSVC like this:
*   warning C4251: 'CLASS_TEST::m_structs':
*           class 'std::vector<_Ty>' needs to have dll-interface to be used by clients of class
* refers to http://www.cnblogs.com/duboway/p/3332057.html
*/
#ifdef MSVC
#define DLL_STL_LIST(STL_API, STL_TYPE) \
    template class STL_API std::allocator< STL_TYPE >; \
    template class STL_API std::vector<STL_TYPE, std::allocator< STL_TYPE > >;
#endif /* MSVC */

/*!
 * \namespace ccgl
 * \brief Common Cross-platform Geographic Library
 */
namespace ccgl {
#if defined CPP_MSVC
/// <summary>1-byte signed integer.</summary>
typedef signed __int8			vint8_t;
/// <summary>1-byte unsigned integer.</summary>
typedef unsigned __int8			vuint8_t;
/// <summary>2-bytes signed integer.</summary>
typedef signed __int16			vint16_t;
/// <summary>2-bytes unsigned integer.</summary>
typedef unsigned __int16		vuint16_t;
/// <summary>4-bytes signed integer.</summary>
typedef signed __int32			vint32_t;
/// <summary>4-bytes unsigned integer.</summary>
typedef unsigned __int32		vuint32_t;
/// <summary>8-bytes signed integer.</summary>
typedef signed __int64			vint64_t;
/// <summary>8-bytes unsigned integer.</summary>
typedef unsigned __int64		vuint64_t;
#elif defined CPP_GCC
typedef int8_t					vint8_t;
typedef uint8_t					vuint8_t;
typedef int16_t					vint16_t;
typedef uint16_t				vuint16_t;
typedef int32_t					vint32_t;
typedef uint32_t				vuint32_t;
typedef int64_t					vint64_t;
typedef uint64_t				vuint64_t;
#endif

#ifdef CPP_64
/// <summary>Signed interface whose size is equal to sizeof(void*).</summary>
typedef vint64_t				vint;
/// <summary>Signed interface whose size is equal to sizeof(void*).</summary>
typedef vint64_t				vsint;
/// <summary>Unsigned interface whose size is equal to sizeof(void*).</summary>
typedef vuint64_t				vuint;

#else
/// <summary>Signed interface whose size is equal to sizeof(void*).</summary>
typedef vint32_t				vint;
/// <summary>Signed interface whose size is equal to sizeof(void*).</summary>
typedef vint32_t				vsint;
/// <summary>Unsigned interface whose size is equal to sizeof(void*).</summary>
typedef vuint32_t				vuint;
#endif
/// <summary>Signed integer representing position.</summary>
typedef vint64_t				pos_t;

/*!
 * Use static_cast<T>(a) instead (T)a or T(a) to convert datetypes
 */
#define CVT_INT(param) static_cast<int>((param))
#define CVT_SIZET(param) static_cast<size_t>((param))
#define CVT_FLT(param) static_cast<float>((param))
#define CVT_DBL(param) static_cast<double>((param))

#define CVT_VINT(param) static_cast<vint>((param))
#define CVT_VSINT(param) static_cast<vsint>((param))
#define CVT_VUINT(param) static_cast<vuint>((param))


/*!
 * Global utility definitions
 */
/**
 * \def NODATA_VALUE
 * \brief NODATA value
 */
#ifndef NODATA_VALUE
#define NODATA_VALUE    (-9999.0f)
#endif /* NODATA_VALUE */
const float MISSINGFLOAT = -1 * FLT_MAX;
const float MAXIMUMFLOAT = FLT_MAX;
#ifndef PATH_MAX
#define PATH_MAX 1024
#endif /* PATH_MAX */
/**
 * \def ZERO
 * \brief zero value used in numeric calculation
 */
#ifndef UTIL_ZERO
#define UTIL_ZERO        1.0e-6f
#endif /* UTIL_ZERO */
/**
 * \def PI
 * \brief PI value used in numeric calculation
 */
#ifndef PI
#define PI                3.14159265358979323846f
#endif /* PI */
/**
 * \def MINI_SLOPE
 * \brief Minimum slope gradient
 */
#ifndef MINI_SLOPE
#define MINI_SLOPE        0.0001f
#endif /* MINI_SLOPE */

#ifdef windows
#define SEP "\\"
#ifndef MSVC
#define Tag_So "lib"
#endif
#define Tag_DyLib ".dll"
#else
#define SEP "/"
#define Tag_So "lib"
#endif /* windows */
#ifdef linux
#define Tag_DyLib ".so"
#elif (defined macos) || (defined macosold)
#define Tag_DyLib ".dylib"
#endif /* linux */

// Days number of each month
static int MonthDays[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/*
 * Constant value type pointer
 */
typedef const int *CINTPTR;
typedef const float *CFLOATPTR;
typedef const double *CDOUBLEPTR;

/*!
 * \class NotCopyable
 * \brief Base class for classes that cannot be copied. By inheriting this
 *        class you can disable copying of your classes.
 *        e.g., class myClass: private NotCopyable {}
 */
class NotCopyable {
private:
    NotCopyable(const NotCopyable &) {};

    NotCopyable &operator=(const NotCopyable &) { return *this; }
public:
    NotCopyable() {};
};

/*!
 * \class DefaultConstructor
 * \brief All class should inherit from this default object.
 */
class DefaultConstructor {
public:
#if defined(_MSC_VER) && (_MSC_VER <= 1600)
    DefaultConstructor() {};
    virtual ~DefaultConstructor() {};
#else
    DefaultConstructor() = default;
    virtual ~DefaultConstructor() = default;
#endif /* less than VS2010 */
};
} /* namespace ccgl */

#endif /* CCGL_BASIC_H */
