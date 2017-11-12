#pragma once

CHT_PUSH_WARNINGS()

#include <boost/mpl/vector.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/mpl/begin.hpp>

CHT_POP_WARNINGS()

#include "type_traits.h"
#include "enum_common.h"

/************************************************************************/
/* EXT_ENUM                                                             */
/************************************************************************/
#define CHT_PP_EXT_ENUM_ADD_MEMBER_CASE(z, n, elements)    \
    case BOOST_PP_TUPLE_ELEM(n, elements)_:    \
    ostr << BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(n, elements));  \
    break;

#define CHT_PP_EXT_ENUM_ADD_WMEMBER_CASE(z, n, elements)    \
    case BOOST_PP_TUPLE_ELEM(n, elements)_:    \
    ostr << BOOST_PP_WSTRINGIZE(BOOST_PP_TUPLE_ELEM(n, elements));  \
    break;

#define CHT_PP_EXT_ENUM_ADD_ENUM_MEMBER(z, n, elements)    \
    BOOST_PP_TUPLE_ELEM(n, elements)_ ,

#define CHT_PP_EXT_ENUM_ADD_STATIC_MEMBER(z, n, qelements)    \
    static const CHT_PP_ENUM_GET_PREFIX(qelements) BOOST_PP_TUPLE_ELEM(n, CHT_PP_ENUM_GET_ELEMS(qelements));

#define CHT_PP_EXT_ENUM_ADD_STATIC_MEMBER_INIT(z, n, qelements)    \
    const __declspec(selectany) CHT_PP_ENUM_GET_PREFIX(qelements) \
    CHT_PP_ENUM_GET_PREFIX(qelements)::BOOST_PP_TUPLE_ELEM(n, CHT_PP_ENUM_GET_ELEMS(qelements))   \
        = CHT_PP_ENUM_GET_PREFIX(qelements)::BOOST_PP_CAT( \
                BOOST_PP_TUPLE_ELEM(n, CHT_PP_ENUM_GET_ELEMS(qelements)), _);

#define CHT_PP_EXT_ENUM_DEFINE_OSTREAM(name)  \
    inline ::std::ostream &operator << ( ::std::ostream &ostr, name const en)  \
    {   \
        ostr << BOOST_PP_STRINGIZE(BOOST_PP_CAT(name, ::)); \
        en.naked_dump(ostr);    \
        return ostr;    \
    }   \
    \
    inline ::std::wostream &operator << ( ::std::wostream &ostr, name const en)  \
    {   \
        ostr << BOOST_PP_WSTRINGIZE(BOOST_PP_CAT(name, ::)); \
        en.naked_dump(ostr);    \
        return ostr;    \
    }

#define CHT_PP_EXT_ENUM_DEFINE_ARITHMETIC(name)   \
    name &operator ++ ()     \
    {   \
        return (*this = name(get_value() + 1));     \
    }   \
    name &operator ++ (int)    \
    {   \
        return (*this = name(get_value() + 1));    \
    } \
    name &operator -- () \
    {   \
        return (*this = name(get_value() - 1));    \
    }  \
    name &operator -- (int)    \
    {   \
        return (*this = name(get_value() - 1));    \
    }   \
    name operator += (int i)    \
    {   \
        return (*this = name(get_value() + i));    \
    }   \
    name operator + (int i) const    \
    {   \
        return (name(*this) += i);    \
    }   \
    name operator -= (int i)    \
    {   \
        return (*this = name(get_value() - i));    \
    }   \
    name operator - (int i) const    \
    {   \
        return (name(*this) -= i);   \
    }   \
    template <typename Other> typename ::std::enable_if<     \
    ::cmn::meta::is_related<Other, name>::value,  bool>::type \
    operator == (Other const &other) const noexcept    \
    {   \
        return get_value() == other.get_value();    \
    }   \
    template <typename Other> typename ::std::enable_if<     \
    ::cmn::meta::is_related<Other, name>::value,  bool>::type \
    operator != (Other const &other) const noexcept    \
    {   \
        return !(*this == other);    \
    }   \
    template <typename Other> typename ::std::enable_if<     \
    ::cmn::meta::is_related<Other, name>::value,  bool>::type \
    operator > (Other const &other) const noexcept    \
    {   \
        return get_value() > other.get_value();    \
    }   \
    template <typename Other> typename ::std::enable_if<     \
    ::cmn::meta::is_related<Other, name>::value,  bool>::type \
    operator <= (Other const &other) const noexcept    \
    {   \
        return !(*this > other);    \
    }   \
    template <typename Other> typename ::std::enable_if<     \
    ::cmn::meta::is_related<Other, name>::value,  bool>::type \
    operator < (Other const &other) const noexcept    \
    {   \
        return get_value() < other.get_value();    \
    }   \
    template <typename Other> typename ::std::enable_if<     \
    ::cmn::meta::is_related<Other, name>::value,  bool>::type \
    operator >= (Other const &other) const noexcept    \
    {   \
        return !(*this < other);    \
    }

