#ifdef _MSC_VER
#pragma once
#endif	// _MSC_VER

// stl
#include <iosfwd>
#include <cstring>
#include <cwchar>
#include <cassert>

// boost.mpl
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>

// boost.preprocessor
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/enum.hpp>
#include <boost/preprocessor/tuple/size.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/wstringize.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/control/expr_if.hpp>
#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/arithmetic/mul.hpp>
#include <boost/preprocessor/arithmetic/div.hpp>

#include "manip.h"         // for enum_mask(), bitfield_delim(), blank

namespace detail_enum
{

using std::is_enum;
using std::underlying_type;
using boost::mpl::eval_if_c;
using boost::mpl::identity;

template <typename T>
struct base_type: eval_if_c<is_enum<T>::value, typename underlying_type<T>, typename identity<T> >
{};

}   // namespace detail_enum

#ifndef PP_ENUM_DEF_BASE
#define PP_ENUM_DEF_BASE                int
#endif

#define PP_ENUM_UND_TYPE(type_)    \
    ::detail_enum::base_type<type_>::type
#define PP_ENUM_VALUE(en_)   \
    static_cast< ::detail_enum::base_type<decltype(en_)>::type>(en_)

////////////////////////////////////////////////////////////////////////////////
// Operator declarators
////////////////////////////////////////////////////////////////////////////////
#define PP_ENUM_DEFINE_BINARY_CONSTEXPR_OP(lhtype_, rhtype_, res_, op_)   \
    constexpr res_ operator op_ (lhtype_ lh, rhtype_ rh)   \
    {   \
        return static_cast<res_>(PP_ENUM_VALUE(lh) op_ PP_ENUM_VALUE(rh));   \
    }

#define PP_ENUM_DEFINE_PREFIX_UNARY_CONSTEXPR_OP(type_, res_, op_)   \
    constexpr res_ operator op_ (type_ en)    \
    {   \
        return static_cast<res_>(op_ PP_ENUM_VALUE(en)); \
    }

#define PP_ENUM_DEFINE_BINARY_MUTABLE_OP(lhtype_, rhtype_, op_)   \
    inline lhtype_ &operator op_ (lhtype_ &lh, rhtype_ rh)   \
    {   \
        auto &i = reinterpret_cast<PP_ENUM_UND_TYPE(lhtype_) &>(lh); \
        return reinterpret_cast<lhtype_ &>(i op_ PP_ENUM_VALUE(rh));   \
    }

#define PP_ENUM_DEFINE_PREFIX_UNARY_MUTABLE_OP(type_, op_)   \
    inline type_ &operator op_ (type_ &en)    \
    {   \
        auto &i = reinterpret_cast<PP_ENUM_UND_TYPE(type_) &>(en); \
        return reinterpret_cast<type_ &>(op_ i);   \
    }

#define PP_ENUM_DEFINE_POSTFIX_UNARY_MUTABLE_OP(type_, op_)   \
    inline type_ operator op_ (type_ &en, int)    \
    {   \
        auto old_en = en;   \
        auto &i = reinterpret_cast<PP_ENUM_UND_TYPE(type_) &>(en); \
        op_ i;   \
        return old_en;  \
    }

#ifdef _DEBUG
#define PP_ERROR_ENUM_RANGE_ERROR_2(name, value)    \
    assert(false, "Enum range error")

#define PP_ERROR_ENUM_RANGE_ERROR_4(name, value, min_, max_)   \
    assert("Enum range error ")
#else   
#define PP_ERROR_ENUM_RANGE_ERROR_2(name, value)  ((void)0)
#define PP_ERROR_ENUM_RANGE_ERROR_4(name, value, min_, max_) ((void)0)
#endif  // _DEBUG

#define PP_ENUM_MAKE_QELEMS(name, elements)    (name, elements)
#define PP_ENUM_GET_PREFIX(qelements)  BOOST_PP_TUPLE_ELEM(0, qelements)
#define PP_ENUM_GET_ELEMS(qelements)   BOOST_PP_TUPLE_ELEM(1, qelements)
#define PP_ENUM_GET_QUALIFIED(n, qelements)   \
    PP_ENUM_GET_PREFIX(qelements)::BOOST_PP_TUPLE_ELEM(n, PP_ENUM_GET_ELEMS(qelements))
#define PP_ENUM_GET_NAKED(n, qelements)   \
    BOOST_PP_TUPLE_ELEM(n, PP_ENUM_GET_ELEMS(qelements))

// complex enum
#define PP_COMPLEX_ENUM_MASK(n, mask_elements_pairs) \
    BOOST_PP_TUPLE_ELEM(BOOST_PP_MUL(n, 2), mask_elements_pairs)
