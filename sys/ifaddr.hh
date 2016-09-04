#ifndef SYS_IFADDR_HH
#define SYS_IFADDR_HH

#include <map>
#include <vector>
#include <chrono>
#include <cassert>
#include <algorithm>
#include <iterator>

#include <sys/bits/check.hh>
#include "endpoint.hh"
#include "subnet_iterator.hh"

namespace sys {

	template<class Addr>
	class ifaddr {

		typedef sys::bits::Const_char<'/'> Slash;
		typedef typename sys::ipaddr_traits<Addr> traits_type;

	public:

		typedef Addr addr_type;
		typedef typename addr_type::rep_type rep_type;
		typedef subnet_iterator<addr_type> iterator;

		constexpr
		ifaddr(const addr_type& addr, const addr_type& netmask) noexcept:
		_address(addr), _netmask(netmask)
		{}

		constexpr
		ifaddr(const addr_type& addr, const sys::prefix_type prefix) noexcept:
		_address(addr), _netmask(addr_type::from_prefix(prefix))
		{}

		constexpr ifaddr() noexcept = default;
		constexpr ifaddr(const ifaddr&) noexcept = default;
		constexpr ifaddr(ifaddr&&) noexcept = default;
		ifaddr& operator=(const ifaddr&) noexcept = default;

		constexpr const addr_type&
		address() const noexcept {
			return _address;
		}

		constexpr const addr_type&
		netmask() const noexcept {
			return _netmask;
		}

		sys::prefix_type
		prefix() const noexcept {
			return _netmask.to_prefix();
		}

		constexpr const addr_type&
		gateway() const noexcept {
			return addr_type(first());
		}

		constexpr rep_type
		position() const noexcept {
			return _address.position(_netmask);
		}

		constexpr iterator
		begin() const noexcept {
			return iterator(first());
		}

		constexpr iterator
		middle() const noexcept {
			return iterator(_address);
		}

		constexpr iterator
		end() const noexcept {
			return iterator(last());
		}

		constexpr rep_type
		count() const noexcept {
			return last() - first();
		}

		constexpr bool
		is_loopback() const noexcept {
			return _address[0] == traits_type::loopback_first_octet
				and _netmask == traits_type::loopback_mask();
		}

		constexpr bool
		is_widearea() const noexcept {
			return _netmask == traits_type::widearea_mask();
		}

		explicit constexpr
		operator bool() const noexcept {
			return static_cast<bool>(_address) and static_cast<bool>(_netmask);
		}

		constexpr bool
		operator !() const noexcept {
			return !operator bool();
		}

		constexpr bool
		operator==(const ifaddr& rhs) const noexcept {
			return _address == rhs._address && _netmask == rhs._netmask;
		}

		constexpr bool
		operator!=(const ifaddr& rhs) const noexcept {
			return !operator==(rhs);
		}

		friend std::ostream&
		operator<<(std::ostream& out, const ifaddr& rhs) {
			return out << rhs._address << Slash() << rhs._netmask.to_prefix();
		}

		friend std::istream&
		operator>>(std::istream& in, ifaddr& rhs) {
			sys::prefix_type prefix = 0;
			in >> rhs._address >> Slash() >> prefix;
			rhs._netmask = addr_type::from_prefix(prefix);
			return in;
		}

	private:

		constexpr rep_type
		first() const noexcept {
			return (addr_long() & mask_long()) + 1;
		}

		constexpr rep_type
		last() const noexcept {
			return (addr_long() & mask_long()) + (~mask_long());
		}

		constexpr const rep_type
		addr_long() const noexcept {
			return sys::to_host_format(_address.rep());
		}

		constexpr const rep_type
		mask_long() const noexcept {
			return sys::to_host_format(_netmask.rep());
		}

		addr_type _address;
		addr_type _netmask;
	};

}

#endif // SYS_IFADDR_HH
