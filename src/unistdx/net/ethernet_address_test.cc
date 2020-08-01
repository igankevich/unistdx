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

#include <unistdx/net/ethernet_address>
#include <unistdx/test/operator>

TEST(ethernet_address, Calculus) {
    using namespace sys;
    EXPECT_EQ(ethernet_address{}, ethernet_address{});
    EXPECT_NE(
        ethernet_address{},
        ethernet_address(0xff,0xff,0xff,0xff,0xff,0xff)
    );
    EXPECT_EQ(
        ethernet_address(0xff,0xff,0xff,0xff,0xff,0xff),
        ethernet_address(0xff,0xff,0xff,0xff,0xff,0xff)
    );
}

typedef decltype(std::right) manipulator;

void
test_print(
    const char* expected,
    sys::ethernet_address addr,
    manipulator manip = std::left,
    size_t width=17
) {
    std::stringstream str;
    str << std::setw(width) << manip << addr;
    std::string result = str.str();
    EXPECT_EQ(width, result.size());
    EXPECT_EQ(expected, str.str());
}

TEST(ethernet_address, print_padding) {
    using namespace sys;
    test_print("00:00:00:00:00:00", ethernet_address{});
    test_print("ff:ff:ff:ff:ff:ff", ethernet_address{0xff,0xff,0xff,0xff,0xff,0xff});
    test_print(" 00:00:00:00:00:00", ethernet_address{}, std::right, 18);
    test_print("00:00:00:00:00:00 ", ethernet_address{}, std::left, 18);
}

TEST(ethernet_address, io) {
    using namespace sys;
    test::io_operators(ethernet_address{0xff,0xff,0xff,0xff,0xff,0xff});
}
