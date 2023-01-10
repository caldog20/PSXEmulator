#pragma once

#include "utils.hpp"
#include "BitField.hpp"


union Instruction {
    u32 code;

    BitField<26, 6, u32> opcode;

    union {
        BitField<21, 5, u32> rs;
        BitField<16, 5, u32> rt;
        BitField<11, 5, u32> rd;
    };

    union {
        BitField<0, 16, u32> imm;
    };

    union {
        BitField<0, 16, s32> immse;
    };

    union {
        BitField<0, 26, u32> tar;
    };


    BitField<16, 1, u32> bgez;
    BitField<20, 1, u32> b_link;

    BitField<6, 5, u32> sa;
    BitField<0, 6, u32> fn;

    void operator=(const u32 value) {
        code = value;
    }

};
