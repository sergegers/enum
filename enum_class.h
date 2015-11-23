#ifdef _MSC_VER
#pragma once
#endif	// _MSC_VER

#include "enum_common.h"

// range dependent arithmetic
#define PP_ENUM_CLASS_SIZE(name)  PP_ENUM_VALUE(BOOST_PP_CAT(name::, cel))

#define PP_ENUM_CLASS_DEFINE_PREFIX_UNARY_MUTABLE_RANGE_OP(type_, op_)   \
    inline type_ &operator op_ (type_ &en)    \
    {   \
        auto &i = reinterpret_cast<PP_ENUM_UND_TYPE(type_) &>(en);   \
        op_ i;  \
        if (i < 0 || i > PP_ENUM_VALUE(PP_ENUM_CLASS_SIZE(type_)))   \
            PP_ERROR_ENUM_RANGE_ERROR_4(type_, i, 0, PP_ENUM_VALUE(PP_ENUM_CLASS_SIZE(type_)));    \
        return reinterpret_cast<type_ &>(i); \
    }

#define PP_ENUM_CLASS_DEFINE_POSTFIX_UNARY_MUTABLE_RANGE_OP(type_, op_)   \
    inline type_ operator op_ (type_ &en, int)    \
    {   \
        auto old_en = en;   \
        auto &i = reinterpret_cast<PP_ENUM_UND_TYPE(type_) &>(en);   \
        op_ i;  \
        if (i < 0 || i > PP_ENUM_VALUE(PP_ENUM_CLASS_SIZE(type_)))   \
            PP_ERROR_ENUM_RANGE_ERROR_4(type_, i, 0, PP_ENUM_VALUE(PP_ENUM_CLASS_SIZE(type_)));    \
        return old_en; \
    }

#define PP_ENUM_CLASS_DEFINE_BINARY_MUTABLE_RANGE_OP(lh_type_, rh_type_, op_)   \
    inline lh_type_ &operator op_ (lh_type_ &lh, rh_type_ rh)    \
    {   \
        auto &i = reinterpret_cast<PP_ENUM_UND_TYPE(lh_type_) &>(lh);   \
        i op_ PP_ENUM_VALUE(rh);   \
        if (i < 0 || i > PP_ENUM_VALUE(PP_ENUM_CLASS_SIZE(lh_type_)))   \
            PP_ERROR_ENUM_RANGE_ERROR_4(lh_type_, i, 0, PP_ENUM_VALUE(PP_ENUM_CLASS_SIZE(lh_type_)));    \
        return reinterpret_cast<lh_type_ &>(i); \
    }

#define PP_ENUM_CLASS_DEFINE_ARITHMETIC_RANGE(name)    \
    PP_ENUM_CLASS_DEFINE_PREFIX_UNARY_MUTABLE_RANGE_OP(name, ++)   \
    PP_ENUM_CLASS_DEFINE_POSTFIX_UNARY_MUTABLE_RANGE_OP(name, ++)  \
    PP_ENUM_CLASS_DEFINE_PREFIX_UNARY_MUTABLE_RANGE_OP(name, --)   \
    PP_ENUM_CLASS_DEFINE_POSTFIX_UNARY_MUTABLE_RANGE_OP(name, --)   \
    PP_ENUM_CLASS_DEFINE_BINARY_MUTABLE_RANGE_OP(name, PP_ENUM_UND_TYPE(name), +=)   \
    PP_ENUM_CLASS_DEFINE_BINARY_MUTABLE_RANGE_OP(name, PP_ENUM_UND_TYPE(name), -=)

////////////////////////////////////////////////////////////////////////////////
#define PP_ENUM_CLASS_DEFINE_COMPARISON(name)  \
    PP_ENUM_DEFINE_BINARY_CONSTEXPR_OP(name, PP_ENUM_UND_TYPE(name), bool, >)  \
    PP_ENUM_DEFINE_BINARY_CONSTEXPR_OP(PP_ENUM_UND_TYPE(name), name, bool, >)  \
    PP_ENUM_DEFINE_BINARY_CONSTEXPR_OP(name, PP_ENUM_UND_TYPE(name), bool, >=)  \
    PP_ENUM_DEFINE_BINARY_CONSTEXPR_OP(PP_ENUM_UND_TYPE(name), name, bool, >=)  \
    PP_ENUM_DEFINE_BINARY_CONSTEXPR_OP(name, PP_ENUM_UND_TYPE(name), bool, <)  \
    PP_ENUM_DEFINE_BINARY_CONSTEXPR_OP(PP_ENUM_UND_TYPE(name), name, bool, <)  \
    PP_ENUM_DEFINE_BINARY_CONSTEXPR_OP(name, PP_ENUM_UND_TYPE(name), bool, <=)  

