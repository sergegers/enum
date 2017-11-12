#pragma once

#include <common/shared/io/slot_manip.h>

namespace cmn::io
{

////////////////////////////////////////////////////////////////////////////////
// Manipulator bitfield_mask_t could be used with custom enums
////////////////////////////////////////////////////////////////////////////////
namespace detail
{

template <typename Enum>
constexpr long const def_bitfield_mask_init = std::numeric_limits<unsigned long>::max();
	
} // namespace detail

template <typename Enum>
using bitfield_mask_t =
    basic_int_slot_manip_impl
    <
        struct bitfield_mask_,
        Enum,
        detail::def_bitfield_mask_init<Enum>
    >
;

constexpr detail::int_slot_manip_forwarder<bitfield_mask_t> const bitfield_mask {};

////////////////////////////////////////////////////////////////////////////////
// enum_open manipulator
////////////////////////////////////////////////////////////////////////////////
namespace detail
{

template <typename Char>
constexpr long const def_enum_open_init =
    string_decoder::decode_slot(meta::symbols<Char>::open_square_bracket());

template <typename Char>
constexpr long const def_enum_open =
    string_decoder::decode_slot(meta::symbols<Char>::nothing());
	
} // namespace detail

template <typename Char, typename CharTraits = std::char_traits<Char> >
using basic_enum_open_t = 
    basic_string_slot_manip_impl
    <
        struct basic_enum_open_,
        Char,
        CharTraits,
        detail::def_enum_open_init<Char>,
        detail::def_enum_open<Char>
    >
;

using enum_open_t = basic_enum_open_t<char>;
using wenum_open_t = basic_enum_open_t<wchar_t>;

constexpr detail::string_slot_manip_forwarder<basic_enum_open_t> const enum_open {};

////////////////////////////////////////////////////////////////////////////////
// enum_close manipulator
////////////////////////////////////////////////////////////////////////////////
namespace detail
{

template <typename Char>
constexpr long const def_enum_close_init =
    string_decoder::decode_slot(cmn::meta::symbols<Char>::close_square_bracket());

template <typename Char>
constexpr long const def_enum_close =
    string_decoder::decode_slot(cmn::meta::symbols<Char>::nothing());

} // namespace detail

template <typename Char, typename CharTraits = std::char_traits<Char> >
using basic_enum_close_t =
    basic_string_slot_manip_impl
    <
        struct basic_enum_close_,
        Char,
        CharTraits,
        detail::def_enum_close_init<Char>,
        detail::def_enum_close<Char>
    >
;

using enum_close_t = basic_enum_close_t<char>;
using wenum_close_t = basic_enum_close_t<wchar_t>;

constexpr detail::string_slot_manip_forwarder<basic_enum_close_t> const enum_close {};

////////////////////////////////////////////////////////////////////////////////
// Manipulator basic_bitfield_delim_t for customizing delimiter between
// bitfield elements during output.
// Default value is " "
////////////////////////////////////////////////////////////////////////////////
namespace detail
{

template <typename Char>
constexpr long const def_bitfield_delim_init = string_decoder::decode_slot(meta::symbols<Char>::space());
template <typename Char>
constexpr long const def_bitfield_delim = string_decoder::decode_slot(meta::symbols<Char>::nothing());
	
} // namespace detail

template <typename Char, typename CharTraits = std::char_traits<Char> >
using basic_bitfield_delim_t =
    basic_string_slot_manip_impl
    <
        struct basic_bitfield_delim_,
        Char,
        CharTraits,
        detail::def_bitfield_delim_init<Char>,
        detail::def_bitfield_delim<Char>
    >
;
using bitfield_delim_t = basic_bitfield_delim_t<char>;
using wbitfield_delim_t = basic_bitfield_delim_t<wchar_t>;

constexpr detail::string_slot_manip_forwarder<basic_bitfield_delim_t> const bitfield_delim {};

}   // namespace cmn::io

