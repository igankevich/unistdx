#include <vector>

#include <gtest/gtest.h>

#include <unistdx/base/log_message>
#include <unistdx/io/pipe>
#include <unistdx/ipc/process>
#include <unistdx/net/veth_interface>
#include <unistdx/test/config>

template <class ... Args>
inline void
log(const Args& ... args) {
    sys::log_message("tst", args...);
}


void test_bare() {
    std::vector<sys::veth_interface> veths;
    for (int i=0; i<10; ++i) {
        std::clog << "i=" << i << std::endl;
        veths.emplace_back("x" + std::to_string(i), "vx" + std::to_string(i));
    }
    for (auto& v : veths) {
        v.up();
        v.down();
        std::clog << "v.index()=" << v.index() << std::endl;
        std::clog << "v.peer().index()=" << v.peer().index() << std::endl;
        v.destroy();
    }
}

void test_unshare() {
    using f = sys::unshare_flag;
    sys::this_process::unshare(f::users | f::network);
    test_bare();
}

void test_clone_unshare() {
    sys::process child{[&] () {
        try {
            test_unshare();
            return 0;
        } catch (const std::exception& err) {
            log("error: _", err.what());
        }
        return 1;
    }, sys::process_flag::fork, 4096*10};
    EXPECT_EQ(0, child.wait().exit_code());
}

void test_clone() {
    using pf = sys::process_flag;
    sys::pipe pipe;
    pipe.in().unsetf(sys::open_flag::non_blocking);
    pipe.out().unsetf(sys::open_flag::non_blocking);
    sys::process child{[&] () {
        try {
            pipe.out().close();
            char ch;
            pipe.in().read(&ch, 1);
            test_bare();
            return 0;
        } catch (const std::exception& err) {
            std::clog << "err.what()=" << err.what() << std::endl;
        }
        return 1;
    }, pf::signal_parent | pf::unshare_users | pf::unshare_network, 4096*10};
    child.init_user_namespace();
    pipe.in().close();
    pipe.out().write("x", 1);
    EXPECT_EQ(0, child.wait().exit_code());
}

int main(int argc, char* argv[]) {
    #if defined(UNISTDX_TEST_SANITIZE_ADDRESS)
    std::exit(77);
    #endif
    test_clone_unshare();
    //test_clone();
    std::clog << "-\n";
    test_unshare();
    return 0;
}

