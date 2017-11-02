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
			h.h.for_each_message<sys::rtnetlink_header>(
				len,
				[&] (sys::rtnetlink_header& msg) {
					std::string action;
					if (msg.new_address()) {
						action = "add";
					} else if (msg.delete_address()) {
						action = "del";
					}
					if (msg.new_address() || msg.delete_address()) {
						sys::ifaddr_message* m = msg.get_ifaddr_message();
						sys::ipv4_addr address{};
						m->for_each_attribute(
							len,
							[&] (sys::rtattributes& attrs) {
								if (attrs.type() == IFA_ADDRESS) {
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
