#include <algorithm>
#include <gtest/gtest.h>
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

#include "random_string.hh"

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

	std::string packet_in = random_string<char>(GetParam());
	std::string packet_out(packet_in.size(), '_');

	packetbuf_type buf;
	buf.role(role::client);
	EXPECT_FALSE(buf.client_handshake());
	buf.pubsync();
	buf.role(role::server);
	EXPECT_FALSE(buf.server_handshake());
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