#define PP_ENUM_CLASS_DEFINE_ARITHMETIC(name)    \
    PP_ENUM_CLASS_DEFINE_ARITHMETIC_RANGE(name)  \
    PP_ENUM_DEFINE_ARITHMETIC_CONSTEXPR(name)  \
    PP_ENUM_CLASS_DEFINE_COMPARISON(name)

////////////////////////////////////////////////////////////////////////////////
// ostream cases
#define PP_ENUM_CLASS_ADD_OSTREAM_CASE(z, n, qelements)    \
    case PP_ENUM_GET_QUALIFIED(n, qelements):    \
    ostr << BOOST_PP_STRINGIZE(PP_ENUM_GET_NAKED(n, qelements));  \
    break;

#define PP_ENUM_CLASS_ADD_WOSTREAM_CASE(z, n, qelements)    \
    case PP_ENUM_GET_QUALIFIED(n, qelements):    \
    ostr << BOOST_PP_WSTRINGIZE(PP_ENUM_GET_NAKED(n, qelements));  \
    break;

#define PP_ENUM_CLASS_DEFINE_OSTREAMS_T(name, elements, char_, add_ostream) \
    template <typename TCharTraits> \
    ::std::basic_ostream<char_, TCharTraits> &operator << ( ::std::basic_ostream<char_, TCharTraits> &ostr,     \
        name const en)  \
    {   \
        switch (en)    \
        {   \
            BOOST_PP_REPEAT(BOOST_PP_TUPLE_SIZE(elements), add_ostream, \
                            PP_ENUM_MAKE_QELEMS(name, elements))    \
        default:    \
            auto const fl = ostr.flags();  \
            ostr << ::std::hex << ::std::showbase << ::std::uppercase <<    \
                PP_ENUM_VALUE(en) << ::blank{}; \
            ostr.flags(fl); \
        }   \
        return ostr;    \
    }


#define PP_ENUM_CLASS_DEFINE_STREAMS(name, elements)   \
    PP_ENUM_CLASS_DEFINE_OSTREAMS_T(name, elements, char, PP_ENUM_CLASS_ADD_OSTREAM_CASE)   \
    PP_ENUM_CLASS_DEFINE_OSTREAMS_T(name, elements, wchar_t, PP_ENUM_CLASS_ADD_WOSTREAM_CASE)   \

////////////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* PP_DEFINE_ENUM_CLASS                                            */
/* Usage:                                                               */
/* PP_DEFINE_ENUM_CLASS(enum_name, (elem1, elem2, ...));           */
/************************************************************************/
#define PP_DEFINE_ENUM_CLASS(name, elements) \
    __pragma(warning(push)) \
    __pragma(warning(disable: 4390))    \
    enum class name: PP_ENUM_DEF_BASE   \
    { \
        BOOST_PP_TUPLE_ENUM(elements), \
        cel \
    };  \
    \
    PP_ENUM_CLASS_DEFINE_STREAMS(name, elements)   \
    PP_ENUM_CLASS_DEFINE_ARITHMETIC(name)    \
    __pragma(warning(pop))

/************************************************************************/
/* PP_ADAPT_ENUM_CLASS                                               */
/************************************************************************/

#define PP_ENUM_CLASS_ADAPT_ARITHMETIC(name)   \
    PP_ENUM_ADAPT_ARITHMETIC_FREE(name)   \
    PP_ENUM_DEFINE_ARITHMETIC_CONSTEXPR(name)  \
    PP_ENUM_CLASS_DEFINE_COMPARISON(name)

////////////////////////////////////////////////////////////////////////////////
#define PP_ADAPT_ENUM_CLASS(name, elements)    \
    PP_ENUM_CLASS_ADAPT_ARITHMETIC(name) \
    PP_ENUM_CLASS_DEFINE_STREAMS(name, elements)

/************************************************************************/
/* PP_ADAPT_BITFIELD_CLASS                                           */
/************************************************************************/
#define PP_BITFILED_CLASS_ADD_MEMBER_IF(z, n, qelements)    \
    if (0 != PP_ENUM_VALUE(en & PP_ENUM_GET_QUALIFIED(n, qelements)))    \
    {   \
        ostr << BOOST_PP_STRINGIZE(PP_ENUM_GET_NAKED(n, qelements)) <<  \
            ::basic_bitfield_delim_t<char, TCharTraits>::get_delim(ostr);  \
        en &= ~PP_ENUM_GET_QUALIFIED(n, qelements);    \
    }

#define PP_BITFILED_CLASS_ADD_WMEMBER_IF(z, n, qelements)    \
    if (0 != PP_ENUM_VALUE(en & PP_ENUM_GET_QUALIFIED(n, qelements)))    \
    {   \
        ostr << BOOST_PP_WSTRINGIZE(PP_ENUM_GET_NAKED(n, qelements)) << \
            ::basic_bitfield_delim_t<wchar_t, TCharTraits>::get_delim(ostr);  \
        en &= ~PP_ENUM_GET_QUALIFIED(n, qelements);    \
    }

