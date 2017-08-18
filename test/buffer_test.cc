#include <stdx/debug.hh>

#include <gtest/gtest.h>
#include <random>
#include <sys/io/fdstream>
#include <sys/io/fildesbuf>
#include <sys/net/pstream>
#include <vector>

#include "datum.hh"


template <class T>
struct BufferTest: public ::testing::Test {

	std::basic_string<T>
	random_string(std::streamsize n) {
		std::uniform_int_distribution<T> dist('a', 'z');
		std::basic_string<T> str(n, '_');
		std::generate(str.begin(), str.end(), std::bind(dist, rng));
		return str;
	}

	std::vector<std::streamsize> _sizes {1, 2, 3, 133, 4095, 4096, 4097};
	std::default_random_engine rng;
};

typedef ::testing::Types<char, unsigned char> MyTypes;
TYPED_TEST_CASE(BufferTest, MyTypes);

TYPED_TEST(BufferTest, FdStream) {

	typedef TypeParam T;
	typedef std::char_traits<T> Tr;
	typedef std::basic_stringbuf<T> Fd;
	typedef sys::basic_fdstream<T,Tr,Fd> stream_type;
	typedef Fd sink_type;
	for (std::streamsize k : this->_sizes) {
		std::basic_string<T> expected_contents = this->random_string(k);
		stream_type s {sink_type {}};
		s << expected_contents << std::flush;
		std::basic_stringstream<T> contents;
		contents << s.rdbuf();
		std::basic_string<T> result = contents.str();
		EXPECT_EQ(result, expected_contents);
	}
}

TYPED_TEST(BufferTest, FildesBuf) {

	typedef TypeParam T;
	typedef std::char_traits<T> Tr;
	typedef std::basic_stringbuf<T> Fd;
	typedef sys::basic_fildesbuf<T,Tr,Fd> packetbuf_type;
	typedef Fd sink_type;

	for (std::streamsize k : this->_sizes) {
		std::basic_string<T> contents = this->random_string(k);
		packetbuf_type buf(sink_type {});
		std::basic_ostream<T> out(&buf);
		buf.begin_packet();
		out << contents;
		buf.end_packet();
		buf.pubsync();
		std::basic_istream<T> in(&buf);
		std::basic_string<T> result(k, '_');
		buf.pubsync();
		buf.read_packet();
		in.read(&result[0], k);
		if (in.gcount() < k) {
			in.clear();
			buf.read_packet();
			in.read(&result[0], k);
		}
		EXPECT_EQ(result, contents);
	}
}

TYPED_TEST(BufferTest, PacketStream) {

	typedef TypeParam T;
	typedef std::char_traits<T> Tr;
	typedef std::basic_stringbuf<T> Fd;
	typedef Fd sink_type;
	typedef sys::basic_fildesbuf<T,Tr,Fd> basebuf;

	for (std::streamsize size : this->_sizes) {

		std::vector<Datum> input(size);
		std::vector<Datum> output(size);

		basebuf buf {sink_type {}};
		sys::basic_pstream<T> str(&buf);

		EXPECT_EQ(0, str.tellp()) << "buffer is not empty before write";
		std::for_each(
			input.begin(),
			input.end(),
			[&str] (const Datum& rhs) {
			    str.begin_packet();
			    str << rhs;
			    str.end_packet();
			}
		);
		str.flush();

		EXPECT_EQ(0, str.tellg()) << "buffer is not empty before read";

		str.sync();
		std::for_each(
			output.begin(),
			output.end(),
			[&str] (Datum& rhs) {
			    str.read_packet();
			    str >> rhs;
			}
		);

		EXPECT_EQ(input, output);
	}
}
