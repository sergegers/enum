#ifdef _MSC_VER
#pragma once
#endif	// _MSC_VER

#include <CodeAnalysis/Warnings.h>

/******************************************************************************/
/* code analysis & w4 warnings                                                */
/******************************************************************************/

// compiler warnings
#ifdef _MSC_VER

// These w4 warnings are disabled for third party code
#define W4_WARNINGS 4002 4005 4091 4100 4121 4127 4180 4201 4244 4267 4302 4310 4389 4456    \
    4458 4459 4510 4512 4624 4702 4838 4913

// The w4 warnings below are disabled for all code
#pragma warning (disable: 4714 4996)

// emitted by enum macro generators
#pragma warning (disable: 4002) // variadic macro tricks
#pragma warning (disable: 4003) // variadic macro tricks
//#pragma warning (disable: 4268) // variadic macro tricks VS 2017 with 2015 toolset

#pragma warning (disable: 4100) // VS 2017 U2
#pragma warning (disable: 4709) // VS 2017 U2
//#pragma warning (disable: 4645) // VS 2015 U3

#pragma warning (disable: 6011) // VS 2017 with 2015 toolset

#ifndef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
#endif

// using checked MS shit is seriously compromise portability
#ifndef _SCL_SECURE_NO_WARNINGS
    #define _SCL_SECURE_NO_WARNINGS
#endif

// compiler version specific warnings
#if (_MSC_FULL_VER == 180021114)
    // VC++ Nov 2013 CTP
    // disable alpha version false warning
    #pragma warning (disable: 4592)
#elif (_MSC_FULL_VER == 190021901)
    // VC++ 14 CTP2
    // hides global declaration warning
    #pragma warning (disable: 4459)
    // ignoring typedef
    #pragma warning (disable: 4091)
#endif  // (_MSC_FULL_VER)

// globally suppressed code analysis warnings
#pragma warning(disable: 28251) // IDL header files generator

/******************************************************************************/
/* Place SDK and third party library headers between CHT_PUSH_WARNINGS()      */
/* CHT_POP_WARNINGS() guards.                                                 */
/******************************************************************************/

#define CHT_PUSH_WARNINGS()    \
    __pragma( warning (push)) \
    __pragma( warning (disable: W4_WARNINGS ALL_CODE_ANALYSIS_WARNINGS))

#define CHT_POP_WARNINGS() \
    __pragma( warning (pop))

#else

#define CHT_PUSH_WARNINGS()
#define CHT_POP_WARNINGS()

#endif	// _MSC_VER

