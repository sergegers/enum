#pragma once

#include <iosfwd>
#include <string_view>

#include <common/shared/meta/type_traits.h>     // for meta::symbols<>, meta::is_comparable<>

namespace cmn::io
{

////////////////////////////////////////////////////////////////////////////////
// Basic slot manipulator
////////////////////////////////////////////////////////////////////////////////
namespace detail
{

////////////////////////////////////////////////////////////////////////////////
// Stream slot storage
// template parameter Tag added to make an unique slot for every manipulator
////////////////////////////////////////////////////////////////////////////////
template <typename Tag>
class stream_slot_storage
{
public:
    using out_type = long;
public:
    static int get_xalloc()
    {
        // call xalloc once to get an index at which we can store data for this
        // manipulator.
        static int const idx = std::ios_base::xalloc();
        return idx;
    }
    static auto get_value(std::ios_base const &ios_)->  out_type
    {
        return const_cast<std::ios_base &>(ios_).iword(get_xalloc());
    }

    static void apply(std::ios_base &ios_, out_type value)
    {
        // set mask
        ios_.iword(get_xalloc()) = value;
    }
};

////////////////////////////////////////////////////////////////////////////////
// Decoder template parameters:
// InType - input type
// DefaultInit_ - the initial manipulator value set before manipulator applying
// Decoder methods:
// auto decode(in_type)->out_type
// auto encode(out_type)->in_type
////////////////////////////////////////////////////////////////////////////////

static constexpr long k_impossible_value = 0xFFFFFFFF;

namespace string_decoder
{

template <typename Char>
constexpr std::size_t slot_room() { return sizeof(long) / sizeof(Char); }

template <typename Char, typename CharTraits = std::char_traits<Char>>
auto encode_slot(long decoded)
{
    using result_type = std::basic_string<Char, CharTraits>;

    // for little endian
    return
        reinterpret_cast<Char const *>(&decoded)[slot_room<Char>() - 1] == 0?
            result_type { reinterpret_cast<Char const *>(&decoded) }:
            result_type { reinterpret_cast<Char const *>(&decoded), slot_room<Char>() }
    ;
}

// endian independent implementation, 
// also it keeps function constexpr
constexpr long decode_slot(char const *delim, std::size_t N)
{
    // N counts the trailing zero
    using shift = std::integral_constant<unsigned char, sizeof(char) * 8>;
    return
        N == 1 ? 0 :
        N == 2 ? delim[0] :
        N == 3 ? delim[0] | delim[1] << shift::value :
        N == 4 ? delim[0] | delim[1] << shift::value | delim[2] << shift::value * 2 :
        N == 5 ? delim[0] | delim[1] << shift::value | delim[2] << shift::value * 2 | delim[3] << shift::value * 3 :
        k_impossible_value;
}

constexpr long decode_slot(wchar_t const *delim, std::size_t N)
{
    // N counts the trailing zero
    using shift = std::integral_constant<unsigned char, sizeof(wchar_t) * 8>;
    return
        N == 1 ? 0 :
        N == 2 ? delim[0] :
        N == 3 ? delim[0] | delim[1] << shift::value : k_impossible_value;
}

template <typename Char, std::size_t N_>
constexpr long decode_slot(Char const (&delim)[N_])
{
    static_assert(slot_room<Char>() >= N_ - 1, "Delimiter is too big.");
    return decode_slot(delim, N_);
}
	
} // namespace string_decoder

template <typename String, long DefaultInit_>
class slot_string_decoder;

template <typename Char, typename CharTraits, long DefaultInit_>
class slot_string_decoder<std::basic_string<Char, CharTraits>, DefaultInit_>
{
public:
    using in_type = std::basic_string<Char, CharTraits>;
    using out_type = long;
    // exchange default_init_val & real_default_init_val during decode/encode
    // operations
    static constexpr out_type const real_default_init_val = 0L;
    static constexpr out_type const default_init_val = DefaultInit_;

    static_assert(string_decoder::slot_room<Char>() >= 1, "Type is too big.");
private:
    
