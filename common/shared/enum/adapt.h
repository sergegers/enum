#pragma once

CHT_PUSH_WARNINGS()

#include <boost/mpl/arithmetic.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/integral_c.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/size_t.hpp>
#include <boost/mpl/aux_/largest_int.hpp>
#include <boost/mpl/range_c.hpp>

CHT_POP_WARNINGS()

//
// see http://www.boost.org/doc/libs/1_64_0/libs/mpl/test/numeric_ops.cpp
//

namespace boost::mpl
{

// define type to use it in tag dispatching
namespace aux { struct half_open_range_tag {}; }

}   // namespace boost::mpl

namespace cmn
{

struct enum_tag: boost::mpl::integral_c_tag {};
struct enum_range_c_tag: boost::mpl::aux::half_open_range_tag {};

} // namespace cmn 

namespace boost::mpl
{

// enum type
template<> struct plus_impl< cmn::enum_tag, cmn::enum_tag >
{
    template< typename N1, typename N2 >
    struct apply
        : integral_c<typename N1::value_type, N1::value + N2::value>
    {};
};

template<> struct plus_impl< cmn::enum_tag, integral_c_tag >
{
    template< typename N1, typename N2 >
    struct apply
        : integral_c<typename N1::value_type, N1::value + N2::value>
    {};
};

template<> struct minus_impl< cmn::enum_tag, cmn::enum_tag >
{
    template< typename N1, typename N2 >
    struct apply
        : integral_c<typename N1::value_type, N1::value - N2::value>
    {};
};

template<> struct minus_impl< cmn::enum_tag, integral_c_tag >
{
    template< typename N1, typename N2 >
    struct apply
        : integral_c<typename N1::value_type, N1::value - N2::value>
    {};
};

template<> struct equal_to_impl< cmn::enum_tag, cmn::enum_tag >
{
    template< typename N1, typename N2 > 
    struct apply: 
        bool_<N1::value == N2::value>
    {};
};

template<> struct less_impl< cmn::enum_tag, cmn::enum_tag >
{
    template< typename N1, typename N2 >
    struct apply :
        bool_<N1::value < N2::value>
    {};
};

// range_c type
template<>
struct size_impl< cmn::enum_range_c_tag >
{
    template< typename Range > struct apply
        : size_t<static_cast<std::size_t>(Range::finish::value - Range::start::value)>
    {};
};

} // namespace boost::mpl

// declare it in a global scope
#define CHT_PP_ENUM_CLASS_MPL_ADAPT(T_)    \
    namespace boost::mpl   \
    {   \
        template <T_ N_>   \
        struct integral_c<T_, N_>  \
        {   \
            using tag = cmn::enum_tag; \
            static constexpr T_ value = N_;   \
            using type = integral_c<T_, N_>;  \
            using value_type = T_; \
        };  \
        \
        template<T_ N_>    \
        struct next<integral_c<T_, N_>>:    \
            plus<integral_c<T_, N_>, int_<1>>   \
        {}; \
        \
        template<T_ N_>    \
        struct prior<integral_c<T_, N_>>:    \
            minus<integral_c<T_, N_>, int_<1>>   \
        {}; \
        \
        template<T_ Start_, T_ Finish_>  \
        struct range_c<T_, Start_, Finish_> \
        {   \
            using tag = cmn::enum_range_c_tag;    \
            using value_type = T_;  \
            using type = range_c<T_, Start_, Finish_>;  \
            \
            using start = integral_c<T_, Start_>;   \
            using finish = integral_c<T_, Finish_>; \
            \
            using begin = r_iter<start>;    \
            using end = r_iter<finish>; \
        };  \
    }