/************************************************************************/
/* CHT_PP_DEFINE_EXT_ENUM                                              */
/************************************************************************/
#define CHT_PP_DEFINE_EXT_ENUM(name, elements) \
    class name   \
    { \
    private:    \
        friend ::std::ostream &operator << ( ::std::ostream &ostr, name const en);  \
        friend ::std::wostream &operator << ( ::std::wostream &ostr, name const en);  \
        friend class ::boost::serialization::access;   \
        \
        int m_value;    \
        \
        template <typename Archive>    \
        void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)    \
        {   \
            ar & BOOST_SERIALIZATION_NVP(m_value);   \
        }   \
    protected:  \
        enum underlying_t    \
        {   \
            BOOST_PP_REPEAT(BOOST_PP_TUPLE_SIZE(elements), CHT_PP_EXT_ENUM_ADD_ENUM_MEMBER, elements) \
            BOOST_PP_CAT(cel, name) \
        };  \
        \
        void naked_dump( ::std::ostream &ostr) const  \
        {   \
            switch (get_value())    \
            {   \
                BOOST_PP_REPEAT(BOOST_PP_TUPLE_SIZE(elements), CHT_PP_EXT_ENUM_ADD_MEMBER_CASE, elements)    \
            default:    \
                CHT_PP_ERROR_ENUM_RANGE_ERROR_2(name, get_value());   \
            }   \
        }   \
        void naked_dump( ::std::wostream &ostr) const  \
        {   \
            switch (get_value())    \
            {   \
                BOOST_PP_REPEAT(BOOST_PP_TUPLE_SIZE(elements), CHT_PP_EXT_ENUM_ADD_WMEMBER_CASE, elements)    \
            default:    \
                CHT_PP_ERROR_ENUM_RANGE_ERROR_2(name, get_value());   \
            }   \
        }   \
        template <typename Other>  \
        void static check_bounds(int value)    \
        {   \
            if ((value < Other::begin_t::value) || (value >= Other::end_t::value))  \
                CHT_PP_ERROR_ENUM_RANGE_ERROR_4(name, value,  \
                                                Other::begin_t::value, Other::end_t::value);   \
        }   \
        \
        struct no_check_arg {};    \
        name(int value, no_check_arg ) noexcept: m_value(value) {}  \
    public:     \
        name(int value = 0L):   \
            m_value(value)     \
        {   \
            check_bounds<name>(value);    \
        }  \
        int get_value() const noexcept { return m_value; }   \
        CHT_PP_EXT_ENUM_DEFINE_ARITHMETIC(name)   \
        \
        typedef ::std::integral_constant< \
        underlying_t, static_cast<underlying_t>(0)>                         self_begin_t;    \
        typedef ::std::integral_constant< \
        underlying_t, static_cast<underlying_t>(BOOST_PP_CAT(cel, name))>   self_end_t;  \
        typedef self_begin_t                                                    begin_t;    \
        typedef self_end_t                                                      end_t;  \
        typedef ::boost::mpl::vector<name>                                      parents_t;  \
        \
        BOOST_PP_REPEAT(BOOST_PP_TUPLE_SIZE(elements), CHT_PP_EXT_ENUM_ADD_STATIC_MEMBER, \
                        CHT_PP_ENUM_MAKE_QELEMS(name, elements))    \
    };  \
    \
    BOOST_PP_REPEAT(BOOST_PP_TUPLE_SIZE(elements), CHT_PP_EXT_ENUM_ADD_STATIC_MEMBER_INIT, \
                    CHT_PP_ENUM_MAKE_QELEMS(name, elements))    \
    \
    CHT_PP_EXT_ENUM_DEFINE_OSTREAM(name)