    in_type const  m_real_default_slot_init;
    in_type const  m_default_slot_init;

    auto decode_impl(in_type in, std::false_type) const
    {
        return 
            in == m_real_default_slot_init?
                default_init_val:
                in == m_default_slot_init?
                    real_default_init_val:
                    string_decoder::decode_slot(in.c_str(), in.length() + 1);
    }

    auto decode_impl(in_type in, std::true_type) const
    {
        return string_decoder::decode_slot(in.c_str(), in.length() + 1);
    }

    auto encode_impl(out_type out, std::false_type) const
    {
        switch (out)
        {
            case real_default_init_val: return m_default_slot_init;
            case default_init_val: return m_real_default_slot_init;
            default: return string_decoder::encode_slot<Char, CharTraits>(out);
        }
    }

    auto encode_impl(out_type out, std::true_type) const
    {
        return string_decoder::encode_slot<Char, CharTraits>(out);
    }
public:
    slot_string_decoder():
        m_real_default_slot_init { string_decoder::encode_slot<Char, CharTraits>(real_default_init_val) },
        m_default_slot_init { string_decoder::encode_slot<Char, CharTraits>(default_init_val) }
    {}

    auto decode(in_type in) const -> out_type
    {
        return decode_impl(in, std::bool_constant < real_default_init_val == default_init_val >{});
    }

    auto encode(out_type out) const -> in_type
    {
        return encode_impl(out, std::bool_constant<real_default_init_val == default_init_val>{});
    }
};

namespace int_decoder
{

template <typename InType>
constexpr long decode_slot(InType in) { return static_cast<long>(in); }

template <typename InType>
constexpr InType encode_slot(long out) 
{ 
#pragma warning (push)
#pragma warning (disable: 4800)
    return static_cast<InType>(out);
#pragma warning (pop)
}

} // namespace int_decoder

template <typename Int, long DefaultInit_>
class slot_int_decoder
{
public:
    using in_type = Int;
    using out_type = long;
    // exchange default_init_val & real_default_init_val during decode/encode
    // operations
    static constexpr out_type const real_default_init_val = 0L;
    static constexpr out_type const default_init_val = DefaultInit_;

    static_assert(std::is_pod_v<in_type>, "Type is not supported.");
    static_assert(sizeof(in_type) <= sizeof(out_type), "Type is too big.");
private:

    in_type const  m_real_default_slot_init;
    in_type const  m_default_slot_init;

    auto decode_impl(in_type in, std::false_type) const
    {
        return
            in == m_real_default_slot_init?
                default_init_val:
                in == m_default_slot_init?
                    real_default_init_val:
                    int_decoder::decode_slot(in);
    }

    auto decode_impl(in_type in, std::true_type) const
    {
        return int_decoder::decode_slot(in);
    }

    auto encode_impl(out_type out, std::false_type) const
    {
        switch (out)
        {
            case real_default_init_val: return m_default_slot_init;
            case default_init_val: return m_real_default_slot_init;
            default: return int_decoder::encode_slot<in_type>(out);
        }
    }

    auto encode_impl(out_type out, std::true_type) const
    {
        return int_decoder::encode_slot<in_type>(out);
    }
public:
    slot_int_decoder(): 
        m_real_default_slot_init { int_decoder::encode_slot<in_type>(real_default_init_val) },
        m_default_slot_init { int_decoder::encode_slot<in_type>(default_init_val) } 
    {}

    auto decode(in_type in) const-> out_type
    {
        return decode_impl(in, std::bool_constant<real_default_init_val == default_init_val>{});
    }

