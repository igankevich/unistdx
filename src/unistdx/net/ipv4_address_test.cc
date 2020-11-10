/*
UNISTDX — C++ library for Linux system calls.
© 2018, 2019, 2020 Ivan Gankevich

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

#include <unistdx/net/ipv4_address>
#include <unistdx/net/ipv6_address>
#include <unistdx/test/operator>

TEST(IPv4Addr, Calculus) {
    using sys::ipv4_address;
    typedef ipv4_address::rep_type rep_type;
    EXPECT_EQ(rep_type(1), ipv4_address(127,0,0,1).position(ipv4_address(255,0,0,0)));
    EXPECT_EQ(rep_type(5), ipv4_address(127,0,0,5).position(ipv4_address(255,0,0,0)));
    EXPECT_EQ(ipv4_address(255,255,255,0).to_prefix(), sys::prefix_type(24));
    EXPECT_EQ(ipv4_address(255,255,0,0).to_prefix(), sys::prefix_type(16));
    EXPECT_EQ(ipv4_address(255,0,0,0).to_prefix(), sys::prefix_type(8));
}

typedef decltype(std::right) manipulator;

void
test_print(
    const char* expected,
    sys::ipv4_address addr,
    manipulator manip,
    size_t width
) {
    std::stringstream str;
    str << std::setw(width) << manip << addr;
    std::string result = str.str();
    EXPECT_EQ(width, result.size());
    EXPECT_EQ(expected, str.str());
}

TEST(ipv4_address, print_padding) {
    test_print(" 127.0.0.1", sys::ipv4_address{127,0,0,1}, std::right, 10);
    test_print("127.0.0.1 ", sys::ipv4_address{127,0,0,1}, std::left, 10);
}

TEST(ipv4_address, identities) {
    EXPECT_EQ(sys::ipv4_address{}, sys::ipv4_address{"0.0.0.0"});
    EXPECT_EQ(sys::ipv4_address(0,0,0,0), sys::ipv4_address{"0.0.0.0"});
}

TEST(ipv4_address, ordering) {
    EXPECT_LT(sys::ipv4_address("10.0.0.1"), sys::ipv4_address("10.0.0.2"));
    EXPECT_GE(sys::ipv4_address("10.0.0.2"), sys::ipv4_address("10.0.0.1"));
}

TEST(ipv6_address, identities) {
    EXPECT_EQ(sys::ipv6_address{}, sys::ipv6_address{"::"});
    EXPECT_EQ(sys::ipv6_address(0,0,0,0, 0,0,0,0), sys::ipv6_address{"0:0:0:0:0:0:0:0"});
}

TEST(ipv6_address, ordering) {
    EXPECT_LT(sys::ipv6_address("10::1"), sys::ipv6_address("10::2"));
    EXPECT_GE(sys::ipv6_address("10::2"), sys::ipv6_address("10::1"));
}