/************************************************************************/
/* CHT_PP_DEFINE_EXT_ENUM_BASE                                         */
/************************************************************************/
#define CHT_PP_EXT_ENUM_BASE_ADD_ENUM_FIRST_MEMBER(elements, value)    \
    BOOST_PP_TUPLE_ELEM(0, elements)_ = value,

#define CHT_PP_DEFINE_EXT_ENUM_BASE(name, elements, base) \
    class name: public base   \
    { \
    private:    \
        friend ::std::ostream &operator << ( ::std::ostream &ostr, name const en);  \
        friend ::std::wostream &operator << ( ::std::wostream &ostr, name const en);  \
        friend class ::boost::serialization::access;   \
        struct enabler{};   \
        \
        template <typename Archive>    \
        void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)    \
        {   \
            ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(base);   \
        }   \
    protected:  \
        enum underlying_t    \
        {   \
            CHT_PP_EXT_ENUM_BASE_ADD_ENUM_FIRST_MEMBER(elements, BOOST_PP_CAT(base, ::self_end_t::value))  \
            BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_TUPLE_SIZE(elements), \
                                    CHT_PP_EXT_ENUM_ADD_ENUM_MEMBER, elements) \
            BOOST_PP_CAT(cel, name) \
        };  \
        \
        void naked_dump( ::std::ostream &ostr) const \
        {   \
            switch (get_value())    \
            {   \
                BOOST_PP_REPEAT(BOOST_PP_TUPLE_SIZE(elements), CHT_PP_EXT_ENUM_ADD_MEMBER_CASE, elements)    \
            default:    \
                base::naked_dump(ostr); \
            }   \
        }   \
        void naked_dump( ::std::wostream &ostr) const  \
        {   \
            switch (get_value())    \
            {   \
                BOOST_PP_REPEAT(BOOST_PP_TUPLE_SIZE(elements), CHT_PP_EXT_ENUM_ADD_WMEMBER_CASE, elements)    \
            default:    \
                base::naked_dump(ostr); \
            }   \
        }   \
        name(int value, no_check_arg nca) noexcept: base(value, nca) {}  \
    public:     \
        name(int value = 0L):   \
            base(value, no_check_arg())    \
        {   \
            check_bounds<name>(value);    \
        }  \
        template <typename Other> name(Other const &other,   \
                                        typename ::std::enable_if_t<    \
                                        ::cmn::meta::is_related_v<Other, name>, enabler> = enabler{}    \
                                       ):  \
            base(other.get_value(), no_check_arg())  \
        {   \
            check_bounds<name>(other.get_value());  \
        }   \
        CHT_PP_EXT_ENUM_DEFINE_ARITHMETIC(name)   \
        \
        typedef ::std::integral_constant<underlying_t,   \
            static_cast<underlying_t>(BOOST_PP_CAT(base, ::self_end_t::value))> self_begin_t;    \
        typedef ::std::integral_constant<underlying_t,   \
            static_cast<underlying_t>(BOOST_PP_CAT(cel, name))>                 self_end_t;  \
        typedef ::boost::mpl::push_back<base::parents_t, name>::type            parents_t;  \
        typedef ::boost::mpl::deref<    \
            ::boost::mpl::begin<parents_t>::type>::type                         first_parent;  \
        typedef ::std::integral_constant< \
            underlying_t,   \
            static_cast<underlying_t>(first_parent::self_begin_t::value)>       begin_t;    \
        typedef self_end_t                                                      end_t;  \
        \
        BOOST_PP_REPEAT(BOOST_PP_TUPLE_SIZE(elements), CHT_PP_EXT_ENUM_ADD_STATIC_MEMBER, \
                        CHT_PP_ENUM_MAKE_QELEMS(name, elements))    \
    };  \
    \
    BOOST_PP_REPEAT(BOOST_PP_TUPLE_SIZE(elements), CHT_PP_EXT_ENUM_ADD_STATIC_MEMBER_INIT, \
                    CHT_PP_ENUM_MAKE_QELEMS(name, elements))    \
    CHT_PP_EXT_ENUM_DEFINE_OSTREAM(name)

