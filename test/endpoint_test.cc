#include <stdx/debug.hh>

#include <iostream>

#include <sys/endpoint.hh>
#include <sys/packetstream.hh>
#include <stdx/random.hh>

#include "test.hh"


using stdx::n_random_bytes;
using sys::operator""_ipv4;
using sys::ipv4_addr;
using sys::ipv6_addr;


template<class T = uint32_t, class IP_addr = ipv4_addr>
struct Test_endpoint {

	typedef std::random_device engine_type;

	void test_single() {
		sys::endpoint addr1 = random_addr();
		sys::endpoint addr2;
		std::stringstream s;
		s << addr1;
		s >> addr2;
		if (addr1 != addr2) {
			std::stringstream msg;
			msg << "[single] Addresses do not match: " << addr1 << " /= " << addr2;
			throw std::runtime_error(msg.str());
		}
	}

	void test_multiple() {
		std::vector<sys::endpoint> addrs(10);
		std::generate(addrs.begin(), addrs.end(), [this] () { return random_addr(); });

		// write
		std::stringstream os;
		std::ostream_iterator<sys::endpoint> oit(os, "\n");
		std::copy(addrs.begin(), addrs.end(), oit);

		// read
		std::vector<sys::endpoint> addrs2;
		std::istream_iterator<sys::endpoint> iit(os), eos;
		std::copy(iit, eos, std::back_inserter(addrs2));

		if (addrs.size() != addrs2.size()) {
			std::stringstream msg;
			msg << "[multiple] Read/write arrays size do not match: "
				<< addrs.size() << " /= " << addrs2.size();
			throw std::runtime_error(msg.str());
		}

		for (size_t i=0; i<addrs.size(); ++i) {
			if (addrs[i] != addrs2[i]) {
				std::stringstream msg;
				msg << "[multiple] Addresses do not match: "
					<< addrs[i] << " /= " << addrs2[i];
				throw std::runtime_error(msg.str());
			}
		}
	}

	void test_variations_ipv4() {
		// basic functionality
		check_read("0.0.0.0:0"             , sys::endpoint({0,0,0,0}         , 0));
		check_read("0.0.0.0:1234"          , sys::endpoint({0,0,0,0}         , 1234));
		check_read("0.0.0.0:65535"         , sys::endpoint({0,0,0,0}         , 65535));
		check_read("10.0.0.1:0"            , sys::endpoint({10,0,0,1}        , 0));
		check_read("255.0.0.1:0"           , sys::endpoint({255,0,0,1}       , 0));
		check_read("255.255.255.255:65535" , sys::endpoint({255,255,255,255} , 65535));
		// out of range ports
		check_read("0.0.0.0:65536"         , sys::endpoint({0,0,0,0}         , 0));
		check_read("0.0.0.1:65536"         , sys::endpoint({0,0,0,0}         , 0));
		check_read("10.0.0.1:100000"       , sys::endpoint({0,0,0,0}         , 0));
		// out of range addrs
		check_read("1000.0.0.1:0"          , sys::endpoint({0,0,0,0}         , 0));
		// good spaces
		check_read(" 10.0.0.1:100"         , sys::endpoint({10,0,0,1}        , 100));
		check_read("10.0.0.1:100 "         , sys::endpoint({10,0,0,1}        , 100));
		check_read(" 10.0.0.1:100 "        , sys::endpoint({10,0,0,1}        , 100));
		// bad spaces
		check_read("10.0.0.1: 100"         , sys::endpoint({0,0,0,0}         , 0));
		check_read("10.0.0.1 :100"         , sys::endpoint({0,0,0,0}         , 0));
		check_read("10.0.0.1 : 100"        , sys::endpoint({0,0,0,0}         , 0));
		check_read(" 10.0.0.1 : 100 "      , sys::endpoint({0,0,0,0}         , 0));
		// fancy addrs
		check_read("10:100"                , sys::endpoint({0,0,0,0}         , 0));
		check_read("10.1:100"              , sys::endpoint({0,0,0,0}         , 0));
		check_read("10.0.1:100"            , sys::endpoint({0,0,0,0}         , 0));
		check_read(""                      , sys::endpoint({0,0,0,0}         , 0));
		check_read("anc:100"               , sys::endpoint({0,0,0,0}         , 0));
		check_read(":100"                  , sys::endpoint({0,0,0,0}         , 0));
		check_read("10.0.0.0.1:100"        , sys::endpoint({0,0,0,0}         , 0));
	}

