#pragma once

#include <string_view>

#include "utils.h"

namespace cmn::enum_
{

////////////////////////////////////////////////////////////////////////////////////////////////
template <typename Enum>
struct traits
{
    using underlying_type = std::underlying_type_t<Enum>;
    using utils_type = utils<Enum>;
    static constexpr kind_t kind = kind_v<Enum>;
    static constexpr std::string_view name = get_name(Enum{});
    static constexpr std::wstring_view wname = get_wname(Enum{});
    static constexpr auto groups = get_groups(Enum{});

    // can calc in constexpr
    template <std::size_t Size_>
    static constexpr auto get_str(typename utils_type::template group_t<Size_> const &group, Enum en)
    {
        auto const idx = utils_type::get_index(group, en);
        decltype(auto) rec = group[idx];
        return utils<Enum>::str::template get<char>(rec);
    }
};

} // namespace cmn::enum_

namespace cmn
{

// shortcut
template <typename Enum> using enum_traits = enum_::traits<Enum>;

} // namespace cmn

