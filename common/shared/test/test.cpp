
#include <common/shared/targetver.h>

#include <iostream>
#include <boost/io/ios_state.hpp>

#include <common/shared/enum/enum.h>
#include <common/shared/enum/bitfield.h>
#include <common/shared/enum/combo.h>
#include <common/shared/enum/io.h>

CHT_PP_DECLARE_ENUM
(
    en_0,
    (en0_0, 0)
    (en0_1)    // can skip value
    (en0_5, 5)
)

CHT_PP_DECLARE_ENUM_CLASS_BASE
(
    en_1, char,
    (_0)
    (_1)
    (_2)
)

struct foo
{
    CHT_PP_DECLARE_ENUM_INNER
    (
        en_2,
        (en2_0)
        (en2_4, 4)
        (en2_5)
    )
};

CHT_PP_DECLARE_BITFIELD_CLASS
(
    bf_0,
    (_1, 0x1)
    (_2, 0x2)
    (_4, 0x4)
    (_8, 0x8)
)

CHT_PP_DECLARE_COMBO
(
    c0,
    (
        (c0_1, 0x1)
        (c0_2, 0x2)
        (c0_3, 0x3)
    )
    (
        (c0_4, 0x4)
        (c0_8, 0x8)
        (c0_c, 0xC)
    )
    ,
    (c0_mask_7, 0x7)    // can add convinient enum constants
    (c0_mask_c, 0xC)
)

int main()
{
    using namespace std;
    using boost::io::ios_iword_saver;

    using namespace cmn::io;

    {
        ios_iword_saver eos { cout, enum_open_t::get_xalloc() };
        ios_iword_saver ecs { cout, enum_close_t::get_xalloc() };

        cout << en0_0 << "\n";
        cout << enum_open("<") << enum_close(">") << en_1::_1 << "\n";
        cout << foo::en2_4 << "\n";        
        cout << (c0_3 | c0_8) << "\n";
    }

    {
        ios_iword_saver bds { cout, bitfield_delim_t::get_xalloc() };
        ios_iword_saver bms { cout, bitfield_mask_t<c0>::get_xalloc() };

        cout << "'" << bitfield_delim_t::get_value(cout) << "'\n";
        cout << bitfield_delim(", ") << (bf_0::_2 | bf_0::_4) << "\n";
        cout << (c0_3 | c0_8) << "\n";
        cout << bitfield_mask(c0_mask_7) << (c0_3 | c0_8) << "\n";
        cout << bitfield_mask(c0_mask_c) << (c0_3 | c0_8) << "\n";        
    }
}