#define PP_BITFIELD_CLASS_DEFINE_OSTREAMS_T(name, elements, char_, add_member) \
    template <typename TCharTraits> \
    ::std::basic_ostream<char_, TCharTraits> &operator << ( ::std::basic_ostream<char_, TCharTraits> &ostr,   \
            name en)  \
    {   \
        BOOST_PP_REPEAT(BOOST_PP_TUPLE_SIZE(elements), add_member, \
                        PP_ENUM_MAKE_QELEMS(name, elements))    \
        if (0 != PP_ENUM_VALUE(en))  \
        {   \
            auto const fl = ostr.flags();  \
            ostr << ::std::hex << ::std::showbase << ::std::uppercase <<    \
                PP_ENUM_VALUE(en) << ::blank{}; \
            ostr.flags(fl); \
        }   \
        return ostr;    \
    }

////////////////////////////////////////////////////////////////////////////////
#define PP_ADAPT_BITFIELD_CLASS(name, elements)    \
    PP_ENUM_ADAPT_ARITHMETIC(name)   \
    PP_BITFIELD_ADAPT_LOGIC(name)    \
    PP_BITFIELD_CLASS_DEFINE_OSTREAMS_T(name, elements, char, PP_BITFILED_CLASS_ADD_MEMBER_IF) \
    PP_BITFIELD_CLASS_DEFINE_OSTREAMS_T(name, elements, wchar_t, PP_BITFILED_CLASS_ADD_WMEMBER_IF)

///************************************************************************/
///* PP_ADAPT_COMPLEX_ENUM_CLASS                                       */
///************************************************************************/
#define PP_COMPLEX_ENUM_CLASS_NAME(name_elements) BOOST_PP_TUPLE_ELEM(0, name_elements)
#define PP_COMPLEX_ENUM_CLASS_ELEM(n, name_elements) \
    BOOST_PP_TUPLE_ELEM(n, BOOST_PP_TUPLE_ELEM(1, name_elements))
#define PP_COMPLEX_ENUM_CLASS_QELEM(n, name_elements) \
    PP_COMPLEX_ENUM_CLASS_NAME(name_elements)::PP_COMPLEX_ENUM_CLASS_ELEM(n, name_elements)

#define PP_COMPLEX_ENUM_CLASS_ADD_MEMBER_CASE(z, n, name_elements)    \
    case PP_COMPLEX_ENUM_CLASS_QELEM(n, name_elements):    \
        ostr << BOOST_PP_STRINGIZE(PP_COMPLEX_ENUM_CLASS_ELEM(n, name_elements)) << \
            ::basic_bitfield_delim_t<char, TCharTraits>::get_delim(ostr);  \
    break;

#define PP_COMPLEX_ENUM_CLASS_ADD_WMEMBER_CASE(z, n, name_elements)    \
    case PP_COMPLEX_ENUM_CLASS_QELEM(n, name_elements):    \
        ostr << BOOST_PP_WSTRINGIZE(PP_COMPLEX_ENUM_CLASS_ELEM(n, name_elements)) <<    \
            ::basic_bitfield_delim_t<wchar_t, TCharTraits>::get_delim(ostr);  \
    break;

////////////////////////////////////////////////////////////////////////////////
#define PP_COMPLEX_ENUM_CLASS_NAME(args)  BOOST_PP_TUPLE_ELEM(0, args)
#define PP_COMPLEX_ENUM_CLASS_MASK_ELEMS(args)  BOOST_PP_TUPLE_ELEM(1, args)
#define PP_COMPLEX_ENUM_CLASS_QMASK(n, args)  \
    PP_COMPLEX_ENUM_CLASS_NAME(args)::PP_COMPLEX_ENUM_MASK(n, PP_COMPLEX_ENUM_CLASS_MASK_ELEMS(args))
#define PP_COMPLEX_ENUM_CLASS_ELEMS(n, args)  \
    PP_COMPLEX_ENUM_ELEMS(n, PP_COMPLEX_ENUM_CLASS_MASK_ELEMS(args))
#define PP_COMPLEX_ENUM_CLASS_FORWARD_NAME_ELEMS(n, args)   \
    (PP_COMPLEX_ENUM_CLASS_NAME(args), PP_COMPLEX_ENUM_CLASS_ELEMS(n, args))

