/*
UNISTDX — C++ library for Linux system calls.
© 2017, 2018, 2020 Ivan Gankevich

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

#include <gtest/gtest.h>

#include <algorithm>
#include <iomanip>
#include <limits>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include <unistdx/base/websocket>
#include <unistdx/base/websocketbuf>
#include <unistdx/io/fildesbuf>
#include <unistdx/io/pipe>

#include <unistdx/test/operator>
#include <unistdx/test/random_string>

using sys::u16;

struct PacketSizeTest: public ::testing::TestWithParam<size_t> {};

std::vector<size_t> packet_sizes{
    1,
    2,
    3,
    4,
    125,
    126,
    127,
    4095,
    4096,
    4097,
    std::numeric_limits<u16>::max()-1,
    std::numeric_limits<u16>::max(),
    std::numeric_limits<u16>::max()+1,
    0
};

TEST_P(PacketSizeTest, Write) {

    typedef char T;
    typedef std::char_traits<T> Tr;
    typedef std::basic_stringbuf<T> Fd;
    class websocketbuf:
        public sys::basic_websocketbuf<T,Tr>,
        public sys::basic_fildesbuf<T,Tr,Fd> {};
    typedef websocketbuf packetbuf_type;
    typedef typename packetbuf_type::role_type role;

    std::string packet_in = test::random_string<char>(GetParam());
    std::string packet_out(packet_in.size(), '_');

    packetbuf_type buf;
    buf.role(role::client);
    EXPECT_EQ(role::client, buf.role());
    EXPECT_FALSE(buf.client_handshake());
    EXPECT_FALSE(buf.handshake());
    buf.pubsync();
    buf.role(role::server);
    EXPECT_EQ(role::server, buf.role());
    EXPECT_FALSE(buf.server_handshake());
    EXPECT_FALSE(buf.handshake());
    buf.pubsync();
    buf.role(role::client);
    EXPECT_TRUE(buf.client_handshake());
    buf.pubsync();
    buf.role(role::server);
    EXPECT_TRUE(buf.server_handshake());
    buf.role(role::client);
    buf.begin_packet();
    buf.sputn(packet_in.data(), packet_in.size());
    buf.end_packet();
    buf.pubsync();
    buf.role(role::server);
    EXPECT_TRUE(buf.read_packet());
    buf.sgetn(&packet_out[0], packet_out.size());
    EXPECT_EQ(packet_in, packet_out) << "packet_size=" << packet_in.size();
}

INSTANTIATE_TEST_CASE_P(
    WebSocketBuf,
    PacketSizeTest,
    ::testing::ValuesIn(packet_sizes)
);


TEST(websocket_frame, members) {
    sys::websocket_frame frame;
    frame.mask(0);
    EXPECT_NE("", test::stream_insert(frame));
}

struct websocket_test_params {
    std::string method;
    std::string header;
    std::string status;
};

std::ostream& operator<<(std::ostream& out, const websocket_test_params& rhs) {
    out << "Method: \"" << rhs.method << "\"\n";
    out << "Header: \"" << rhs.header << "\"\n";
    out << "Status: \"" << rhs.status << "\"\n";
    return out;
}

struct websocketbuf_test:
    public ::testing::TestWithParam<websocket_test_params> {};

std::vector<websocket_test_params> all_headers {
    {"\r\n", "", ""},
    {"GED / HTTP/1.1\r\n", "", ""},
    {"GET / HTTP/1.1\r\ngarbage\r\n", "", ""},
    {"GET / HTTP/1.1\r\n\r\n", "", ""},
    {"", "", "\r\n"},
    {"", "", "HDDP\r\n"},
    {"", "", "HTTP/1.1\r\n"},
    {"", "", "HTTP/1.1 1\r\n"},
    {"", "", "HTTP/1.1 999\r\n"},
};

TEST_P(websocketbuf_test, errors) {

    typedef char T;
    typedef std::char_traits<T> Tr;
    typedef std::basic_stringbuf<T> Fd;
    class websocketbuf:
        public sys::basic_websocketbuf<T,Tr>,
        public sys::basic_fildesbuf<T,Tr,Fd> {};
    typedef websocketbuf packetbuf_type;
    typedef typename packetbuf_type::role_type role;

    std::string bad_method = GetParam().method;
    std::string bad_header = GetParam().header;
    std::string bad_status = GetParam().status;
    packetbuf_type buf;
    buf.role(role::client);
    if (!bad_method.empty()) {
        buf.sputn(bad_method.data(), bad_method.size());
    }
    EXPECT_FALSE(buf.client_handshake());
    if (!bad_header.empty()) {
        buf.sputn(bad_header.data(), bad_header.size());
    }
    buf.pubsync();
    buf.role(role::server);
    if (!bad_status.empty()) {
        buf.sputn(bad_status.data(), bad_status.size());
    }
    EXPECT_FALSE(buf.server_handshake());
    buf.pubsync();
    buf.role(role::client);
    EXPECT_FALSE(buf.client_handshake());
    buf.pubsync();
    buf.role(role::server);
    EXPECT_FALSE(buf.server_handshake());
}

INSTANTIATE_TEST_CASE_P(
    _,
    websocketbuf_test,
    ::testing::ValuesIn(all_headers)
);