#define PP_COMPLEX_ENUM_ELEMS(n, mask_elements_pairs) \
    BOOST_PP_TUPLE_ELEM(BOOST_PP_INC(BOOST_PP_MUL(n, 2)), mask_elements_pairs)

////////////////////////////////////////////////////////////////////////////////
// ENUM
////////////////////////////////////////////////////////////////////////////////
#define PP_ENUM_STREAM_BUFFER_SIZE  0x100

// range independent arithmetic
#define PP_ENUM_DEFINE_ARITHMETIC_CONSTEXPR(name)    \
    PP_ENUM_DEFINE_BINARY_CONSTEXPR_OP(name, name, name, +)  \
    PP_ENUM_DEFINE_BINARY_CONSTEXPR_OP(name, PP_ENUM_UND_TYPE(name), name, +)  \
    PP_ENUM_DEFINE_BINARY_CONSTEXPR_OP(PP_ENUM_UND_TYPE(name), name, name, +)  \
    PP_ENUM_DEFINE_BINARY_CONSTEXPR_OP(name, name, name, -)  \
    PP_ENUM_DEFINE_BINARY_CONSTEXPR_OP(name, PP_ENUM_UND_TYPE(name), name, -)  \
    PP_ENUM_DEFINE_BINARY_CONSTEXPR_OP(PP_ENUM_UND_TYPE(name), name, name, -)

// check free arithmetic
#define PP_ENUM_ADAPT_ARITHMETIC_FREE(name)   \
    PP_ENUM_DEFINE_PREFIX_UNARY_MUTABLE_OP(name, ++)   \
    PP_ENUM_DEFINE_POSTFIX_UNARY_MUTABLE_OP(name, ++)   \
    PP_ENUM_DEFINE_PREFIX_UNARY_MUTABLE_OP(name, --)   \
    PP_ENUM_DEFINE_POSTFIX_UNARY_MUTABLE_OP(name, --)   \
    PP_ENUM_DEFINE_BINARY_MUTABLE_OP(name, PP_ENUM_UND_TYPE(name), +=)   \
    PP_ENUM_DEFINE_BINARY_MUTABLE_OP(name, PP_ENUM_UND_TYPE(name), -=)

// check free logic
#define PP_BITFIELD_ADAPT_LOGIC(name)    \
    PP_ENUM_DEFINE_BINARY_CONSTEXPR_OP(name, name, name, |)    \
    PP_ENUM_DEFINE_BINARY_CONSTEXPR_OP(name, PP_ENUM_UND_TYPE(name), name, |)    \
    PP_ENUM_DEFINE_BINARY_CONSTEXPR_OP(PP_ENUM_UND_TYPE(name), name, name, |)    \
    PP_ENUM_DEFINE_BINARY_CONSTEXPR_OP(name, name, name, &)    \
    PP_ENUM_DEFINE_BINARY_CONSTEXPR_OP(name, PP_ENUM_UND_TYPE(name), name, &)    \
    PP_ENUM_DEFINE_BINARY_CONSTEXPR_OP(PP_ENUM_UND_TYPE(name), name, name, &)    \
    PP_ENUM_DEFINE_PREFIX_UNARY_CONSTEXPR_OP(name, name, ~)   \
    PP_ENUM_DEFINE_BINARY_MUTABLE_OP(name, name, |=)   \
    PP_ENUM_DEFINE_BINARY_MUTABLE_OP(name, PP_ENUM_UND_TYPE(name), |=)   \
    PP_ENUM_DEFINE_BINARY_MUTABLE_OP(name, name, &=)   \
    PP_ENUM_DEFINE_BINARY_MUTABLE_OP(name, PP_ENUM_UND_TYPE(name), &=)

#define PP_ENUM_ADAPT_ARITHMETIC(name)   \
    PP_ENUM_ADAPT_ARITHMETIC_FREE(name)   \
    PP_ENUM_DEFINE_ARITHMETIC_CONSTEXPR(name)

////////////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* PP_DEFINE_MACRO_CONST                                           */
/************************************************************************/
#define PP_DEFINE_MACRO_CONST(val_)    \
    const PP_ENUM_DEF_BASE val_##_ = static_cast<PP_ENUM_DEF_BASE>(val_)

/************************************************************************/
/* PP_DEFINE_ENUM_CONST                                            */
/************************************************************************/
#define PP_DEFINE_ENUM_CONST(val_) \
    val_##_ = static_cast<PP_ENUM_DEF_BASE>(val_)