    auto encode(out_type out) const -> in_type
    {
        return encode_impl(out, std::bool_constant<real_default_init_val == default_init_val>{});
    }
};

}   // namespace detail

////////////////////////////////////////////////////////////////////////////////
// make base manipulator class tagged to provide the unique stream slots 
// for the different classes
// Base manipulator template parameters:
// Tag - unique tag
// InType - see decoder
// DefaultInit_ - see decoder
// Default_ - the manipulator value set by default constructor
// DecoderT - decoder template
////////////////////////////////////////////////////////////////////////////////
template
<
    typename Tag,
    typename InType,
    long DefaultInit_,
    long Default_,
    template <typename, long> typename DecoderT
>
class basic_slot_manip_impl
{
private:
    static_assert(meta::is_comparable<InType>::value, "Could be comparable");

    using itself = basic_slot_manip_impl<Tag, InType, DefaultInit_, Default_, DecoderT>;
public:
    using decoder_type = DecoderT<InType, DefaultInit_>;
    using in_type = InType;
    using out_type = long;
    using storage_type = detail::stream_slot_storage<Tag>;

    out_type        m_value;
public:
    explicit basic_slot_manip_impl(in_type in) :
        m_value { decoder_type{}.decode(in) } {}

    constexpr basic_slot_manip_impl() :
        // perform manual decode correction
        m_value 
        { 
            Default_ == decoder_type::real_default_init_val? 
                decoder_type::default_init_val: 
                Default_ == decoder_type::default_init_val?
                    decoder_type::real_default_init_val:
                    Default_
        } 
    {}

    static auto get_xalloc() { return storage_type::get_xalloc(); }

    template <typename Char, typename CharTraits>
    static auto get_value(std::basic_ostream<Char, CharTraits> const &ostr) -> in_type
    {
        return decoder_type {}.encode(storage_type::get_value(ostr));
    }
};

template
<
    typename Tag,
    typename Char,
    typename CharTraits,
    long DefaultInit_,
    long Default_ = DefaultInit_
>
class basic_string_slot_manip_impl: public
    basic_slot_manip_impl
    <
        Tag, 
        std::basic_string<Char, CharTraits>, 
        DefaultInit_,
        Default_,
        detail::slot_string_decoder
    >
{
private:
    using inherited =
        basic_slot_manip_impl
        <
            Tag, 
            std::basic_string<Char, CharTraits>, 
            DefaultInit_,
            Default_,
            detail::slot_string_decoder
        >;
    using itself = basic_string_slot_manip_impl<Tag, Char, CharTraits, DefaultInit_, Default_>;
protected:
    using stream_type = std::basic_ostream<Char, CharTraits>;
    using storage_type = typename inherited::storage_type;
    using in_type = typename inherited::in_type;

    // hide base method
    using inherited::get_value;
public:
    constexpr basic_string_slot_manip_impl() = default;

    template <std::size_t N_>
    explicit basic_string_slot_manip_impl(Char const (&in)[N_]) : inherited{ in }
    {
        static_assert(detail::string_decoder::slot_room<Char>() >= N_, "Delimiter is too big.");
    }

    static auto get_value(stream_type const &ostr) -> in_type
    {
        return inherited::get_value(ostr);
    }

    friend decltype(auto) operator << (std::basic_ostream<Char, CharTraits> &ostr, itself const &self)
    {
        return storage_type::apply(ostr, self.m_value), ostr;
    }
};

namespace detail
{

template <template <typename, typename> typename StringManipT, typename... Args>
struct string_slot_manip_caller
{
private:
    using itself = string_slot_manip_caller<StringManipT, Args...>;

    std::tuple<Args...> m_args;

    template <typename Char, typename CharTraits>
    static auto make_obj(Args... args)
    {
        return StringManipT<Char, CharTraits>{ std::forward<Args>(args)... };
    }
public:
    template <typename... CallArgs>
    explicit string_slot_manip_caller(CallArgs &&... args) : 
        m_args { std::forward<CallArgs>(args)... } {}

    template <typename Char, typename CharTraits>
    friend decltype(auto) operator << (std::basic_ostream<Char, CharTraits> &ostr, itself &&caller_)
    {
        return ostr << std::apply(&itself::make_obj<Char, CharTraits>, std::move(caller_.m_args));
    }

