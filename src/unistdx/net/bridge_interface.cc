#include <unistdx/net/bridge_interface>

void sys::bridge_interface::init() {
    this->_socket.call(fildes::operation::bridge_add, this->_name[0]);
}

void sys::bridge_interface::destroy() {
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
