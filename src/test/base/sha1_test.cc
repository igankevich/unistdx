#include <algorithm>
#include <string>
#include <tuple>
#include <vector>

#include <unistdx/base/sha1>

#include <gtest/gtest.h>

using sys::u32;

typedef std::tuple<std::string, std::string> hash_tuple;

struct SHA1Test: public ::testing::TestWithParam<hash_tuple> {};

const std::vector<std::tuple<std::string, std::string>> KNOWN_HASHES = {
	std::make_tuple(
		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
		"c1c8bbdc 22796e28 c0e15163 d20899b6 5621d65a"
	),
	std::make_tuple("",    "da39a3ee 5e6b4b0d 3255bfef 95601890 afd80709"),
	std::make_tuple("sha", "d8f45903 20e1343a 915b6394 170650a8 f35d6926"),
	std::make_tuple("Sha", "ba79baeb 9f10896a 46ae7471 5271b7f5 86e74640"),
	std::make_tuple(
		"The quick brown fox jumps over the lazy dog",
		"2fd4e1c6 7a2d28fc ed849ee1 bb76e739 1b93eb12"
	),
	std::make_tuple(
		"The quick brown fox jumps over the lazy cog",
		"de9f2c7f d25e1b3a fad3e85a 0bd17d9b 100db4b3"
	),
	std::make_tuple("abc", "a9993e36 4706816a ba3e2571 7850c26c 9cd0d89d"),
	std::make_tuple(
		"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
		"84983e44 1c3bd26e baae4aa1 f95129e5 e54670f1"
	),
};

const std::string SHA_OF_ONE_MILLION_OF_A = "34aa973c d4c4daa4 f61eeb2b dbad2731 6534016f";
const std::string SHA_OF_64_OF_A = "0098ba82 4b5c1642 7bd7a112 2a5a442a 25ec644d";

std::string sha1_digest_to_string(const std::vector<u32>& result) {
	std::stringstream str;
	str << std::hex << std::setfill('0');
	std::for_each(result.begin(), result.end(), [&str] (u32 n) {
		str << std::setw(8) << n << ' ';
	});
	std::string output = str.str();
	output.pop_back(); // remove space character
	return output;
}

TEST_P(SHA1Test, All) {
	const std::string& input = std::get<0>(GetParam());
	const std::string& expected_output = std::get<1>(GetParam());
	std::vector<u32> result(5);
	sys::sha1 sha;
	sha.put(input.data(), input.size());
	sha.compute();
	sha.digest(result.data());
	std::string output = sha1_digest_to_string(result);
	EXPECT_EQ(expected_output, output) << "input=" << input;
}

INSTANTIATE_TEST_CASE_P(
	SHA1Instance,
	SHA1Test,
	::testing::ValuesIn(KNOWN_HASHES)
);


TEST(SHA1, OneMillionOfAs) {
	sys::sha1 sha;
	std::string a = "aaaaaaaaaa";
    for (int i=0; i<100000; i++) {
		sha.put(a.data(), a.size());
	}
	std::vector<u32> result(5);
	sha.compute();
	sha.digest(result.data());
	std::string output = sha1_digest_to_string(result);
	EXPECT_EQ(SHA_OF_ONE_MILLION_OF_A, output)
		<< "SHA of one million of 'a' failed";
}

TEST(SHA1, BigInputs) {
	sys::sha1 sha;
	EXPECT_THROW(
		sha.put("", std::numeric_limits<size_t>::max()),
		std::length_error
	);
	sha.reset();
	sha.put("a", 1);
	EXPECT_THROW(
		sha.put("", std::numeric_limits<size_t>::max()/8-1),
		std::length_error
	);
}

TEST(SHA1, RepeatingCompute) {
	sys::sha1 sha;
	std::vector<char> a(64, 'a');
	sha.put(a.data(), a.size());
	sha.compute();
	EXPECT_EQ(a.size()*8, sha.length());
	sha.compute();
	std::vector<u32> result(5);
	sha.digest(result.data());
	std::string output = sha1_digest_to_string(result);
	EXPECT_EQ(SHA_OF_64_OF_A, output) << "SHA of 64 of 'a' failed";
}
