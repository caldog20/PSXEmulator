#pragma once
#include <cstddef>

enum Exception : size_t {
    Syscall = 0x8,
    Overflow = 0xC,
    BadLoadAddress = 0x4,
    BadStoreAddress = 0x5,
};
