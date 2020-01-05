#include <limits>
#include <type_traits>

#include <gtest/gtest.h>

#include <unistdx/ipc/process>
#include <unistdx/net/network_interface>
#include <unistdx/net/network_bridge>

namespace {
    template <class T>
    void print_flags(T flags) {
        using int_type = typename std::underlying_type<T>::type;
        auto nbits = std::numeric_limits<int_type>::nbits * sizeof(int_type);
        for (int_type i=0; i<nbits; ++i) {
            if (flags & (int_type(1) << i)) {
                std::printf("%d 0x%x\n", i, 1<<i);
            }
        }
    }
}

TEST(network_interface, flags) {
    using f = sys::network_interface::flag;
    sys::network_interface lo("lo");
    ASSERT_EQ(f{}, (lo.flags() & f::up));
    lo.flags(lo.flags() | f::up);
    ASSERT_EQ(f::up, (lo.flags() & f::up));
    lo.unsetf(f::up);
    ASSERT_EQ(f{}, (lo.flags() & f::up));
    lo.setf(f::up);
    ASSERT_EQ(f::up, (lo.flags() & f::up));
}

TEST(network_bridge, add) {
    using f = sys::network_interface::flag;
    sys::network_bridge br("br0");
    br.up();
    ASSERT_EQ(f::up, (br.flags() & f::up));
    br.down();
    ASSERT_EQ(f{}, (br.flags() & f::up));
}

int main(int argc, char* argv[]) {
    using f = sys::unshare_flag;
    sys::this_process::unshare(f::users | f::network);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
