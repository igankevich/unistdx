#include <algorithm>
#include <gtest/gtest.h>
#include <iomanip>
#include <string>
#include <vector>
#include <sstream>
#include <ostream>

#include <unistdx/base/websocket>
#include <unistdx/base/websocketbuf>
#include <unistdx/io/fildesbuf>
#include <unistdx/io/pipe>

#include "random_string.hh"

TEST(WebSocket, EncodeDecode) {
	std::random_device rng;
	std::vector<std::string> inputs = {""};
	for (int i=0; i<20; ++i) {
		std::string str = random_string<char>(1 << i);
		inputs.push_back(str);
	}
	std::for_each(inputs.begin(), inputs.end(),
		[&rng] (const std::string& in) {
			std::stringstream str;
			std::ostream_iterator<char> oit(str);
			std::stringstream str2;
			std::ostream_iterator<char> oit2(str2);
			sys::opcode_type opcode;
			sys::websocket_encode(in.begin(), in.end(), oit, rng);
			std::string encoded = str.str();
			sys::websocket_decode(encoded.begin(), encoded.end(), oit2, &opcode);
			std::string out = str2.str();
			EXPECT_EQ(out, in);
		}
	);
}

TEST(WebSocketBuf, Write) {

	typedef char T;
	typedef std::char_traits<T> Tr;
	typedef std::basic_stringbuf<T> Fd;
	class websocketbuf:
		public sys::basic_websocketbuf<T,Tr>,
		public sys::basic_fildesbuf<T,Tr,Fd> {};
	typedef websocketbuf packetbuf_type;
	typedef typename packetbuf_type::role_type role;

	packetbuf_type buf;
	buf.role(role::client);
	buf.update_client_state();
	buf.pubsync();
	buf.role(role::server);
	buf.update_server_state();
	buf.pubsync();
	buf.role(role::client);
	buf.update_client_state();

}