    // needed for boost::unit_test::lazy_ostream<>
    template <typename Char, typename CharTraits>
    friend decltype(auto) operator << (std::basic_ostream<Char, CharTraits> &ostr, itself const &caller_)
    {
        return ostr << std::apply(&itself::make_obj<Char, CharTraits>, caller_.m_args);
    }
};

template <template <typename, typename> typename StringManipT>
struct string_slot_manip_forwarder
{
    template <typename... Args>
    auto operator ()(Args &&... args) const
    {
        return string_slot_manip_caller<StringManipT, Args...>{ std::forward<Args>(args)... };
    }
};
	
} // namespace detail

template
<
    typename Tag,
    typename Int,
    long DefaultInit_,
    long Default_ = DefaultInit_
>
class basic_int_slot_manip_impl: public
    basic_slot_manip_impl
    <
        Tag, 
        Int, 
        DefaultInit_,
        Default_,
        detail::slot_int_decoder
    >
{
private:
    using inherited = 
        basic_slot_manip_impl
        <
            Tag, 
            Int, 
            DefaultInit_,
            Default_,
            detail::slot_int_decoder
        >;
    using itself = basic_int_slot_manip_impl<Tag, Int, DefaultInit_, Default_>;
protected:
    using storage_type = typename inherited::storage_type;
public:
    using inherited::inherited;
    using int_type = Int;

    template <typename Char, typename CharTraits>
    friend decltype(auto) operator << (std::basic_ostream<Char, CharTraits> &ostr, itself const &self)
    {
        return storage_type::apply(ostr, self.m_value), ostr;
    }
};

namespace detail
{

template <typename IntManip, typename... Args>
struct int_slot_manip_caller
{
private:
    using itself = int_slot_manip_caller<IntManip, Args...>;

    std::tuple<Args...> m_args;

    static auto make_obj(Args... args)
    {
        return IntManip{ std::forward<Args>(args)... };
    }
public:
    template <typename... CallArgs>
    constexpr explicit int_slot_manip_caller(CallArgs &&... args) : 
        m_args { std::forward<CallArgs>(args)... } {}

    // boost::io::group() requires copyable manipulator
    int_slot_manip_caller(itself const &) = default;
    int_slot_manip_caller(itself &&) = default;
    itself &operator =(itself const &) = default;
    itself &operator =(itself &&) = default;

    template <typename Char, typename CharTraits>
    friend decltype(auto) operator << (std::basic_ostream<Char, CharTraits> &ostr, itself &&caller_)
    {
        return ostr << std::apply(&itself::make_obj, std::move(caller_.m_args));
    }

    // needed for boost::unit_test::lazy_ostream<>
    template <typename Char, typename CharTraits>
    friend decltype(auto) operator << (std::basic_ostream<Char, CharTraits> &ostr, itself const &caller_)
    {
        return ostr << std::apply(&itself::make_obj, caller_.m_args);
    }
};

template <template <typename> typename IntManipT>
struct int_slot_manip_forwarder
{
    template <typename Int, typename... Args>
    constexpr auto operator ()(Int val, Args &&... args) const
    {
        return int_slot_manip_caller<IntManipT<Int>, Int, Args...>{ val, std::forward<Args>(args)... };
    }

    template <typename Int>
    constexpr auto def() const
    {
        return int_slot_manip_caller<IntManipT<Int>>{};
    }
};

template <typename IntManip>
struct int_slot_manip_forwarder_2
{
    template <typename... Args>
    constexpr auto operator ()(Args &&... args) const
    {
        return int_slot_manip_caller<IntManip, Args...>{ std::forward<Args>(args)... };
    }
};

template <typename IntManip>
struct string_view_slot_manip_forwarder
{
    template <typename Char, typename CharTraits>
    constexpr auto operator ()(std::basic_string_view<Char, CharTraits> arg) const
    {
        using int_type = typename IntManip::int_type;
        return int_slot_manip_caller<IntManip, int_type>{ reinterpret_cast<int_type>(arg.data()) };
    }
};

} // namespace detail
	
} // namespace cmn::io