	void test_variations_ipv6() {
		std::clog << __func__ << std::endl;
		// basic functionality
		check_read("[::1]:0"                 , sys::endpoint({0x0,0,0,0,0,0,0,1}  , 0));
		check_read("[1::1]:0"                , sys::endpoint({0x1,0,0,0,0,0,0,1}  , 0));
		check_read("[::]:0"                  , sys::endpoint({0x0,0,0,0,0,0,0,0}  , 0));
		check_read("[2001:1:0::123]:0"       , sys::endpoint({0x2001,1,0,0,0,0,0,0x123}  , 0));
		check_read("[0:0:0:0:0:0:0:0]:0"     , sys::endpoint({0x0,0,0,0,0,0,0,0}  , 0));
		check_read("[0:0:0:0:0:0:0:0]:1234"  , sys::endpoint({0x0,0,0,0,0,0,0,0}  , 1234));
		check_read("[0:0:0:0:0:0:0:0]:65535" , sys::endpoint({0x0,0,0,0,0,0,0,0}  , 65535));
		check_read("[10:1:0:1:0:0:0:0]:0"    , sys::endpoint({0x10,1,0,1,0,0,0,0} , 0));
		check_read("[255:0:0:1:1:2:3:4]:0"   , sys::endpoint({0x255,0,0,1,1,2,3,4}, 0));
		check_read("[ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff]:65535",
			sys::endpoint({0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff} , 65535));
		// out of range ports
		check_read("[::1]:65536"             , sys::endpoint({0,0,0,0,0,0,0,0}  , 0));
		check_read("[::0]:65536"             , sys::endpoint({0,0,0,0,0,0,0,0}  , 0));
		check_read("[::0]:100000"            , sys::endpoint({0,0,0,0,0,0,0,0}  , 0));
		// out of range addrs
		check_read("[1ffff::1]:0"              , sys::endpoint({0,0,0,0,0,0,0,0}  , 0));
		// good spaces
		check_read(" [10::1]:100"              , sys::endpoint({0x10,0,0,0,0,0,0,1}        , 100));
		check_read("[10::1]:100 "              , sys::endpoint({0x10,0,0,0,0,0,0,1}        , 100));
		check_read(" [10::1]:100 "             , sys::endpoint({0x10,0,0,0,0,0,0,1}        , 100));
		// bad spaces
		check_read("[10::1]: 100"         , sys::endpoint({0,0,0,0,0,0,0,0}  , 0));
		check_read("[10::1 ]:100"         , sys::endpoint({0,0,0,0,0,0,0,0}  , 0));
		check_read("[10::1 ]: 100"        , sys::endpoint({0,0,0,0,0,0,0,0}  , 0));
		check_read(" [10::1 ]: 100 "      , sys::endpoint({0,0,0,0,0,0,0,0}  , 0));
		// fancy addrs
		check_read("[::1::1]:0"              , sys::endpoint({0,0,0,0,0,0,0,0}  , 0));
		check_read("[:::]:0"                 , sys::endpoint({0,0,0,0,0,0,0,0}  , 0));
		check_read("[:]:0"                   , sys::endpoint({0,0,0,0,0,0,0,0}  , 0));
		check_read("[]:0"                    , sys::endpoint({0,0,0,0,0,0,0,0}  , 0));
		check_read("]:0"                     , sys::endpoint({0,0,0,0,0,0,0,0}  , 0));
		check_read("[:0"                     , sys::endpoint({0,0,0,0,0,0,0,0}  , 0));
		check_read("[10:0:0:0:0:0:0:0:1]:0"  , sys::endpoint({0,0,0,0,0,0,0,0}  , 0));
		// IPv4 mapped addrs
		check_read("[::ffff:127.1.2.3]:0"    , sys::endpoint({0,0,0,0xffff,0x127,1,2,3}, 0));
	}

	void test_operators() {
		// operator bool
		check_bool(sys::endpoint(), false);
		check_bool(!sys::endpoint(), true);
		// operator bool (IPv4)
		check_bool(sys::endpoint("0.0.0.0", 0), false);
		check_bool(!sys::endpoint("0.0.0.0", 0), true);
		check_bool(sys::endpoint("127.0.0.1", 100), true);
		check_bool(!sys::endpoint("127.0.0.1", 100), false);
		check_bool(sys::endpoint("127.0.0.1", 0), true);
		check_bool(!sys::endpoint("127.0.0.1", 0), false);
		// operator bool (IPv6)
		check_bool(sys::endpoint("0:0:0:0:0:0:0:0", 0), false);
		check_bool(!sys::endpoint("0:0:0:0:0:0:0:0", 0), true);
		check_bool(sys::endpoint("::", 0), false);
		check_bool(!sys::endpoint("::", 0), true);
		check_bool(sys::endpoint("::1", 0), true);
		check_bool(!sys::endpoint("::1", 0), false);
		// comparison operators
		check_bool(sys::endpoint("::", 0) == sys::endpoint(), true);
		check_bool(sys::endpoint("0:0:0:0:0:0:0:0", 0) == sys::endpoint(), true);
		check_bool(sys::endpoint("0.0.0.0", 0) == sys::endpoint(), true);
		check_bool(sys::endpoint("::", 0) != sys::endpoint("0.0.0.0", 0), true);
		// ordering
		check_bool(sys::endpoint("10.0.0.1", 0) < sys::endpoint("10.0.0.2", 0), true);
		check_bool(sys::endpoint("10.0.0.2", 0) < sys::endpoint("10.0.0.1", 0), false);
		check_bool(sys::endpoint("10::1", 0) < sys::endpoint("10::2", 0), true);
		check_bool(sys::endpoint("10::2", 0) < sys::endpoint("10::1", 0), false);
		check_bool(sys::endpoint("10.0.0.1", 0) < sys::endpoint("10::1", 0), true);
		// copying
		test::equal(sys::endpoint(sys::endpoint("10.0.0.1", 1234), 100), sys::endpoint("10.0.0.1", 100));
	}

