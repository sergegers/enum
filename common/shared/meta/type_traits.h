#pragma once

#include <memory>

CHT_PUSH_WARNINGS()

// boost.mpl
#include <boost/mpl/vector.hpp>
#include <boost/mpl/empty.hpp>
// boost.traits
#include <boost/type_traits/function_traits.hpp>
#include <boost/type_traits/detail/yes_no_type.hpp>
// boost.range
#include <boost/range/iterator_range_core.hpp>
// boost.preprocessor
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/arithmetic/sub.hpp>
#include <boost/preprocessor/inc.hpp>

// sdk
#include <guiddef.h>    // for GUID
#include <Unknwn.h>     // for IUnknown

CHT_POP_WARNINGS()

namespace cmn::meta
{

////////////////////////////////////////////////////////////////////////////////
// Trait template <typename T> is_com_interface
////////////////////////////////////////////////////////////////////////////////
namespace detail
{

template <typename T, const ::GUID &> struct uuid_maker;

template <typename T>
struct has_uuid
{
    template <typename U>
    static std::true_type apply(int, uuid_maker<U, __uuidof(U)> * = nullptr);

    template <typename U>
    static std::false_type apply(...);

    using type = decltype(apply<T>(0));
    static constexpr bool value = type::value;
};

template <typename T>
constexpr bool has_uuid_v = has_uuid<T>::value;

}	// namespace detail

template <typename T>
constexpr bool is_com_interface_v = 
    std::is_base_of_v< ::IUnknown, T> && 
    std::is_abstract_v<T> && 
    detail::has_uuid_v<T>;

template <typename T>
using is_com_interface = std::bool_constant<is_com_interface_v<T>>;

////////////////////////////////////////////////////////////////////////////////
// Trait template <typename On, typename T> struct add_const_on
////////////////////////////////////////////////////////////////////////////////
template <typename On, typename T, typename Const = std::add_const_t<T>>
struct add_const_on : boost::mpl::if_c<std::is_const_v<On>, Const, T> {};

template <bool On_, typename T, typename Const = std::add_const_t<T>>
struct add_const_on_c : boost::mpl::if_c<On_, Const, T> {};

template <typename On, typename T, typename Const = std::add_const_t<T>>
using add_const_on_t = typename add_const_on<On, T, Const>::type;

template <bool On_, typename T, typename Const = std::add_const_t<T>>
using add_const_on_c_t = typename add_const_on_c<On_, T, Const>::type;

////////////////////////////////////////////////////////////////////////////////
// Trait template <typename On, typename T> struct remove_const_on
////////////////////////////////////////////////////////////////////////////////
template <typename On, typename T, typename NonConst = std::remove_const_t<T>>
struct remove_const_on : boost::mpl::if_c<!std::is_const_v<On>, NonConst, T> {};

template <bool On_, typename T, typename NonConst = std::remove_const_t<T>>
struct remove_const_on_c : boost::mpl::if_c<On_, NonConst, T> {};

template <typename On, typename T, typename NonConst = std::remove_const_t<T>>
using remove_const_on_t = typename remove_const_on<On, T, NonConst>::type;

template <bool On_, typename T, typename NonConst = std::remove_const_t<T>>
using remove_const_on_c_t = typename remove_const_on_c<On_, T, NonConst>::type;

////////////////////////////////////////////////////////////////////////////////
// Trait template <typename On, typename T> struct deep_add_const_on
////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct deep_add_const : boost::mpl::identity<T const> {};

template <typename Ref>
struct deep_add_const<Ref &> : boost::mpl::identity<Ref const &> {};

template <typename Ptr>
struct deep_add_const<Ptr *> : boost::mpl::identity<Ptr const * const> {};

template <typename T>
using deep_add_const_t = typename deep_add_const<T>::type;

template <typename On, typename T, typename Const = deep_add_const_t<T>>
struct deep_add_const_on : boost::mpl::if_c<std::is_const_v<On>, Const, T> {};

template <typename OnRef, typename T, typename Const>
struct deep_add_const_on<OnRef &, T, Const> :
    boost::mpl::if_c<std::is_const_v<OnRef>, deep_add_const_t<Const>, T> {};

template <typename OnPtr, typename T, typename Const>
struct deep_add_const_on<OnPtr *, T, Const> :
    boost::mpl::if_c<std::is_const_v<OnPtr>, deep_add_const_t<Const>, T> {};

template <typename On, typename T, typename Const = deep_add_const_t<T>>
using deep_add_const_on_t = typename deep_add_const_on<On, T, Const>::type;

////////////////////////////////////////////////////////////////////////////////
// Trait template <typename On, typename T> struct add_volatile_on
////////////////////////////////////////////////////////////////////////////////
template <typename On, typename T, typename Volatile = std::add_volatile_t<T>>
struct add_volatile_on : boost::mpl::if_c<std::is_volatile_v<On>, Volatile, T> {};

template <typename On, typename T, typename Volatile = std::add_volatile_t<T>>
using add_volatile_on_t = typename add_volatile_on<On, T, Volatile>::type;

////////////////////////////////////////////////////////////////////////////////
// Trait template <typename On, typename T> struct add_rvalue_reference_on
////////////////////////////////////////////////////////////////////////////////
template <typename On, typename T, typename RvalRef = std::add_rvalue_reference_t<T>>
struct add_rvalue_reference_on : boost::mpl::if_c<std::is_rvalue_reference_v<On>, RvalRef, T> {};

template <typename On, typename T, typename RValRef = std::add_rvalue_reference_t<T>>
using add_rvalue_reference_on_t = typename add_rvalue_reference_on<On, T, RValRef>::type;

////////////////////////////////////////////////////////////////////////////////
// Trait template <typename On, typename T> struct add_lvalue_reference_on
////////////////////////////////////////////////////////////////////////////////
template <typename On, typename T, typename LvalRef = std::add_lvalue_reference_t<T>>
struct add_lvalue_reference_on : boost::mpl::if_c<std::is_lvalue_reference_v<On>, LvalRef, T> {};

template <typename On, typename T, typename LValRef = std::add_lvalue_reference_t<T>>
using add_lvalue_reference_on_t = typename add_lvalue_reference_on<On, T, LValRef>::type;

////////////////////////////////////////////////////////////////////////////////
// Trait template <typename On, typename T> struct undecay_on
////////////////////////////////////////////////////////////////////////////////
template <typename On, typename T>
struct undecay_on:
    add_lvalue_reference_on
    <
        On,
        add_rvalue_reference_on_t
        <
            On, 
            add_volatile_on_t
            <
                std::remove_reference_t<On>,
                add_const_on_t
                <
                    std::remove_reference_t<On>, 
                    T
                >
            >
        >
    >
{};

template <typename On, typename T>
using undecay_on_t = typename undecay_on<On, T>::type;

////////////////////////////////////////////////////////////////////////////////
// remove_rvalue_reference
////////////////////////////////////////////////////////////////////////////////
template <typename T> struct remove_rvalue_reference: boost::mpl::identity<T> {};
template <typename T> struct remove_rvalue_reference<T &&>: boost::mpl::identity<T> {};

template <typename T> 
using remove_rvalue_reference_t = typename remove_rvalue_reference<T>::type;

////////////////////////////////////////////////////////////////////////////////
// remove_lvalue_reference
////////////////////////////////////////////////////////////////////////////////
template <typename T> struct remove_lvalue_reference: boost::mpl::identity<T> {};
template <typename T> struct remove_lvalue_reference<T &>: boost::mpl::identity<T> {};

template <typename T> 
using remove_lvalue_reference_t = typename remove_lvalue_reference<T>::type;

////////////////////////////////////////////////////////////////////////////////
// array_size metafunction: always compiled
////////////////////////////////////////////////////////////////////////////////
template<class T> struct array_size:
    std::integral_constant<std::size_t, std::numeric_limits<std::size_t>::max()> {};
template<class T, std::size_t N_> struct array_size<T[N_]>: std::integral_constant<std::size_t, N_> {};
template<class T> struct array_size<T[]>: std::integral_constant<std::size_t, 0> {};

template<class T> 
constexpr std::size_t array_size_v = array_size<T>::value;

////////////////////////////////////////////////////////////////////////////////
// metafunction is_related
////////////////////////////////////////////////////////////////////////////////
template <typename Left, typename Right>
constexpr bool is_related_v = std::is_base_of_v<Left, Right> || std::is_base_of_v<Right, Left>;

template <typename Left, typename Right>
using is_related = std::bool_constant<is_related_v<Left, Right>>;

////////////////////////////////////////////////////////////////////////////////
// template <typename Func, std::size_t N_> struct arg_type_c
// boost function traits extension
////////////////////////////////////////////////////////////////////////////////
template <typename Func, std::size_t N_> struct arg_type_c;

#define CHT_PP_FUNC_ARG_TYPE(z, N_, _)   \
    template <typename Func>   \
    struct arg_type_c<Func, N_>  \
    {   \
        using type = typename ::boost::function_traits<Func>::   \
            BOOST_PP_CAT(arg, BOOST_PP_CAT(BOOST_PP_INC(N_), _type));    \
    };

#ifndef CHT_PP_FUN_ARG_MAX_ARITY
    #define CHT_PP_FUN_ARG_MAX_ARITY    10
#endif // !CHT_PP_FUN_ARG_MAX_ARITY

BOOST_PP_REPEAT(CHT_PP_FUN_ARG_MAX_ARITY, CHT_PP_FUNC_ARG_TYPE, nil)

template <typename Func, std::size_t N_>
using arg_type_c_t = typename arg_type_c<Func, N_>::type;

template <typename Func, typename N> 
struct arg_type : arg_type_c<Func, N::value> {};

template <typename Func, typename N>
using arg_type_t = typename arg_type<Func, N>::type;

////////////////////////////////////////////////////////////////////////////////
// Metafunction 
// template <typename F, typename... Args> using can_call
////////////////////////////////////////////////////////////////////////////////
namespace detail::can_call_
{

// #bug: VS 2015 U2 RC compiler bug. It cannot correctly process function 
// signatures with references.
template <typename F> struct nonref;

template <typename Res, typename... Args>
struct nonref<Res(Args...)>
{
    using type = Res(std::remove_reference_t<Args>...);
};

template <typename F>
using nonref_t = typename nonref<F>::type;

template <typename F, typename... Args>
struct can_call_impl
{
    template <typename F0, typename... Args0>
    static auto apply() -> 
        decltype
        (
            std::declval<nonref_t<F0> >()(std::declval<Args0>()...), 
            std::declval<std::true_type>()
        );

