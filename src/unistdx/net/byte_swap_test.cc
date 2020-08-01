/*
UNISTDX — C++ library for Linux system calls.
© 2020 Ivan Gankevich

This file is part of UNISTDX.

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

#include <unistdx/net/byte_swap>

#include <gtest/gtest.h>

TEST(ByteSwap, All) {
    #define UINT128_C(x) x ## _u128
    using namespace sys;
    EXPECT_EQ(byte_swap<u16>(UINT16_C(0xABCD)), UINT16_C(0xCDAB));
    EXPECT_EQ(byte_swap<u32>(UINT32_C(0xABCDDCBA)), UINT32_C(0xBADCCDAB));
    EXPECT_EQ(
        byte_swap<u64>(UINT64_C(0xABCDDCBA12344321)),
        UINT64_C(0x21433412BADCCDAB)
    );
    using namespace sys::literals;
    EXPECT_EQ(
        byte_swap<u128>(UINT128_C(0xABCDDCBA12344321ae15826154cdef25)),
        UINT128_C(0x25efcd54618215ae21433412badccdab)
    );
}
