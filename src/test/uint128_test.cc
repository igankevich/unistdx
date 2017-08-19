#include <iostream>
#include <sstream>
#include <string>
#include <unistdx/base/uint128>
#include <gtest/gtest.h>

typedef std::ios_base& (*manipulator_type)(std::ios_base&);

namespace std {
	constexpr int numeric_limits<sys::uint128_t>::digits;
}


template<class I>
void
check_op(I x, const std::string& y) {
	std::stringstream tmp;
	tmp << x;
	std::string res = tmp.str();
	EXPECT_EQ(y, res);
}

template<class T>
void
check_write(T rhs, const char* expected_result, manipulator_type pf = nullptr) {
	std::stringstream str;
	if (pf) { str << pf; }
	str << rhs;
	std::string result = str.str();
	EXPECT_EQ(expected_result, result);
}

template<class T>
void
check_read(const char* str, T expected_result, manipulator_type pf = nullptr) {
	T result;
	std::stringstream s;
	s << str;
	if (pf) s >> pf;
	s >> result;
	EXPECT_EQ(expected_result, result);
}

TEST(Uint128Test, Operators) {
	using namespace sys::literals;
	using sys::uint128_t;
	// check different numbers
	check_op(uint128_t(0u), "0");
	check_op(uint128_t(1), "1");
	check_op(100000000000000000000_u128, "100000000000000000000");
	check_op(18446744073709551615_u128, "18446744073709551615"); // 2^64-1
	check_op(18446744073709551616_u128, "18446744073709551616"); // 2^64
	check_op(18446744073709551617_u128, "18446744073709551617"); // 2^64+1
	check_op(18446744073709551618_u128, "18446744073709551618"); // 2^64+2
	check_op(1180591620717411303423_u128, "1180591620717411303423"); // 2^70-1
	check_op(1180591620717411303424_u128, "1180591620717411303424"); // 2^70
	check_op(1208925819614629174706176_u128, "1208925819614629174706176"); // 2^80
	check_op(1267650600228229401496703205376_u128, "1267650600228229401496703205376"); // 2^100
	check_op(170141183460469231731687303715884105727_u128, "170141183460469231731687303715884105727");
	check_op(340282366920938463463374607431768211455_u128, "340282366920938463463374607431768211455");
	check_op(0xffffffffffffffffffffffffffffffff_u128, "340282366920938463463374607431768211455");
	check_op(0x0123456789abcdef_u128, "81985529216486895");
	check_write(0x123456789abcdef_u128, "123456789abcdef", std::hex);
	check_write(0123_u128, "123", std::oct);
	// check arithmetic operations
	check_op(uint128_t(1)+1, "2");
	check_op(18446744073709551615_u128+1, "18446744073709551616");
	check_op(18446744073709551615_u128*1000_u128, "18446744073709551615000");
	check_op(18446744073709551615_u128*0_u128, "0");
	check_op(18446744073709551615_u128*1_u128, "18446744073709551615");
	check_op(18446744073709551615_u128%1000_u128, "615");
	check_op(18446744073709551615_u128/1000_u128, "18446744073709551");
	check_op(18446744073709551615_u128-1000_u128, "18446744073709550615");
	// check logical operations
	check_op(18446744073709551615_u128&123456789_u128, "123456789");
	check_op(18446744073709551615_u128|123456789_u128, "18446744073709551615");
	check_op(18446744073709551615_u128^123456789_u128, "18446744073586094826");
}

TEST(Uint128Test, ReadWrite) {
	using namespace sys::literals;
	check_write(0xffffffffffffff_u128<<8_u128, "ffffffffffffff00", std::hex);
	check_write(0xffffffffffffff_u128>>8_u128, "ffffffffffff", std::hex);
//	check_write(0xffffffffffffff_u128>>129_u128, "0", std::hex);
//	check_write(0xffffffffffffff_u128<<129_u128, "0", std::hex);
//	check_write(0xffffffffffffff_u128>>100_u128, "0", std::hex);
	check_write(0xffffffffffffff_u128<<100_u128, "fffffff0000000000000000000000000", std::hex);
	// check I/O
	check_write(123_u128, "123");
	check_write(0x123456789abcdef_u128, "123456789abcdef", std::hex);
	check_write(0123_u128, "123", std::oct);
	check_read("123", 123_u128);
	check_read("123", 0123_u128, std::oct);
	check_read("123456789abcdef", 0x123456789abcdef_u128, std::hex);
	check_read("ffffffff", 0xffffffff_u128, std::hex);
	check_read("abcdef", 0xabcdef_u128, std::hex);
	// literals
	check_write(10000_u128, "10000");
	check_write(0xabcde_u128, "abcde", std::hex);
	check_write(010000_u128, "10000", std::oct);
}

TEST(Uint128Test, Limits) {
	using namespace sys::literals;
	using sys::uint128_t;
	EXPECT_EQ(
		340282366920938463463374607431768211455_u128,
		std::numeric_limits<uint128_t>::max()
	);
	EXPECT_EQ(0_u128, std::numeric_limits<uint128_t>::min());
	EXPECT_EQ(128, std::numeric_limits<uint128_t>::digits);
}
