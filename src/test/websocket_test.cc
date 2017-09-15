#include <algorithm>
#include <gtest/gtest.h>
#include <iomanip>
#include <string>
#include <vector>

#include <unistdx/base/websocket>

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