    template <typename F0, typename... Args0>
    static std::false_type apply(...);

    static constexpr bool value = decltype(apply<F, Args...>())::value;
};

}   // namespace detail::can_call_

template <typename F, typename... Args>
using can_call = std::bool_constant<detail::can_call_::can_call_impl<F, Args...>::value>;

template <typename F, typename... Args>
constexpr bool can_call_v = can_call<F, Args...>::value;

////////////////////////////////////////////////////////////////////////////////
// can_deref
////////////////////////////////////////////////////////////////////////////////
template <typename T, typename Enable = void>
struct can_deref : std::false_type {};

template <typename T>
struct can_deref
<
    T, 
    decltype(*std::declval<T>(), std::declval<void>())
> : 
    std::true_type {};

template <typename T>
constexpr bool can_deref_v = can_deref<T>::value;

////////////////////////////////////////////////////////////////////////////////
// is_unique_ptr
////////////////////////////////////////////////////////////////////////////////
template <typename Ptr>
struct is_unique_ptr : std::false_type {};

template <typename T, typename Del>
struct is_unique_ptr<typename std::unique_ptr<T, Del> > : std::true_type {};

template <typename Ptr>
using is_unique_ptr_t = typename is_unique_ptr<Ptr>::type;

template <typename Ptr>
constexpr bool is_unique_ptr_v = is_unique_ptr<Ptr>::value;

////////////////////////////////////////////////////////////////////////////////
// is_comparable<>
// http://stackoverflow.com/a/33196728
////////////////////////////////////////////////////////////////////////////////
template<typename Lhs, typename Rhs = Lhs, class = void>
struct is_comparable : std::false_type {};

template<typename Lhs, typename Rhs>
using comparability = decltype(std::declval<Lhs>() == std::declval<Rhs>());

template<typename Lhs, typename Rhs>
struct is_comparable<Lhs, Rhs, std::void_t<comparability<Lhs, Rhs>>> : std::true_type {};

// #bug: probably a bug, default parameters are not supported for
// template variables. VS 2015 U3
template<typename Lhs, typename Rhs /*= Lhs*/>
constexpr bool is_comparable_v = is_comparable<Lhs, Rhs>::value;

////////////////////////////////////////////////////////////////////////////////
// metafunction is_range
////////////////////////////////////////////////////////////////////////////////
template <typename T, typename = void>
struct is_range: std::false_type {};

template <typename T>
struct is_range
<
    T, 
    std::enable_if_t<boost::is_base_and_derived<boost::iterator_range_detail::iterator_range_tag, T>::value>
> :
    std::true_type
{};

class flatten_range_tag {};

template <typename T>
struct is_range
<
    T, 
    std::enable_if_t<boost::is_base_and_derived<flatten_range_tag, T>::value>
> :
    std::true_type
{};

template <typename T>
using is_range_t = typename is_range<T>::type;

template <typename T>
constexpr bool is_range_v = is_range<T>::value;

////////////////////////////////////////////////////////////////////////////////
// metafunction is_container
////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct is_container
{
    template <typename U>
    static std::true_type apply
    (
        int,
        decltype(std::begin(std::declval<U>())) * = nullptr, 
        decltype(std::end(std::declval<U>())) * = nullptr
    );

    template <typename U>
    static std::false_type apply(...);

    using almost_type = decltype(apply<T>(0));

    static constexpr bool value = almost_type::value && !is_range_v<T>;
    using type = std::bool_constant<value>;
};

template <typename T>
constexpr bool is_container_v = is_container<T>::value;

////////////////////////////////////////////////////////////////////////////////
// metafunction is_scoped_enum
// https://stackoverflow.com/a/26936868
////////////////////////////////////////////////////////////////////////////////
template <class T>
using is_scoped_enum = std::integral_constant<bool, !std::is_convertible_v<T, int> && std::is_enum_v<T>>;

template <typename T>
constexpr bool is_scoped_enum_v = is_scoped_enum<T>::value;

////////////////////////////////////////////////////////////////////////////////
// symbols
////////////////////////////////////////////////////////////////////////////////
template <typename Char, typename CharTraits = std::char_traits<Char>> struct symbols;

template <>
struct symbols<char, std::char_traits<char>>
{
    static constexpr decltype(auto) space() { return " "; }
    static constexpr decltype(auto) nothing() { return ""; }
    static constexpr decltype(auto) open_square_bracket() { return "["; }
    static constexpr decltype(auto) close_square_bracket() { return "]"; }
    static constexpr decltype(auto) open_angle_bracket() { return "<"; }
    static constexpr decltype(auto) close_angle_bracket() { return ">"; }
    static constexpr decltype(auto) open_figure_bracket() { return "{"; }
    static constexpr decltype(auto) close_figure_bracket() { return "}"; }
    static constexpr decltype(auto) colon() { return ": "; }
    static constexpr decltype(auto) comma() { return ", "; }
    static constexpr decltype(auto) plus() { return "+"; }
    static constexpr decltype(auto) minus() { return "-"; }
    static constexpr decltype(auto) zero() { return "0"; }
    static constexpr decltype(auto) asterisk() { return "*"; }
    static constexpr decltype(auto) ampersand() { return "&"; }
    static constexpr decltype(auto) end() { return "end"; }
    static constexpr decltype(auto) empty() { return "empty"; }
    static constexpr decltype(auto) endl() { return "\n"; }

