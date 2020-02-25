#include <gtest/gtest.h>
#include <unistdx/net/interface_address>
#include <sstream>
#include <random>
#include <cmath>

#include <unistdx/config>
#include <unistdx/test/make_types>
#include <unistdx/test/operator>

std::default_random_engine rng;

template <class T>
T
genrandom() {
    std::uniform_int_distribution<T> dist(0, std::numeric_limits<T>::max());
    return dist(rng);
}

template <class Addr>
sys::interface_address<Addr>
random_ifaddr() {
    typedef typename Addr::rep_type rep;
    return sys::interface_address<Addr>(Addr(genrandom<rep>()), genrandom<sys::prefix_type>());
}

#if defined(UNISTDX_HAVE_INT128)
TEST(Ifaddr, LocalhostIPv6) {
    typedef sys::interface_address<sys::ipv6_address> ifaddr_type;
    typedef sys::ipaddr_traits<sys::ipv6_address> traits_type;
    ifaddr_type ifa(traits_type::localhost(), traits_type::loopback_mask());
    std::stringstream str;
    str << ifa;
    EXPECT_EQ(str.str(), "0:0:0:0:0:0:0:1/128");
}
#endif

template <class Addr>
struct IfaddrTest: public ::testing::Test {};

#if defined(UNISTDX_HAVE_INT128)
TYPED_TEST_CASE(IfaddrTest, MAKE_TYPES(sys::ipv4_address, sys::ipv6_address));

TYPED_TEST(IfaddrTest, InputOutputOperatorsLocalHost) {
    typedef sys::interface_address<TypeParam> ifaddr_type;
    typedef sys::ipaddr_traits<TypeParam> traits_type;
    ifaddr_type ifa(traits_type::localhost(), traits_type::loopback_mask());
    test::io_operators(ifa);
}

TYPED_TEST(IfaddrTest, InputOutputOperatorsRandom) {
    for (int i=0; i<100; ++i) {
        test::io_operators(random_ifaddr<TypeParam>());
    }
}

TYPED_TEST(IfaddrTest, Loopback) {
    typedef typename TypeParam::rep_type rep;
    typedef sys::interface_address<TypeParam> ifaddr_type;
    typedef sys::ipaddr_traits<TypeParam> traits_type;
    EXPECT_TRUE(
        ifaddr_type(
            traits_type::localhost(),
            traits_type::loopback_mask()
        ).is_loopback()
    );
    EXPECT_TRUE(
        ifaddr_type(
            TypeParam(genrandom<rep>()),
            traits_type::widearea_mask()
        ).is_widearea()
    );
}
#endif

TEST(Ifaddr, ContainsIpV4) {
    typedef sys::interface_address<sys::ipv4_address> ifaddr_type;
    typedef sys::ipaddr_traits<sys::ipv4_address> traits_type;
    ifaddr_type ifa(traits_type::localhost(), traits_type::loopback_mask());
    EXPECT_TRUE(ifa.contains({127,0,0,1}));
    EXPECT_TRUE(ifa.contains({127,0,0,2}));
    EXPECT_TRUE(ifa.contains(*ifa.begin()));
    EXPECT_FALSE(ifa.contains(*ifa.end()));
    EXPECT_FALSE(ifa.contains({127,0,0,0}));
    EXPECT_FALSE(ifa.contains(*--ifa.begin()));
}

TEST(Ifaddr, CountIpv4) {
    typedef sys::interface_address<sys::ipv4_address> ifaddr_type;
    typedef sys::ipaddr_traits<sys::ipv4_address> traits_type;
    ifaddr_type ifa(traits_type::localhost(), traits_type::loopback_mask());
    EXPECT_EQ(std::pow(2, 24)-2, ifa.count());
}
