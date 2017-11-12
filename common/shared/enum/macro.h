#pragma once

#include <utility>
#include <tuple>
#include <array>
#include <type_traits>

// ReSharper disable CppUnusedIncludeDirective

CHT_PUSH_WARNINGS()

// boost.preprocessor
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/wstringize.hpp>
#include <boost/preprocessor/punctuation/comma.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>

CHT_POP_WARNINGS()

// ReSharper restore CppUnusedIncludeDirective

////////////////////////////////////////////////////////////////////////////////
// common enum macros
////////////////////////////////////////////////////////////////////////////////
#define CHT_PP_BEGIN_DECLARE_ENUM_CLASS(NAME_)    \
    enum class NAME_    \
    {

#define CHT_PP_BEGIN_DECLARE_ENUM_CLASS_BASE(NAME_, BASE_)    \
    enum class NAME_: BASE_    \
    {

#define CHT_PP_BEGIN_DECLARE_ENUM(NAME_)    \
    enum NAME_    \
    {

#define CHT_PP_BEGIN_DECLARE_ENUM_BASE(NAME_, BASE_)    \
    enum NAME_: BASE_    \
    {

#define CHT_PP_END_DECLARE_ENUM()  \
    };

#define ADD_ENUM_ELEM_1(...) (DECLARE_ENUM_ELEM(__VA_ARGS__)) ADD_ENUM_ELEM_2 
#define ADD_ENUM_ELEM_2(...) (DECLARE_ENUM_ELEM(__VA_ARGS__)) ADD_ENUM_ELEM_1 
#define ADD_ENUM_ELEM_1_END 
#define ADD_ENUM_ELEM_2_END 

#define DECLARE_ENUM_ELEM_1(A) A
#define DECLARE_ENUM_ELEM_2(A, B) A = B

#define DECLARE_ENUM_ELEM(...) \
    BOOST_PP_CAT(BOOST_PP_OVERLOAD(DECLARE_ENUM_ELEM_, __VA_ARGS__)(__VA_ARGS__), BOOST_PP_EMPTY())

#define DECLARE_ENUM_GROUP(GROUP_SEQ_)    \
        BOOST_PP_SEQ_FOR_EACH_I(MAKE_ENUM_ELEM_DEF, ~, GROUP_SEQ_)

#define MAKE_ENUM_ELEM_DEF(_1, _2, I_, ITEM_)    \
    BOOST_PP_CAT(ITEM_, BOOST_PP_COMMA())

////////////////////////////////////////////////////////////////////////////////

#define ADD_RECORD_ELEM_1(...) (DECLARE_RECORD_ELEM(__VA_ARGS__)) ADD_RECORD_ELEM_2 
#define ADD_RECORD_ELEM_2(...) (DECLARE_RECORD_ELEM(__VA_ARGS__)) ADD_RECORD_ELEM_1 
#define ADD_RECORD_ELEM_1_END 
#define ADD_RECORD_ELEM_2_END 

#define DECLARE_RECORD_ELEM_1(ELEM_) ELEM_
#define DECLARE_RECORD_ELEM_2(ELEM_, VAL_) ELEM_

#define DECLARE_RECORD_ELEM(...) \
    BOOST_PP_OVERLOAD(DECLARE_RECORD_ELEM_, __VA_ARGS__)(__VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
#define CHT_PP_BEGIN_DECLARE_ENUM_META_PFX(PFX_, NAME_, kind)    \
    PFX_ constexpr auto get_kind(NAME_) { return ::cmn::enum_::kkind_t< ::cmn::enum_::kind_t::kind>{}; }    \
    PFX_ constexpr auto get_name(NAME_)     \
        { using namespace ::std::string_view_literals; return BOOST_PP_CAT(BOOST_PP_STRINGIZE(NAME_), sv); }    \
    PFX_ constexpr auto get_wname(NAME_)    \
        { using namespace ::std::string_view_literals; return BOOST_PP_CAT(BOOST_PP_WSTRINGIZE(NAME_), sv); }  \
    PFX_ constexpr auto get_groups(NAME_)   \
    {   \
        using namespace ::std::string_view_literals;    \
        using utils_type = ::cmn::enum_::utils<NAME_>;  \
        return ::std::make_tuple(

#define CHT_PP_END_DECLARE_ENUM_META()  \
        );   \
    }

#define CHT_PP_BEGIN_DECLARE_ENUM_META(NAME_, kind)    \
    CHT_PP_BEGIN_DECLARE_ENUM_META_PFX(, NAME_, kind)

#define CHT_PP_BEGIN_DECLARE_ENUM_META_INNER(NAME_, kind)    \
    CHT_PP_BEGIN_DECLARE_ENUM_META_PFX(friend, NAME_, kind)

////////////////////////////////////////////////////////////////////////////////

#define FWD_MASK(NAME_)             ::cmn::enum_::op::to_mask_t<NAME_>
#define ARG_CAST(NAME_, e)          static_cast<NAME_>(e)
#define FWD_ARG_1(NAME_, e)         ( ::cmn::enum_::op::fwd<NAME_>(e))
#define FWD_ARG_2(LNAME_, lhs, RNAME_, rhs)  \
    ( ::cmn::enum_::op::fwd<LNAME_>(lhs) BOOST_PP_COMMA() ::cmn::enum_::op::fwd<RNAME_>(rhs))

#define CHT_PP_ENUM_CONSTEXPR_BINARY_INTEROPERABLE_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, op_)  \
    PFX_ constexpr auto operator op_ (NAME_ __lhs, FWD_MASK(NAME_) __rhs)  \
    { return ::cmn::enum_::op::operator op_ FWD_ARG_2(NAME_, __lhs, NAME_, ARG_CAST(NAME_, __rhs)); }   \
    \
    PFX_ constexpr auto operator op_ (FWD_MASK(NAME_) __lhs, NAME_ __rhs)  \
    { return ::cmn::enum_::op::operator op_ FWD_ARG_2(NAME_, ARG_CAST(NAME_, __lhs), NAME_, __rhs); }

#define CHT_PP_ENUM_CONSTEXPR_BINARY_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, op_)  \
    PFX_ constexpr auto operator op_ (NAME_ __lhs, NAME_ __rhs)  \
    { return ::cmn::enum_::op::operator op_ FWD_ARG_2(NAME_, __lhs, NAME_, __rhs); }   \
    CHT_PP_ENUM_CONSTEXPR_BINARY_INTEROPERABLE_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, op_)

#define CHT_PP_ENUM_CONSTEXPR_UNARY_OP_PFX_FWD(PFX_, NAME_, op_)   \
    PFX_ constexpr auto operator op_ (NAME_ __e)  \
    { return ::cmn::enum_::op::operator op_ FWD_ARG_1(NAME_, __e); }

#define CHT_PP_ENUM_MUTABLE_PREFIX_UNARY_OP_PFX_FWD(PFX_, NAME_, op_)  \
    PFX_ inline auto operator op_ (NAME_ &__e)  \
    { return ::cmn::enum_::op::operator op_ FWD_ARG_1(NAME_ &, __e); }

#define CHT_PP_ENUM_MUTABLE_POSTFIX_UNARY_OP_PFX_FWD(PFX_, NAME_, op_)  \
    PFX_ inline auto operator op_ (NAME_ &__e, int __i)  \
    {   \
        return ::cmn::enum_::op::operator op_ ( \
            ::cmn::enum_::op::fwd<NAME_ &>(__e) BOOST_PP_COMMA() __i); \
    }

// #possibly bug VS2017 TS140: can't find unqualified enum type name in the class scope
#define CHT_PP_ENUM_OUTPUT_OP_PFX_FWD(PFX_, NAME_)  \
    template <typename Char, typename CharTraits>   \
    PFX_ auto operator << ( ::std::basic_ostream<Char, CharTraits> &ostr, NAME_ __e)->  \
        ::std::basic_ostream<Char, CharTraits> & \
    {   \
        return ::cmn::enum_::io::print (   \
            ostr BOOST_PP_COMMA()   \
            ::cmn::enum_::op::fwd<decltype(__e)>(__e) BOOST_PP_COMMA()    \
            get_kind(__e));     \
    }

#define CHT_PP_ENUM_COMPOUND_OP_PFX_FWD(PFX_, NAME_, op_)  \
    PFX_ inline auto operator op_ (NAME_ &__lhs, NAME_ __rhs)  \
    { return ::cmn::enum_::op::operator op_ FWD_ARG_2(NAME_ &, __lhs, NAME_, __rhs); }   \
    \
    PFX_ inline auto operator op_ (NAME_ &__lhs, FWD_MASK(NAME_) __rhs)  \
    { return ::cmn::enum_::op::operator op_ FWD_ARG_2(NAME_ &, __lhs, NAME_, ARG_CAST(NAME_, __rhs)); }

#define CHT_PP_ENUM_DECLARE_UNIT_STEPPABLE_PFX_FWD(PFX_, NAME_)  \
    CHT_PP_ENUM_MUTABLE_PREFIX_UNARY_OP_PFX_FWD(PFX_, NAME_, ++) \
    CHT_PP_ENUM_MUTABLE_POSTFIX_UNARY_OP_PFX_FWD(PFX_, NAME_, ++)   \
    CHT_PP_ENUM_MUTABLE_PREFIX_UNARY_OP_PFX_FWD(PFX_, NAME_, --) \
    CHT_PP_ENUM_MUTABLE_POSTFIX_UNARY_OP_PFX_FWD(PFX_, NAME_, --)

#define CHT_PP_ENUM_DECLARE_BITWISE_PFX_FWD(PFX_, NAME_)  \
    CHT_PP_ENUM_CONSTEXPR_BINARY_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, |)      \
    CHT_PP_ENUM_CONSTEXPR_BINARY_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, &)      \
    CHT_PP_ENUM_CONSTEXPR_BINARY_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, ^)      \
    CHT_PP_ENUM_CONSTEXPR_UNARY_OP_PFX_FWD(PFX_, NAME_, ~)  \
    CHT_PP_ENUM_COMPOUND_OP_PFX_FWD(PFX_, NAME_, |=)    \
    CHT_PP_ENUM_COMPOUND_OP_PFX_FWD(PFX_, NAME_, &=)    \
    CHT_PP_ENUM_COMPOUND_OP_PFX_FWD(PFX_, NAME_, ^=)

