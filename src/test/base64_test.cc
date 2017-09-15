#include <unistdx/base/base64>

#include <gtest/gtest.h>

#include "random_string.hh"

using sys::base64_decode;
using sys::base64_encode;
using sys::base64_encoded_size;
using sys::base64_max_decoded_size;

typedef std::pair<std::string, std::string> pair_type;

struct SmallSizeTest: public ::testing::TestWithParam<size_t> {};
struct BigSizeTest: public ::testing::TestWithParam<size_t> {};
struct Base64EncodeTest: public ::testing::TestWithParam<pair_type> {};
struct Base64DecodeTest: public ::testing::TestWithParam<pair_type> {};

std::vector<size_t> SMALL_SIZES{0, 1, 2, 3, 4, 22, 77, 4095, 4096, 4097};
std::vector<size_t> BIG_SIZES{
	std::numeric_limits<size_t>::max(),
	std::numeric_limits<size_t>::max()/4u*3u
};

std::vector<pair_type> encode_pairs{
	{"", ""},
	{"a", "YQ=="},
	{"aa", "YWE="},
	{"jM", "ak0="},
	{"aaa", "YWFh"},
	{"abc", "YWJj"},
	{"aaaa", "YWFhYQ=="},
	{"abcd", "YWJjZA=="},
};

std::vector<pair_type> decode_pairs = encode_pairs;

TEST_P(SmallSizeTest, SmallSizes) {
	size_t k = GetParam();
	size_t sz1 = 0, sz2 = 0;
	EXPECT_NO_THROW({
		sz1 = base64_encoded_size(k);
		sz2 = base64_max_decoded_size(sz1);
	});
	EXPECT_GE(sz2, k);
}

INSTANTIATE_TEST_CASE_P(
	Base64EncodedAndMaxDecodedSize,
	SmallSizeTest,
	::testing::ValuesIn(SMALL_SIZES)
);

TEST_P(BigSizeTest, BigSizes) {
	size_t k = GetParam();
	size_t sz1 = 0, sz2 = 0;
	EXPECT_THROW({
		sz1 = base64_encoded_size(k);
		sz2 = base64_max_decoded_size(sz1);
	}, std::length_error);
	EXPECT_EQ(0, sz1);
	EXPECT_EQ(0, sz2);
}

INSTANTIATE_TEST_CASE_P(
	Base64EncodedAndMaxDecodedSize,
	BigSizeTest,
	::testing::ValuesIn(BIG_SIZES)
);

template<class T>
void test_base64(size_t k, bool spoil) {
	typedef std::basic_string<T> string;
	string text = random_string<T>(k);
	string encoded(base64_encoded_size(k), '_');
	string decoded(base64_max_decoded_size(encoded.size()), '_');
	base64_encode(text.data(), text.data() + text.size(), &encoded[0]);
	if (spoil && !encoded.empty()) {
		size_t pos = encoded.size()/2u;
		encoded[pos] = 128;
		EXPECT_THROW(
			base64_decode(
				encoded.data(),
				encoded.data() + encoded.size(),
				&decoded[0]
			),
			std::invalid_argument
		);
	} else {
		size_t decoded_size = base64_decode(
			encoded.data(),
			encoded.data() + encoded.size(),
			&decoded[0]
		);
		decoded.resize(decoded_size);
		EXPECT_EQ(text, decoded) << "encoded: " << encoded;
	}
}

TEST_P(SmallSizeTest, Base64EncodeDecode) {
	size_t k = GetParam();
	test_base64<char>(k, false);
	test_base64<char>(k, true);
}

INSTANTIATE_TEST_CASE_P(
	Base64EncodedDecode,
	SmallSizeTest,
	::testing::ValuesIn(SMALL_SIZES)
);

TEST_P(BigSizeTest, Base64EncodeDecode) {
	size_t k = GetParam();
	EXPECT_THROW(test_base64<char>(k, false), std::length_error);
	EXPECT_THROW(test_base64<char>(k, true), std::length_error);
}

INSTANTIATE_TEST_CASE_P(
	Base64EncodedDecode,
	BigSizeTest,
	::testing::ValuesIn(BIG_SIZES)
);

TEST_P(Base64EncodeTest, X) {
	pair_type p = GetParam();
	std::string result(base64_encoded_size(p.first.size()), '_');
	base64_encode(
		p.first.data(),
		p.first.data() + p.first.size(),
		&result[0]
	);
	EXPECT_EQ(p.second, result);
}

INSTANTIATE_TEST_CASE_P(
	Base64EncodeTest,
	Base64EncodeTest,
	::testing::ValuesIn(encode_pairs)
);

TEST_P(Base64DecodeTest, X) {
	pair_type p = GetParam();
	std::swap(p.first, p.second);
	std::string result(base64_max_decoded_size(p.first.size()), '_');
	size_t n = base64_decode(
		p.first.data(),
		p.first.data() + p.first.size(),
		&result[0]
	);
	result.resize(n);
	EXPECT_EQ(p.second, result);
}

INSTANTIATE_TEST_CASE_P(
	Base64DecodeTest,
	Base64DecodeTest,
	::testing::ValuesIn(decode_pairs)
);

/*
TEST(Base63, GenerateTable) {
	const unsigned char base64_alphabet[64] = {
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
		'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b',
		'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
		'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3',
		'4', '5', '6', '7', '8', '9', '+', '/'
	};
	unsigned char table[128] = {0};
	std::memset(table, 77, 128);
	for (int i=0; i<64; ++i) {
		table[base64_alphabet[i]] = i;
	}
	table['='] = 0;
	for (int i=0; i<128; ++i) {
		std::cout << int(table[i]) << ',';
	}
}
*/