#define PP_COMPLEX_ENUM_CLASS_MAKE_SWITCH_T(z, n, args, case_macro)    \
    if (0 != PP_ENUM_VALUE((mask & PP_COMPLEX_ENUM_CLASS_QMASK(n, args)))) \
    {   \
        switch (en & PP_COMPLEX_ENUM_CLASS_QMASK(n, args))    \
        {   \
            BOOST_PP_REPEAT(    \
                                BOOST_PP_TUPLE_SIZE(PP_COMPLEX_ENUM_CLASS_ELEMS(n, args)),  \
                                case_macro,    \
                                PP_COMPLEX_ENUM_CLASS_FORWARD_NAME_ELEMS(n, args)    \
                           )   \
        default:    \
            if (0 != PP_ENUM_VALUE(en & PP_COMPLEX_ENUM_CLASS_QMASK(n, args)))    \
            {   \
                auto const fl = ostr.flags();  \
                ostr << ::std::hex << ::std::showbase << ::std::uppercase <<    \
                    PP_ENUM_VALUE(en & PP_COMPLEX_ENUM_CLASS_QMASK(n, args)) << ::blank{};  \
                ostr.flags(fl); \
            }   \
        }   \
    }   \
    en &= ~PP_COMPLEX_ENUM_CLASS_QMASK(n, args);

#define PP_COMPLEX_ENUM_CLASS_MAKE_SWITCH(z, n, mask_elements_pairs)  \
    PP_COMPLEX_ENUM_CLASS_MAKE_SWITCH_T(z, n, mask_elements_pairs, PP_COMPLEX_ENUM_CLASS_ADD_MEMBER_CASE)
#define PP_COMPLEX_ENUM_CLASS_MAKE_WSWITCH(z, n, mask_elements_pairs)  \
    PP_COMPLEX_ENUM_CLASS_MAKE_SWITCH_T(z, n, mask_elements_pairs, PP_COMPLEX_ENUM_CLASS_ADD_WMEMBER_CASE)

////////////////////////////////////////////////////////////////////////////////
#define PP_COMPLEX_EMUM_SWITCH_SIZE(mask_element_pairs) BOOST_PP_DIV(BOOST_PP_TUPLE_SIZE(mask_element_pairs), 2)
#define PP_COMPLEX_ENUM_CLASS_FORWARD_ARGS(name, mask_element_pairs)    (name, mask_element_pairs)

#define PP_COMPLEX_ENUM_CLASS_DEFINE_OSTREAM_T(name, mask_element_pairs, char_, make_switch_macro)   \
    template <typename TCharTraits> \
    ::std::basic_ostream<char_, TCharTraits> &operator << ( ::std::basic_ostream<char_, TCharTraits> &ostr,     \
            name en)  \
    {   \
        auto const mask = ::bitfield_mask_t<name>::get_mask(ostr); \
        BOOST_PP_REPEAT(    \
                            PP_COMPLEX_EMUM_SWITCH_SIZE(mask_element_pairs),   \
                            make_switch_macro,    \
                            PP_COMPLEX_ENUM_CLASS_FORWARD_ARGS(name, mask_element_pairs)    \
        )    \
        if (0 != PP_ENUM_VALUE(en))     \
        {   \
            auto const fl = ostr.flags();  \
            ostr << ::std::hex << ::std::showbase << ::std::uppercase <<    \
                PP_ENUM_VALUE(en) << ::blank{}; \
            ostr.flags(fl); \
        }   \
        return ostr;    \
    }   \
     
#define PP_COMPLEX_ENUM_CLASS_DEFINE_OSTREAM(name, mask_element_pairs)  \
    PP_COMPLEX_ENUM_CLASS_DEFINE_OSTREAM_T(    \
        name, mask_element_pairs, char, PP_COMPLEX_ENUM_CLASS_MAKE_SWITCH)
#define PP_COMPLEX_ENUM_CLASS_DEFINE_WOSTREAM(name, mask_element_pairs) \
    PP_COMPLEX_ENUM_CLASS_DEFINE_OSTREAM_T(    \
        name, mask_element_pairs, wchar_t, PP_COMPLEX_ENUM_CLASS_MAKE_WSWITCH)
#define PP_COMPLEX_ENUM_CLASS_DEFINE_OSTREAMS(name, mask_element_pairs)   \
    PP_COMPLEX_ENUM_CLASS_DEFINE_OSTREAM(name, mask_element_pairs)   \
    PP_COMPLEX_ENUM_CLASS_DEFINE_WOSTREAM(name, mask_element_pairs)

////////////////////////////////////////////////////////////////////////////////
#define PP_ADAPT_COMPLEX_ENUM_CLASS(name, mask_element_pairs)   \
    PP_ENUM_ADAPT_ARITHMETIC(name)   \
    PP_BITFIELD_ADAPT_LOGIC(name)    \
    PP_COMPLEX_ENUM_CLASS_DEFINE_OSTREAMS(name, mask_element_pairs)
