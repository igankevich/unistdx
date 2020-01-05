#include <limits>
#include <type_traits>

#include <gtest/gtest.h>

#include <unistdx/ipc/process>
#include <unistdx/net/network_interface>

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

int main(int argc, char* argv[]) {
    using f = sys::unshare_flag;
    sys::this_process::unshare(f::users | f::network);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
