#include <unistdx/net/netlink_poller>

const char*
sys::to_string(ifaddr_attribute rhs) {
    switch (rhs) {
        case ifaddr_attribute::unspecified: return "unspecified";
        case ifaddr_attribute::address: return "interface_address";
        case ifaddr_attribute::local_address: return "local_address";
        case ifaddr_attribute::interface_name: return "interface_name";
        case ifaddr_attribute::broadcast_address: return "broadcast_address";
        case ifaddr_attribute::anycast_address: return "anycast_address";
        case ifaddr_attribute::address_info: return "address_info";
        case ifaddr_attribute::multicast_address: return "multicast_address";
        case ifaddr_attribute::flags: return "flags";
        default: return "unknown";
    }
}

std::ostream&
sys::operator<<(std::ostream& out, const ifaddr_attribute& rhs) {
    return out << to_string(rhs);
}
