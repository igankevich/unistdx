#include <functional>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#include <unistdx/base/make_object>
#include <unistdx/io/fdstream>
#include <unistdx/io/fildesbuf>
#include <unistdx/net/bytes>
#include <unistdx/net/pstream>
#include <unistdx/test/datum>
#include <unistdx/test/make_types>
#include <unistdx/test/random_buffer>

#include <test/io/kernelbuf.hh>

template <class Fildesbuf>
struct BufferTest: public ::testing::Test {

    typedef typename Fildesbuf::char_type char_type;
    typedef typename Fildesbuf::traits_type traits_type;
    typedef std::basic_string<char_type,traits_type> string_type;

    inline string_type
    random_string(std::streamsize n) {
        std::uniform_int_distribution<char_type> dist('a', 'z');
        string_type str(n, '_');
        std::generate(str.begin(), str.end(), std::bind(dist, rng));
        return str;
    }

    std::vector<std::streamsize> _sizes {1, 2, 3, 133, 4095, 4096, 4097};
    std::default_random_engine rng;
};

TYPED_TEST_CASE(
    BufferTest,
    MAKE_TYPES(
        sys::basic_fildesbuf<char,
        std::char_traits<char>,
        test::random_buffer>
    )
);

TYPED_TEST(BufferTest, FdStream) {
    typedef typename TypeParam::char_type char_type;
    typedef typename TypeParam::traits_type traits_type;
    typedef typename TypeParam::fd_type fd_type;
    typedef sys::basic_fdstream<char_type,traits_type,fd_type> stream_type;
    typedef std::basic_string<char_type,traits_type> string_type;
    typedef std::basic_stringstream<char_type,traits_type> stringstream_type;
    for (std::streamsize k : this->_sizes) {
        string_type expected_contents = this->random_string(k);
        stream_type s {fd_type {}};
        s << expected_contents;
        while (s.fdbuf().dirty()) {
            s.fdbuf().pubflush();
        }
        std::streamsize nread = 0;
        do {
            nread += s.fdbuf().pubfill();
        } while (nread < k);
        stringstream_type contents;
        contents << s.rdbuf();
        string_type result = contents.str();
        EXPECT_EQ(result, expected_contents);
    }
}

TYPED_TEST(BufferTest, FildesBuf) {
    typedef typename TypeParam::char_type char_type;
    typedef typename TypeParam::traits_type traits_type;
    typedef typename TypeParam::fd_type fd_type;
    typedef TypeParam fildesbuf_type;
    typedef std::basic_string<char_type,traits_type> string_type;
    typedef std::basic_ostream<char_type,traits_type> ostream_type;
    typedef std::basic_istream<char_type,traits_type> istream_type;
    for (std::streamsize k : this->_sizes) {
        std::clog << "Test size=" << k << std::endl;
        string_type contents = this->random_string(k);
        fildesbuf_type buf(fd_type {});
        ostream_type out(&buf);
        buf.begin_packet();
        out << contents;
        buf.end_packet();
        while (buf.dirty()) {
            buf.pubflush();
        }
        istream_type in(&buf);
        string_type result(k, '_');
        std::streamsize nread = 0;
        do {
            nread += buf.pubfill();
        } while (nread < k);
        in.read(&result[0], k);
        EXPECT_EQ(result, contents);
    }
}

TYPED_TEST(BufferTest, PacketStream) {

    typedef typename TypeParam::char_type char_type;
    typedef typename TypeParam::fd_type fd_type;
    typedef TypeParam fildesbuf_type;
    typedef typename fildesbuf_type::ipacket_guard ipacket_guard;
    typedef test::basic_kernelbuf<fildesbuf_type> kernelbuf;

    for (std::streamsize size : this->_sizes) {

        std::vector<Datum> input(size);
        std::vector<Datum> output(size);

        kernelbuf buf;
        buf.setfd(fd_type());
        sys::basic_pstream<char_type> str(&buf);

        EXPECT_TRUE(buf.handshake());

        std::for_each(
            input.begin(),
            input.end(),
            [&str] (const Datum& rhs) {
                str.begin_packet();
                str << rhs;
                str.end_packet();
            }
        );
        while (buf.dirty()) {
            buf.pubflush();
        }

        int i = 1;
        std::for_each(
            output.begin(),
            output.end(),
            [&] (Datum& rhs) {
                while (!buf.read_packet()) {
                    buf.pubfill();
                }
                ipacket_guard g(&buf);
                str >> rhs;
                ++i;
            }
        );

        for (size_t j=0; j<input.size(); ++j) {
            EXPECT_EQ(
                sys::make_bytes(input[j]),
                sys::make_bytes(output[j])
            ) << "packet #" << (j+1);
        }
    }
}
