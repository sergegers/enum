#pragma once

#include <iosfwd>
#include <string>

//#include <common/shared/range/adaptors.h>

CHT_PUSH_WARNINGS()

// boost.io
#include <boost/io/ios_state.hpp>

CHT_POP_WARNINGS()

#include "manip.h"
#include "op.h"

#define CHT_HEX_OUT()   ::std::hex << ::std::showbase << ::std::uppercase

namespace cmn::enum_::io
{

namespace detail
{

template <typename Enum, typename Char, typename CharTraits>
class print_record
{
private:
    std::basic_ostream<Char, CharTraits>    &m_ostr;
    bool                                    &m_first_time;

public:
    print_record
    (
        std::basic_ostream<Char, CharTraits>& ostr,
        bool &first_time
    )
        : m_ostr{ ostr }, m_first_time{ first_time } {}

    template <typename Record>
    void operator ()(Record const &rec) const
    {
        auto const delim_ = cmn::io::basic_bitfield_delim_t<Char, CharTraits>::get_value(m_ostr);
        if (m_first_time) m_first_time = false; else m_ostr << delim_;

        using utils_type = utils<Enum>;
        m_ostr << utils_type::str::template get<Char>(rec);
    }
};

template <typename Tail, typename Char, typename CharTraits>
void print_tail(Tail tail, std::basic_ostream<Char, CharTraits> &ostr)
{
    if (tail)
    {
        boost::io::ios_flags_saver const ifs{ ostr };
        auto const delim_ = cmn::io::basic_bitfield_delim_t<Char, CharTraits>::get_value(ostr);

        ostr << CHT_HEX_OUT() << delim_ << tail;
    }
}

} // namespace detail
                              
template <typename Enum, typename Char, typename CharTraits>
auto print
(
    std::basic_ostream<Char, CharTraits> &ostr, 
    op::fwd<Enum> const &f,
    kkind_t<kind_t::combo>
)->
decltype(ostr)
{
    using utils_type = utils<Enum>;
    using print_record_type = detail::print_record<Enum, Char, CharTraits>;
    using underlying_type = typename utils_type::underlying_type;

    auto const open_ = cmn::io::basic_enum_open_t<Char, CharTraits>::get_value(ostr);
    auto const close_ = cmn::io::basic_enum_close_t<Char, CharTraits>::get_value(ostr);
    auto const mask_ = utils_type::to_mask_val(cmn::io::bitfield_mask_t<Enum>::get_value(ostr));

    ostr << open_;

    static auto groups = get_groups(Enum{});
    static auto masks = utils_type::calc_masks(groups);

    bool first_time = true;
    auto const remain = utils_type::process_on_groups
    (
        groups,
        masks,
        f.to_mask_val() & mask_,
        print_record_type{ ostr, first_time },
        mask_
    );

    detail::print_tail(remain, ostr);

    return  ostr << close_;
}                  

template <typename Enum, typename Char, typename CharTraits>
auto print
(
    std::basic_ostream<Char, CharTraits> &ostr, 
    op::fwd<Enum> const &f,
    kkind_t<kind_t::enum_>
)->
decltype(ostr)
{
    using utils_type = utils<Enum>;
    using print_record_type = detail::print_record<Enum, Char, CharTraits>;

    auto const open_ = cmn::io::basic_enum_open_t<Char, CharTraits>::get_value(ostr);
    auto const close_ = cmn::io::basic_enum_close_t<Char, CharTraits>::get_value(ostr);

    ostr << open_;

    static auto groups = get_groups(Enum{});
    static_assert
    (
        std::tuple_size_v<decltype(groups)> == 1,
        "Enum must have the one and only one group"
    );

    static decltype(auto) group = std::get<0>(groups);

    bool first_time = true;
    auto const remain = utils_type::process_on_group
    (
        group, f.to_mask_val(), print_record_type{ ostr, first_time }
    );
    detail::print_tail(remain, ostr);

    return ostr << close_;
}
                                 
template <typename Enum, typename Char, typename CharTraits>
auto print
(
    std::basic_ostream<Char, CharTraits> &ostr, 
    op::fwd<Enum> const &f,
    kkind_t<kind_t::bitfield>
)->
decltype(ostr)
{
    using stream_ref_type = std::basic_ostream<Char, CharTraits> &;
    using utils_type = utils<Enum>;
    using print_record_type = detail::print_record<Enum, Char, CharTraits>;

    auto const open_ = cmn::io::basic_enum_open_t<Char, CharTraits>::get_value(ostr);
    auto const close_ = cmn::io::basic_enum_close_t<Char, CharTraits>::get_value(ostr);
    auto const mask_ = utils_type::to_mask_val(cmn::io::bitfield_mask_t<Enum>::get_value(ostr));

    ostr << open_;

    static auto groups = get_groups(Enum{});
    bool first_time = true;
    auto const remain = boost::fusion::fold
    (
        groups,
        f.to_mask_val() & mask_,
        [&ostr, &first_time](auto val, auto const &group)
        {
            static_assert
            (
                std::tuple_size_v<std::decay_t<decltype(group)>> == 1, 
                "Bitfield group must contain the one and only one record"
            );
            decltype(auto) rec = group[0];
            
            if (utils_type::to_mask_val(rec.m_val) & val)
                print_record_type{ ostr, first_time}(rec);

            return val & ~utils_type::to_mask_val(rec.m_val);
        }
    );
    detail::print_tail(remain, ostr);

    return ostr << close_;
}      

}   // namespace cmn::enum_::io