#define CHT_PP_ENUM_DECLARE_EQUALITY_COMPARABLE_PFX_FWD(PFX_, NAME_)  \
    CHT_PP_ENUM_CONSTEXPR_BINARY_INTEROPERABLE_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, ==)     \
    CHT_PP_ENUM_CONSTEXPR_BINARY_INTEROPERABLE_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, !=)

#define CHT_PP_ENUM_DECLARE_COMPARABLE_PFX_FWD(PFX_, NAME_)  \
    CHT_PP_ENUM_CONSTEXPR_BINARY_INTEROPERABLE_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, >)      \
    CHT_PP_ENUM_CONSTEXPR_BINARY_INTEROPERABLE_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, >=)     \
    CHT_PP_ENUM_CONSTEXPR_BINARY_INTEROPERABLE_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, <)      \
    CHT_PP_ENUM_CONSTEXPR_BINARY_INTEROPERABLE_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, <=)      

#define CHT_PP_ENUM_DECLARE_TOTALLY_ORDERED_PFX_FWD(PFX_, NAME_)  \
    CHT_PP_ENUM_DECLARE_EQUALITY_COMPARABLE_PFX_FWD(PFX_, NAME_)  \
    CHT_PP_ENUM_DECLARE_COMPARABLE_PFX_FWD(PFX_, NAME_)

