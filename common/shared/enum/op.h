#pragma once

#include <iosfwd>

#include "utils.h"

namespace cmn::enum_::op
{

template <typename Enum>
using to_mask_t = typename utils<Enum>::underlying_type;

template <typename Enum>
struct fwd
{
    using enum_type = std::decay_t<Enum>;
    using utils_type = utils<enum_type>;
    using mask_type = typename utils_type::underlying_type;

    Enum m_val;

    constexpr fwd(Enum val): m_val{ std::forward<Enum>(val) } {}
    constexpr auto to_mask() const { return utils_type::template to_mask<Enum>(m_val); }
    auto &to_mask() { return utils_type::template to_mask<Enum>(m_val); }
    constexpr auto to_mask_val() const { return utils_type::to_mask_val(m_val); }
};

// NOTE: no need to full qualify names because the macros will be used localy

#define CHT_PP_ENUM_CONSTEXPR_BINARY_OP(op_)  \
    template <typename Enum> constexpr auto operator op_ (fwd<Enum> const &lhs, fwd<Enum> const &rhs)   \
    {   \
        return static_cast<Enum>(lhs.to_mask() op_ rhs.to_mask());  \
    } 

#define CHT_PP_ENUM_CONSTEXPR_BINARY_OP_AUTO(op_)  \
    template <typename Enum> constexpr auto operator op_(fwd<Enum> const &lhs, fwd<Enum> const &rhs)   \
    {   \
        return lhs.to_mask() op_ rhs.to_mask();  \
    }

#define CHT_PP_ENUM_CONSTEXPR_UNARY_OP(op_)  \
    template <typename Enum> constexpr auto operator op_ (fwd<Enum> const &f)   \
    {   \
        return static_cast<Enum>(op_ f.to_mask());  \
    }

#define CHT_PP_ENUM_MUTABLE_PREFIX_UNARY_OP(op_)  \
    template <typename Enum> auto operator op_ (fwd<Enum &> f)   \
    {   \
        return static_cast<Enum>(op_ f.to_mask());  \
    }

#define CHT_PP_ENUM_MUTABLE_POSTFIX_UNARY_OP(op_)  \
    template <typename Enum> auto operator op_ (fwd<Enum &> f, int)   \
    {   \
        auto const tmp = f.m_val;   \
        return f.m_val = static_cast<Enum>(op_ f.to_mask()), tmp;  \
    }

#define CHT_PP_ENUM_COMPOUND_OP(op_)  \
    template <typename Enum> auto operator op_(fwd<Enum &> lhs, fwd<Enum> const &rhs)   \
    {   \
        return (lhs.to_mask() op_ rhs.to_mask()), lhs.m_val;  \
    }

// bitwise
CHT_PP_ENUM_CONSTEXPR_BINARY_OP(|)
CHT_PP_ENUM_CONSTEXPR_BINARY_OP(&)
CHT_PP_ENUM_CONSTEXPR_BINARY_OP(^)
CHT_PP_ENUM_CONSTEXPR_UNARY_OP(~)
CHT_PP_ENUM_COMPOUND_OP(|=)
CHT_PP_ENUM_COMPOUND_OP(&=)
CHT_PP_ENUM_COMPOUND_OP(^=)

// unit steppable
CHT_PP_ENUM_MUTABLE_PREFIX_UNARY_OP(++)
CHT_PP_ENUM_MUTABLE_POSTFIX_UNARY_OP(++)
CHT_PP_ENUM_MUTABLE_PREFIX_UNARY_OP(--)
CHT_PP_ENUM_MUTABLE_POSTFIX_UNARY_OP(--)

// ariphmetic
CHT_PP_ENUM_CONSTEXPR_BINARY_OP(+)
CHT_PP_ENUM_CONSTEXPR_BINARY_OP(-)

// comparable
CHT_PP_ENUM_CONSTEXPR_BINARY_OP_AUTO(==)
CHT_PP_ENUM_CONSTEXPR_BINARY_OP_AUTO(!=)
CHT_PP_ENUM_CONSTEXPR_BINARY_OP_AUTO(>)
CHT_PP_ENUM_CONSTEXPR_BINARY_OP_AUTO(>=)
CHT_PP_ENUM_CONSTEXPR_BINARY_OP_AUTO(<)
CHT_PP_ENUM_CONSTEXPR_BINARY_OP_AUTO(<=)

#undef CHT_PP_ENUM_CONSTEXPR_BINARY_OP
#undef CHT_PP_ENUM_CONSTEXPR_BINARY_OP_AUTO
#undef CHT_PP_ENUM_CONSTEXPR_UNARY_OP
#undef CHT_PP_ENUM_MUTABLE_PREFIX_UNARY_OP
#undef CHT_PP_ENUM_MUTABLE_POSTFIX_UNARY_OP
#undef CHT_PP_ENUM_COMPOUND_OP

}   //namespace cmn::enum_::op

namespace cmn::enum_::io
{

template<typename CharType, typename CharTraits, typename Enum, kind_t Kind_>
auto print
(
    std::basic_ostream<CharType, CharTraits> &ostr, 
    cmn::enum_::op::fwd<Enum>, 
    kkind_t<Kind_>
)-> 
    decltype(ostr)
{
    static_assert
    (
        sizeof(CharType) == 0,
        "If you want to output enum, include header <common/shared/enum/io.h>"
    );
    return ostr;
}

}   // namespace cmn::enum_::io
