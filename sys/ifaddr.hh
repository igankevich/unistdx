#ifndef SYS_IFADDR_HH
#define SYS_IFADDR_HH

#include <map>
#include <vector>
#include <chrono>
#include <cassert>
#include <algorithm>
#include <iterator>

#include <sys/check>
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

		inline constexpr
		ifaddr(const addr_type& addr, const addr_type& netmask) noexcept:
		_address(addr), _netmask(netmask)
		{}

		inline constexpr
		ifaddr(const addr_type& addr, const sys::prefix_type prefix) noexcept:
		_address(addr), _netmask(addr_type::from_prefix(prefix))
		{}

		inline constexpr ifaddr() noexcept = default;
		inline constexpr ifaddr(const ifaddr&) noexcept = default;
		inline constexpr ifaddr(ifaddr&&) noexcept = default;
		inline ifaddr& operator=(const ifaddr&) noexcept = default;

		inline constexpr const addr_type&
		address() const noexcept {
			return _address;
		}

		inline constexpr const addr_type&
		netmask() const noexcept {
			return _netmask;
		}

		inline sys::prefix_type
		prefix() const noexcept {
			return _netmask.to_prefix();
		}

		inline constexpr const addr_type&
		gateway() const noexcept {
			return addr_type(first());
		}

		inline constexpr rep_type
		position() const noexcept {
			return _address.position(_netmask);
		}

		inline constexpr iterator
		begin() const noexcept {
			return iterator(first());
		}

		inline constexpr iterator
		middle() const noexcept {
			return iterator(_address);
		}

		inline constexpr iterator
		end() const noexcept {
			return iterator(last());
		}

		inline constexpr rep_type
		count() const noexcept {
			return last() - first();
		}

		inline constexpr bool
		is_loopback() const noexcept {
			return _address[0] == traits_type::loopback_first_octet
				and _netmask == traits_type::loopback_mask();
		}

		inline constexpr bool
		is_widearea() const noexcept {
			return _netmask == traits_type::widearea_mask();
		}

		inline explicit constexpr
		operator bool() const noexcept {
			return static_cast<bool>(_address) and static_cast<bool>(_netmask);
		}

		inline constexpr bool
		operator !() const noexcept {
			return !operator bool();
		}

		inline constexpr bool
		operator==(const ifaddr& rhs) const noexcept {
			return _address == rhs._address && _netmask == rhs._netmask;
		}

		inline constexpr bool
		operator!=(const ifaddr& rhs) const noexcept {
			return !operator==(rhs);
		}

		inline friend std::ostream&
		operator<<(std::ostream& out, const ifaddr& rhs) {
			return out << rhs._address << Slash() << rhs._netmask.to_prefix();
		}

		inline friend std::istream&
		operator>>(std::istream& in, ifaddr& rhs) {
			sys::prefix_type prefix = 0;
			in >> rhs._address >> Slash() >> prefix;
			rhs._netmask = addr_type::from_prefix(prefix);
			return in;
		}

	private:

		inline constexpr rep_type
		first() const noexcept {
			return (addr_long() & mask_long()) + 1;
		}

		inline constexpr rep_type
		last() const noexcept {
			return (addr_long() & mask_long()) + (~mask_long());
		}

		inline constexpr const rep_type
		addr_long() const noexcept {
			return sys::to_host_format(_address.rep());
		}

		inline constexpr const rep_type
		mask_long() const noexcept {
			return sys::to_host_format(_netmask.rep());
		}

		addr_type _address;
		addr_type _netmask;
	};

}

#endif // SYS_IFADDR_HH
