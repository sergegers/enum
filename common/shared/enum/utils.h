#pragma once

#include <tuple>
#include <array>
#include <limits>
#include <string_view>

CHT_PUSH_WARNINGS()

// boost.mpl
#include <boost/mpl/identity.hpp>
// boost.fusion
// ReSharper disable CppUnusedIncludeDirective
#include <boost/fusion/container/vector.hpp>
// ReSharper restore CppUnusedIncludeDirective
#include <boost/fusion/algorithm/iteration/fold.hpp>
#include <boost/fusion/view/zip_view.hpp>
#include <boost/fusion/adapted/std_array.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
// boost.range
#include <boost/range/algorithm/lower_bound.hpp>

CHT_POP_WARNINGS()

namespace cmn::enum_
{

enum class kind_t
{
    noenum,
    enum_,
    bitfield,
    combo
};

template <kind_t Kind_>
using kkind_t = std::integral_constant<kind_t, Kind_>;

template <typename Enum> constexpr auto get_kind(Enum) { return kkind_t<kind_t::noenum>{}; }

template <typename Enum> constexpr kind_t kind_v = get_kind(Enum{}).value;

template <typename Enum>
class utils
{
public:
    using underlying_type = std::underlying_type_t<Enum>;

    struct record_type { Enum m_val; std::string_view m_str; std::wstring_view m_wstr; };

    template <std::size_t Size_>
    using group_t = std::array<record_type, Size_>;

    template <std::size_t... Sizes_>
    using groups_t = std::tuple<group_t<Sizes_>...>;

    template <std::size_t I_> using int_ = std::integral_constant<std::size_t, I_>;

    struct str
    {
        template <typename Char> static constexpr auto get(record_type const &record);

        template <> constexpr static auto get<char>(record_type const &record)
        { return record.m_str; }

        template <> constexpr static auto get<wchar_t>(record_type const &record)
        { return record.m_wstr; }
    };

private:
    template <typename E>
    struct to_mask_impl;

    template <>
    struct to_mask_impl<Enum>
    {
        constexpr auto operator()(Enum e) const { return static_cast<underlying_type>(e); }
    };

    template <>
    struct to_mask_impl<Enum &>
    {
        constexpr auto &operator()(Enum &e) const { return reinterpret_cast<underlying_type &>(e); }
    };
public:
    template <typename E>
    static constexpr decltype(auto) to_mask(typename boost::mpl::identity<E>::type e)
    {
        return to_mask_impl<E>{}(e);
    }

    static constexpr auto to_mask_val(Enum e) { return to_mask_impl<Enum>{}(e); }

    template <std::size_t Size_, std::size_t Idx_ = Size_ - 1>
    struct calc_mask
    {
        static_assert(Size_ > 0, "No empty groups, pls!");

        group_t < Size_> const &m_group;

        constexpr calc_mask(group_t < Size_> const &group) : m_group{ group } {}

        constexpr auto operator()() const -> underlying_type
        {
            return to_mask_val(m_group[Idx_].m_val) | calc_mask<Size_, Idx_ - 1>{ m_group }();
        }
    };

    template <std::size_t Size_>
    struct calc_mask<Size_, 0>
    {
        static_assert(Size_ > 0, "No empty groups, pls!");

        group_t < Size_> const &m_group;

        constexpr calc_mask(group_t < Size_> const &group) : m_group{ group } {}

        constexpr auto operator()() const -> underlying_type
        {
            return to_mask_val(m_group[0].m_val);
        }
    };

    template <std::size_t... Sizes_>
    struct calc_masks_impl
    {
        template <std::size_t... Indices_>
        constexpr auto operator ()
        (
            groups_t<Sizes_...> const &groups,
            std::index_sequence<Indices_...>
        ) const ->
            std::array<underlying_type, sizeof... (Sizes_)> 
        {
            return { calc_mask<Sizes_>{ std::get<Indices_>(groups) }()... };
        }
    };
public:
    template <std::size_t... Sizes_>
    // BUG: VS 2017.4 - couldn't use typedefs here
    // static constexpr auto calc_masks(groups_t<Sizes_...> const &groups)->
    static constexpr auto calc_masks(std::tuple<std::array<record_type, Sizes_>...> const &groups)->
        std::array<underlying_type, sizeof... (Sizes_)>
    {
        return calc_masks_impl<Sizes_...>{}
        (
             groups,
             std::make_index_sequence <sizeof... (Sizes_)>{}
        );
    }
private:
    template <std::size_t Idx_>
    static constexpr auto mask_overlap_impl(underlying_type const *masks, int_<Idx_>)
    {
        return masks[Idx_] & mask_overlap_impl(masks, int_<Idx_ - 1>{});
    }

    static constexpr auto mask_overlap_impl(underlying_type const *masks, int_<0>)
    {
        return masks[0];
    }
public:
    template <std::size_t Size_>
    static constexpr auto mask_overlap(std::array<underlying_type, Size_> const &masks)
    {
        static_assert(Size_ > 0, "No empty groups, pls!");
        return mask_overlap_impl(masks.data(), int_<Size_ - 1>{});
    }     

    ///////////////////////////////////////////////////////////////////////////
    template <typename Group, typename Op>
    static auto process_on_group(
        Group const &group, underlying_type masked_en, Op const &op) ->underlying_type
    {
        auto const it = boost::lower_bound
        (
         group, masked_en, [](auto const &lhs, auto rhs) { return to_mask_val(lhs.m_val) < rhs; }
        );
        return boost::end(group) == it || to_mask_val(it->m_val) > masked_en?
                   masked_en :
                   (op(*it), masked_en & ~to_mask_val(it->m_val))
            ;
    }

    static constexpr auto def_mask = 
        std::numeric_limits<std::make_unsigned_t<underlying_type>>::max();

    template <typename Op, typename... Groups>
    static auto process_on_groups
    (
        std::tuple<Groups...> const &groups, 
        std::array<underlying_type, sizeof... (Groups)> const &masks,
        underlying_type en, 
        Op const &op,
        underlying_type mask_2 = def_mask
        
    ) ->underlying_type
    {
        using boost::fusion::vector;
        using boost::fusion::zip_view;

        using sequences_type = vector<decltype(groups) &, decltype(masks) &>;
        return boost::fusion::fold
        (
            zip_view<sequences_type>{ sequences_type{ groups, masks } },
            en & mask_2,
            [&op, mask_2](underlying_type val, auto const &group_mask)
            {
                using boost::fusion::at_c;

                auto const mask = at_c<1>(group_mask) & mask_2;
                return mask?
                    static_cast<underlying_type>
                    (
                        val & ~mask | 
                        process_on_group(at_c<0>(group_mask), mask & val, op)
                    ): 
                    val;
            }
        );
    }
private:
    template <std::size_t Size_, std::size_t Index_, std::size_t... Indices_>
    static constexpr int get_index_impl(group_t<Size_> const &group, Enum en,
        std::index_sequence<Index_, Indices_...>)
    {
        return group[Index_].m_val == en ? 
            Index_ : 
            get_index_impl(group, en, std::index_sequence<Indices_...>{})
        ;
    }

    template <std::size_t Size_>
    static constexpr int get_index_impl(group_t<Size_> const &group, Enum en, std::index_sequence<>)
    {
        return -1;
    }
public:
    template <std::size_t Size_>
    static constexpr int get_index(group_t<Size_> const &group, Enum en)
    {
        static_assert(Size_ > 0, "No empty groups!");

        return get_index_impl(group, en, std::make_index_sequence<Size_>{});
    }
};

}   // namespace cmn::enum_
