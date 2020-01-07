#include <unistdx/net/bridge_interface>

sys::bridge_interface::bridge_interface(const std::string& name): network_interface(name) {
    this->_socket.call(fildes::operation::bridge_add, this->_name[0]);
}

sys::bridge_interface::~bridge_interface() {
    this->down();
    this->_socket.call(fildes::operation::bridge_del, this->_name[0]);
}

void sys::bridge_interface::add(const sys::network_interface& iface) {
    network_interface_request req{};
    traits_type::copy(req.ifr_name, this->_name.data(), this->_name.size());
    req.ifr_ifindex = iface.index();
    this->_socket.call(fildes::operation::bridge_add_interface, req);
}

void sys::bridge_interface::remove(const sys::network_interface& iface) {
    network_interface_request req{};
    traits_type::copy(req.ifr_name, this->_name.data(), this->_name.size());
    req.ifr_ifindex = iface.index();
    this->_socket.call(fildes::operation::bridge_del_interface, req);
}
