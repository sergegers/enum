// enum.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include <fstream>

#include "enum.h"
#include "enum_class.h"
#include "manip.h"

using namespace std;

PP_DEFINE_ENUM
(
    simle_enum,
    (
        se00,
        se01,
        se02
    )
);

enum simple_enum_1
{
    se10,
    se11,
    se12
};

PP_ADAPT_ENUM
(
    simple_enum_1,
    (
        se10,
        se11,
        se12
    )
);

enum bitfield
{
    bf1 = 0x1,
    bf2 = 0x2,
    bf4 = 0x4,
    bf8 = 0x8,
    bf10 = 0x10
};

PP_ADAPT_BITFIELD
(
    bitfield,
    (
        bf1,
        bf2,
        bf4,
        bf8,
        bf10
    )
);

enum enum_test
{
    one = 0x0,
    two = 0x1,
    three = 0x2,
    digit_mask = 0x3,
    red = 0x0,
    green = 0x4,
    blue = 0x8,
    color_mask = 0xC
};

PP_ADAPT_COMPLEX_ENUM
(
    enum_test,
    (
        digit_mask, (one, two, three),
        color_mask, (red, green, blue)
    )
);

PP_DEFINE_ENUM_CLASS
(
    class_enum,
    (
        ce00,
        ce01,
        ce02,
        ce03
    )
);

enum class class_enum_1
{
    ce10,
    ce11,
    ce12,
    ce13
};

PP_ADAPT_ENUM_CLASS
(
    class_enum_1,
    (
        ce10,
        ce11,
        ce12,
        ce13
    )
);

//
// EFlags
//
#define EFLAGS_CF               0x01L
#define EFLAGS_ZF               0x40L
#define EFLAGS_TF               0x100L
#define EFLAGS_INTERRUPT_MASK   0x200L
#define EFLAGS_DF               0x400L
#define EFLAGS_NESTED_TASK      0x4000L
#define EFLAGS_V86_MASK         0x20000
#define EFLAGS_ALIGN_CHECK      0x40000
#define EFLAGS_VIF              0x80000
#define EFLAGS_VIP              0x100000
#define EFLAGS_USER_SANITIZE    0x3F4DD7
#define EFLAG_SIGN              0x8000
#define EFLAG_ZERO              0x4000

enum class EFLAGS
{
    EMPTY                   = 0x0,        
    CF                      = EFLAGS_CF,	// 0x1
    PF				        = (1 << 2),		// 0x4
    AF				        = (1 << 4),		// 0x10
    ZF                      = EFLAGS_ZF,	// 0x40
    SF				        = (1 << 7),		// 0x80
    TF                      = EFLAGS_TF,	// 0x100
    IF				        = EFLAGS_INTERRUPT_MASK,	// 0x200
    DF                      = EFLAGS_DF,	// 0x400
    OF				        = (1 << 11),	// 0x800
    IOPL_MASK		        = (0b11 << 12), // 0x3000
    IOPL_0			        = (0b0 << 12),	 // 0x0
    IOPL_1			        = (0b1 << 12),	 // 0x1000
    IOPL_2			        = (0b10 << 12), // 0x2000
    IOPL_3			        = (0b11 << 12), // 0x3000
    NT				        = EFLAGS_NESTED_TASK,	// 0x4000
    RF				        = (1 << 16),	// 0x10000
    VM				        = EFLAGS_V86_MASK,		// 0x20000
    AC				        = EFLAGS_ALIGN_CHECK,	// 0x40000
    VIF                     = EFLAGS_VIF,	// 0x80000
    VIP                     = EFLAGS_VIP,	// 0x100000
    ID				        = (1 << 21)		// 0x200000
};

PP_ADAPT_COMPLEX_ENUM_CLASS
(
	EFLAGS,
	(
		CF, (CF),
		PF, (PF),
		AF, (AF),
		ZF, (ZF),
		SF, (SF),
		TF, (TF),
		IF, (IF),
		DF, (DF),
		OF, (OF),
		IOPL_MASK,
		(
			IOPL_0,
			IOPL_1,
			IOPL_2,
			IOPL_3
		),
		NT, (NT),
		RF, (RF),
		VM, (VM),
		AC, (AC),
		VIF, (VIF),
		VIP, (VIP),
		ID, (ID)
	)
);

int main()
{
    //ofstream out;
    //out.exceptions(fstream::badbit | fstream::failbit);
    //out.open("c:\\tmp\\output.txt");

    auto &out = cout;

    out << se01 << endl;
    out << (simple_enum_1)(se11 + 4) << endl;
    out << (bf2 | bf4 | bf10) << endl;
    out << (one | green) << endl;
    out << class_enum::ce00 << endl;
    out << class_enum_1::ce13 << endl;
    out << (EFLAGS::CF | EFLAGS::IOPL_0 | EFLAGS::NT | EFLAGS::IF) << endl;
    out << bitfield_delim(" | ") << (EFLAGS::CF | EFLAGS::IOPL_1 | EFLAGS::NT | EFLAGS::IF | EFLAGS::VIF) << endl;
    out << bitfield_delim<char>() << (EFLAGS::CF | EFLAGS::IOPL_1 | EFLAGS::NT | EFLAGS::IF | EFLAGS::VIF) << endl;

    return 0;
}