#define CHT_PP_ENUM_DECLARE_ARIPHMETIC_PFX_FWD(PFX_, NAME_)  \
    CHT_PP_ENUM_CONSTEXPR_BINARY_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, +)      \
    CHT_PP_ENUM_CONSTEXPR_BINARY_COMMUTATIVE_OP_PFX_FWD(PFX_, NAME_, -)      \

#define CHT_PP_ENUM_DECLARE_IO_PFX_FWD(PFX_, NAME_)    \
    CHT_PP_ENUM_OUTPUT_OP_PFX_FWD(PFX_, NAME_)

////////////////////////////////////////////////////////////////////////////////
#define CHT_PP_ENUM_DECLARE_UNIT_STEPPABLE(NAME_)  \
    CHT_PP_ENUM_DECLARE_UNIT_STEPPABLE_PFX_FWD(, NAME_)

#define CHT_PP_ENUM_DECLARE_BITWISE(NAME_)  \
    CHT_PP_ENUM_DECLARE_BITWISE_PFX_FWD(, NAME_)

#define CHT_PP_ENUM_DECLARE_EQUALITY_COMPARABLE(NAME_)  \
    CHT_PP_ENUM_DECLARE_EQUALITY_COMPARABLE_PFX_FWD(, NAME_)

#define CHT_PP_ENUM_DECLARE_COMPARABLE(NAME_)  \
    CHT_PP_ENUM_DECLARE_COMPARABLE_PFX_FWD(, NAME_)

