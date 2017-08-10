#ifndef SYS_IFADDR_LIST_HH
#define SYS_IFADDR_LIST_HH

#include "ifaddrs_iterator.hh"
#include "ifaddr.hh"

namespace sys {

	struct ifaddr_list {

		typedef ifaddrs_type value_type;
		typedef ifaddrs_iterator iterator;
		typedef std::size_t size_type;

		inline
		ifaddr_list() {
			UNISTDX_CHECK(::getifaddrs(&this->_addrs));
		}

		inline
		~ifaddr_list() noexcept {
			if (this->_addrs) {
				::freeifaddrs(this->_addrs);
			}
		}

		inline iterator
		begin() noexcept {
			return iterator(this->_addrs);
		}

		inline iterator
		begin() const noexcept {
			return iterator(this->_addrs);
		}

		inline static constexpr iterator
		end() noexcept {
			return iterator();
		}

		inline bool
		empty() const noexcept {
			return this->begin() == this->end();
		}

		inline size_type
		size() const noexcept {
			return std::distance(this->begin(), this->end());
		}

	private:

		ifaddrs_type* _addrs = nullptr;

	};

	template<class Addr, class Result>
	inline void
	enumerate_ifaddrs(Result result) {
		typedef typename sys::ipaddr_traits<Addr> traits_type;
		sys::ifaddr_list addrs;
		std::for_each(
			addrs.begin(), addrs.end(),
			[&result] (const sys::ifaddrs_type& rhs) {
				if (rhs.ifa_addr and rhs.ifa_addr->sa_family == traits_type::family) {
					const Addr addr(*rhs.ifa_addr);
					const Addr netmask(*rhs.ifa_netmask);
					*result = ifaddr<Addr>(addr, netmask);
					++result;
				}
			}
		);
	}

}

#endif // SYS_IFADDR_LIST_HH
