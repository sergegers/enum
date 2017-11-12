#pragma once

// The following macros define the minimum required platform.  The minimum required platform
// is the earliest version of Windows, Internet Explorer etc. that has the necessary features to run 
// your application.  The macros work by enabling all features available on platform versions up to and 
// including the version specified.

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.

// Change this to the appropriate value to target other versions of Windows.
/*#ifndef _WIN32_WINNT
#define _WIN32_WINNT	0x0501
#endif  // _WIN32_WINNT
*/

// code warnings
#include "warn.h"

CHT_PUSH_WARNINGS()

#ifndef NTDDI_VERSION
#define NTDDI_VERSION   NTDDI_WIN7
#endif  // NTDDI_VERSION

#ifndef STRICT
#define STRICT
#endif

// Unicode compilation only
#ifndef _UNICODE
#define _UNICODE
#endif

#ifdef _DDK_   
#define DECLSPEC_DEPRECATED_DDK
#endif  // _DDK_ 

// Including SDKDDKVer.h defines the highest available Windows platform.

// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.
#include <sdkddkver.h>

#ifndef _DDK_   // PSDK defines & headers

/************************************************************************/
/* Windows SDK definitions                                              */
/************************************************************************/
// for old Borland headers
#ifndef NTDDI_WIN7
#define NTDDI_WIN7		0x06010000
#endif	// NTDDI_WIN7

// PSDK defines
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
#endif // !WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
#define NOMINMAX				// remove min, max macros
#endif // !NOMINMAX

#ifndef _RICHEDIT_VER
#define _RICHEDIT_VER	0x0200
#endif	// _RICHEDIT_VER

// http://blogs.msdn.com/b/oldnewthing/archive/2013/01/24/10387757.aspx
#define STRICT_TYPED_ITEMIDS

// NDK user mode
#define NTOS_MODE_USER

#ifdef _MSC_VER

/************************************************************************/
/* ATL definitions                                                      */
/************************************************************************/
#define _WIN32_DCOM 
#define _ATL_FREE_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW
#undef _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

/************************************************************************/
/* WTL definitions                                                      */
/************************************************************************/
#define _WTL_NO_CSTRING
#define _WTL_NO_WTYPES
#define _WTL_NO_AUTOMATIC_NAMESPACE 
#define _WTL_NEW_PAGE_NOTIFY_HANDLERS

// for WTL third party controls
#define END_MSG_MAP_EX	END_MSG_MAP

#elif __BORLANDC__
/************************************************************************/
/* VCL definitions                                                      */
/************************************************************************/
#define VCL_IOSTREAM

#endif	// _MSC_VER || __BORLANDC__

/************************************************************************/
/* BOOST configuration                                                  */
/************************************************************************/ 

#include <boost/config.hpp>
#include <boost/version.hpp>
#include <boost/preprocessor/stringize.hpp>

// compiler
#pragma message("")

#if defined(_MSC_VER) && !defined(__clang__)
        #pragma message ("compiler is VC++")
        #pragma message("_MSC_VER=" BOOST_PP_STRINGIZE(_MSC_VER))
        #pragma message("BOOST_MSVC_FULL_VER=" BOOST_PP_STRINGIZE(BOOST_MSVC_FULL_VER))
#elif defined(__BORLANDC__)
        #pragma message ("compiler is Borland C++")
#elif defined(_MSC_VER) && defined(__clang__)
        #pragma message ("compiler is Clang")
#else
    #error Compiler is not supported!
#endif

#pragma message("BOOST_VERSION=" BOOST_PP_STRINGIZE(BOOST_VERSION))
#pragma message("")

#ifdef CHT_VERBOSE_COMPILATION
    #define BOOST_LIB_DIAGNOSTIC
#endif  // CHT_VERBOSE_COMPILATION

// BOOST mpl configuration
#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_VECTOR_SIZE			            50
#define BOOST_MPL_LIMIT_SET_SIZE                        40
#define BOOST_MPL_LIMIT_MAP_SIZE                        40
#define BOOST_MPL_LIMIT_STRING_SIZE					    128
#define BOOST_MPL_LIMIT_METAFUNCTION_ARITY			    10

// BOOST fusion configuration
#define FUSION_MAX_VECTOR_SIZE                          50
#define BOOST_FUSION_INVOKE_MAX_ARITY                   10
#define BOOST_FUSION_INVOKE_PROCEDURE_MAX_ARITY         10
#define BOOST_FUSION_INVOKE_FUNCTION_OBJECT_MAX_ARITY   10

//BOOST spirit debug configuration
#ifdef _DEBUG    
    #define BOOST_SPIRIT_QI_DEBUG
    #define BOOST_SPIRIT_KARMA_DEBUG
    #define BOOST_SPIRIT_DEBUG_PRINT_SOME               50
#endif // _DEBUG

// BOOST parameter configuration
#define BOOST_PARAMETER_MAX_ARITY                       16

// BOOST test
// conflicts with per_element() manipulator
//#define BOOST_TEST_TOOLS_DEBUGGABLE

#if defined(_MSC_VER) && !defined(__clang__)
// support stdcall binding
// WARNING! non portable!
#define BOOST_BIND_ENABLE_STDCALL
#define BOOST_MEM_FN_ENABLE_STDCALL
#if (_MSC_VER >= 1600)  // VS 2010
//  for boost::result_of
#define BOOST_RESULT_OF_USE_DECLTYPE
#endif  // (_MSC_VER >= 1600)
#elif defined(__BORLANDC__)
// fix boost threads
#define BOOST_THREAD_USE_LIB
#elif defined(_MSC_VER) && defined(__clang__)
#define BOOST_USE_WINDOWS_H
#else
#error Compiler is not supported!
#endif	// BOOST compiler specific

#endif   // !_DDK_

/************************************************************************/
/*  IDA SDK definitions                                                 */
/************************************************************************/ 
// ability to use low level flags directly
#define BYTES_SOURCE

CHT_POP_WARNINGS()
// ! keep a blank line below for the MS resource compiler !