#define CHT_PP_ENUM_DECLARE_TOTALLY_ORDERED(NAME_)  \
    CHT_PP_ENUM_DECLARE_TOTALLY_ORDERED_PFX_FWD(, NAME_)

#define CHT_PP_ENUM_DECLARE_ARIPHMETIC(NAME_)  \
    CHT_PP_ENUM_DECLARE_ARIPHMETIC_PFX_FWD(, NAME_)

#define CHT_PP_ENUM_DECLARE_IO(NAME_)    \
    CHT_PP_ENUM_DECLARE_IO_PFX_FWD(, NAME_)    \

////////////////////////////////////////////////////////////////////////////////
#define CHT_PP_ENUM_INNER_DECLARE_UNIT_STEPPABLE(NAME_)  \
    CHT_PP_ENUM_DECLARE_UNIT_STEPPABLE_PFX_FWD(friend, NAME_)

#define CHT_PP_ENUM_INNER_DECLARE_BITWISE(NAME_)  \
    CHT_PP_ENUM_DECLARE_BITWISE_PFX_FWD(friend, NAME_)

#define CHT_PP_ENUM_INNER_DECLARE_EQUALITY_COMPARABLE(NAME_)  \
    CHT_PP_ENUM_DECLARE_EQUALITY_COMPARABLE_PFX_FWD(friend, NAME_)

#define CHT_PP_ENUM_INNER_DECLARE_COMPARABLE(NAME_)  \
    CHT_PP_ENUM_DECLARE_COMPARABLE_PFX_FWD(friend, NAME_)

#define CHT_PP_ENUM_INNER_DECLARE_TOTALLY_ORDERED(NAME_)  \
    CHT_PP_ENUM_DECLARE_TOTALLY_ORDERED_PFX_FWD(friend, NAME_)

#define CHT_PP_ENUM_INNER_DECLARE_ARIPHMETIC(NAME_)  \
    CHT_PP_ENUM_DECLARE_ARIPHMETIC_PFX_FWD(friend, NAME_)

#define CHT_PP_ENUM_INNER_DECLARE_IO(NAME_)    \
    CHT_PP_ENUM_DECLARE_IO_PFX_FWD(friend, NAME_)    \

////////////////////////////////////////////////////////////////////////////////
#define CHT_PP_QNAME(CLASS_, NAME_) BOOST_PP_CAT(CLASS_, BOOST_PP_CAT(::, NAME_))

#define CHT_PP_DEFINE_ENUM_CONST(ITEM_)     (ITEM_##_ BOOST_PP_COMMA() ITEM_)