	void test_io() {
		std::vector<sys::endpoint> addrs(10);
		std::generate(addrs.begin(), addrs.end(), [this] () { return random_addr(); });

		// write
		stdx::packetbuf buf;
		sys::packetstream os(&buf);
		std::for_each(
			addrs.begin(), addrs.end(),
			[&os] (const sys::endpoint& rhs) {
				os.begin_packet();
				os << rhs;
				os.end_packet();
			}
		);

		// read
		std::vector<sys::endpoint> addrs2(addrs.size());
		std::for_each(
			addrs2.begin(), addrs2.end(),
			[&os] (sys::endpoint& rhs) {
				os.sync();
				os >> rhs;
			}
		);

		if (addrs.size() != addrs2.size()) {
			std::stringstream msg;
			msg << "[io] Read/write arrays size do not match: "
				<< addrs.size() << " /= " << addrs2.size();
			throw std::runtime_error(msg.str());
		}

		for (size_t i=0; i<addrs.size(); ++i) {
			if (addrs[i] != addrs2[i]) {
				std::stringstream msg;
				msg << "[io] Addresses does not match: "
					<< addrs[i] << " /= " << addrs2[i]
					<< ", input[" << i << "] /= output[" << i << "]";
				throw std::runtime_error(msg.str());
			}
		}
	}

	void test_literals() {
		constexpr ipv4_addr any4;
		constexpr ipv6_addr any6;
		constexpr sys::endpoint any;
		std::cout << "192.168.33.77"_ipv4 << std::endl;
		constexpr sys::endpoint endp("192.168.33.77"_ipv4,0);
		std::cout << endp << std::endl;
		constexpr sys::endpoint endpX(sys::endpoint("10.0.0.1"_ipv4, 1234), 100);
		constexpr sys::endpoint endpY("10.0.0.1"_ipv4, 100);
		test::equal(endpX, endpY);
		constexpr sys::endpoint endpU(sys::endpoint(ipv6_addr(), 1234), 100);
		constexpr sys::endpoint endpV(ipv6_addr(), 100);
		test::equal(endpU, endpV);
	}

private:

	void check_bool(sys::endpoint x, bool y) {
		if ((x && !y) || (!(x) && y)) {
			std::stringstream msg;
			msg << "Boolean operator failed. sys::endpoint=" << x;
			throw std::runtime_error(msg.str());
		}
	}

	void check_bool(bool x, bool y) {
		if (x != y) {
			throw std::runtime_error("Boolean operator failed.");
		}
	}

	void check_read(const char* str, sys::endpoint expected_result) {
		sys::endpoint addr;
		std::stringstream s;
		s << str;
		s >> addr;
		if (addr != expected_result) {
			std::stringstream msg;
			msg << "Read failed for '" << str << "': '"
				<< addr << "' (read) /= '" << expected_result << "' (expected).";
			throw std::runtime_error(msg.str());
		}
	}

	sys::endpoint random_addr() { return sys::endpoint(IP_addr(n_random_bytes<T>(generator)), 0); }

	engine_type generator;
};

struct App {
	void test_ipv4() {
		Test_endpoint<uint32_t, ipv4_addr> test;
		test.test_single();
		test.test_multiple();
		test.test_variations_ipv4();
		test.test_operators();
//		test.test_io();
		test.test_literals();
	}
	void test_ipv6() {
		Test_endpoint<std::uint128_t, ipv6_addr> test;
		test.test_single();
		test.test_multiple();
		test.test_variations_ipv6();
		test.test_operators();
//		test.test_io();
	}
	int run(int, char**) {
//		try {
			test_ipv4();
			test_ipv6();
//		std::clog << sys::endpoint({192, 168, 0, 1}, 0) << std::endl;
		std::clog << sys::endpoint({10, 0, 0, 0, 0, 0, 0, 1}, 0) << std::endl;
//		} catch (std::exception& e) {
//			std::cerr << e.what() << std::endl;
//			return 1;
//		}
		return 0;
	}
};

int main(int argc, char* argv[]) {
	App app;
	return app.run(argc, argv);
}
