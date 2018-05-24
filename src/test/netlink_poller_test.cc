#include <unistdx/net/netlink_poller>

#include <mutex>

#include <gtest/gtest.h>

TEST(NetlinkPoller, First) {
	sys::endpoint endp(RTMGRP_IPV4_IFADDR);
	sys::socket sock(
		sys::family_type::netlink,
		sys::socket_type::raw,
		NETLINK_ROUTE
	);
	sock.bind(endp);
	sys::event_poller poller;
	poller.insert({sock.get_fd(), sys::event::in});
	std::mutex mtx;
	poller.wait(mtx);
	for (const sys::epoll_event& ev : poller) {
		std::clog << "ev=" << ev << std::endl;
		if (ev.fd() == sock.get_fd()) {
			union {
				sys::netlink_header h;
				char bytes[4096];
			} h;
			ssize_t len = sock.read(h.bytes, sizeof(h.bytes));
			h.h.for_each_message<sys::ifaddr_message_header>(
				len,
				[&] (sys::ifaddr_message_header& hdr) {
					std::string action;
					if (hdr.new_address()) {
						action = "add";
					} else if (hdr.delete_address()) {
						action = "del";
					}
					if (hdr.new_address() || hdr.delete_address()) {
						sys::ifaddr_message* m = hdr.message();
						std::clog << "m->family()=" << m->family() << std::endl;
						std::clog << "m->prefix()=" << m->prefix() << std::endl;
						sys::ipv4_addr address{};
						m->for_each_attribute(
							len,
							[&] (sys::ifaddr_message::attr_type& attrs) {
							std::clog << "attrs.type()=" << attrs.type() << std::endl;
								if (attrs.type() == sys::ifaddr_attribute::address) {
									address = *attrs.data<sys::ipv4_addr>();
								}
							}
						);
						std::clog << action << ' ' << address << std::endl;
					}
				}
			);
		}
	}
}
