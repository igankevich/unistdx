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
	poller.insert({sock.fd(), sys::event::in});
	std::mutex mtx;
	poller.wait_for(mtx, std::chrono::seconds(1));
	for (const sys::epoll_event& ev : poller) {
		std::clog << "ev=" << ev << std::endl;
		if (ev.fd() == sock.fd()) {
			sys::ifaddr_message_container cont;
			cont.read(sock);
			for (sys::ifaddr_message_header& hdr : cont) {
				std::string action;
				if (hdr.new_address()) {
					action = "add";
				} else if (hdr.delete_address()) {
					action = "del";
				}
				if (hdr.new_address() || hdr.delete_address()) {
					sys::ifaddr_message* m = hdr.message();
					sys::ipv4_addr address;
					for (auto& attr : m->attributes(cont.length())) {
						if (attr.type() == sys::ifaddr_attribute::address) {
							address = *attr.data<sys::ipv4_addr>();
						}
					}
					std::clog << action << ' ' << address << std::endl;
				}
			}
		}
	}
}
