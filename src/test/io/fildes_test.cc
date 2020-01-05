#include <gtest/gtest.h>

#include <set>

#include <unistdx/bits/for_each_file_descriptor>
#include <unistdx/io/pipe>
#include <unistdx/io/fildes>

#include <unistdx/test/temporary_file>

TEST(ForEachFileDescriptor, Pipe) {
    sys::pipe p;
    std::set<sys::fd_type> expected{p.in().fd(), p.out().fd()}, actual;
    sys::bits::for_each_file_descriptor(
        [&] (const sys::poll_event& rhs) {
            const auto fd = rhs.fd();
            if (fd == p.in().fd() || fd == p.out().fd()) {
                actual.emplace(fd);
            }
        }
    );
    EXPECT_EQ(expected, actual);
}

TEST(fildes, duplicate) {
    sys::fildes f(UNISTDX_TMPFILE, sys::open_flag::create, 0644);
    sys::fildes tmp1(f);
    sys::fildes tmp2(1000);
    tmp2 = f;
}

TEST(fildes, basic) {
    sys::fildes a, b;
    EXPECT_FALSE(a);
    EXPECT_FALSE(b);
    EXPECT_EQ(a, b);
    EXPECT_EQ(a, b.fd());
    EXPECT_EQ(a.fd(), b);
    EXPECT_NO_THROW(b.open(UNISTDX_TMPFILE, sys::open_flag::create, 0644));
    EXPECT_TRUE(static_cast<bool>(b));
    EXPECT_NE(a, b);
    EXPECT_NE(a.fd(), b);
    EXPECT_NE(a, b.fd());
}

TEST(fildes, traits) {
    typedef sys::streambuf_traits<sys::fildes> traits_type;
    sys::fildes a;
    char buf[1024] = {0};
    EXPECT_THROW(traits_type::read(a, buf, 1024), sys::bad_call);
    EXPECT_THROW(traits_type::write(a, buf, 1024), sys::bad_call);
}

TEST(fd_type, traits) {
    typedef sys::streambuf_traits<sys::fd_type> traits_type;
    sys::fildes a;
    char buf[1024] = {0};
    EXPECT_THROW(traits_type::read(a.fd(), buf, 1024), sys::bad_call);
    EXPECT_THROW(traits_type::write(a.fd(), buf, 1024), sys::bad_call);
    a.open("/dev/null");
    EXPECT_TRUE(static_cast<bool>(a));
    EXPECT_NO_THROW(traits_type::read(a.fd(), buf, 1024));
}