    static constexpr auto c_open_angle_bracket() { return '<'; }
    static constexpr auto c_close_angle_bracket() { return '>'; }
    static constexpr auto c_endl() { return '\n'; }
    static constexpr auto c_ends() { return '\0'; }
};

template <>
struct symbols<wchar_t, std::char_traits<wchar_t>>
{
    static constexpr decltype(auto) space() { return L" "; }
    static constexpr decltype(auto) nothing() { return L""; }
    static constexpr decltype(auto) open_square_bracket() { return L"["; }
    static constexpr decltype(auto) close_square_bracket() { return L"]"; }
    static constexpr decltype(auto) open_angle_bracket() { return L"<"; }
    static constexpr decltype(auto) close_angle_bracket() { return L">"; }
    static constexpr decltype(auto) open_figure_bracket() { return "{"; }
    static constexpr decltype(auto) close_figure_bracket() { return "}"; }
    static constexpr decltype(auto) colon() { return L": "; }
    static constexpr decltype(auto) comma() { return L", "; }
    static constexpr decltype(auto) plus() { return L"+"; }
    static constexpr decltype(auto) minus() { return L"-"; }
    static constexpr decltype(auto) zero() { return L"0"; }
    static constexpr decltype(auto) asterisk() { return L"*"; }
    static constexpr decltype(auto) ampersand() { return L"&"; }
    static constexpr decltype(auto) end() { return L"end"; }
    static constexpr decltype(auto) empty() { return L"empty"; }
    static constexpr decltype(auto) endl() { return L"\n"; }

    static constexpr auto c_open_angle_bracket() { return L'<'; }
    static constexpr auto c_close_angle_bracket() { return L'>'; }
    static constexpr auto c_endl() { return L'\n'; }
    static constexpr auto c_ends() { return L'\0'; }
};

}	// namespace cmn::meta

