#include <gtest/gtest.h>

#include <unistdx/net/hosts>

TEST(host, _) {
    using f = sys::host_flags;
    sys::hosts hosts;
    hosts.socket_type(sys::socket_type::stream);
    hosts.flags(f::canonical_name | f::all | f::v4_mapped);
    hosts.update("very-long-host-name-that-does-not-exist");
    EXPECT_EQ(0, std::distance(hosts.begin(),hosts.end()));
    /*
    hosts.update("m1.cmms", "ssh");
    std::clog << "num-addresses " << std::distance(hosts.begin(),hosts.end()) << std::endl;
    for (const auto& host : hosts) {
        if (host.canonical_name()) {
            std::clog << "host.canonical_name()=" << host.canonical_name() << std::endl;
        }
        std::clog << "host.family()=" << host.family() << std::endl;
        std::clog << "host.socket_type()=" << int(host.socket_type()) << std::endl;
        std::clog << "host.socket_address_size()=" << host.socket_address_size() << std::endl;
        std::clog << "host.socket_address()=" << host.socket_address() << std::endl;
        std::clog << "sys::host_name(host.socket_address())=" << sys::host_name(host.socket_address()) << std::endl;
    }
    std::clog << "sys::service_name(22)=" << sys::service_name(22) << std::endl;
    */
}
