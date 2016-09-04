#ifndef SYS_SUBNET_ITERATOR_HH
#define SYS_SUBNET_ITERATOR_HH

#include <iterator>
#include "network_format.hh"

namespace sys {

	template<class Address>
	struct subnet_iterator:
	public std::iterator<std::random_access_iterator_tag,Address>
	{
		typedef Address addr_type;
		typedef typename addr_type::rep_type rep_type;
		typedef std::iterator<std::random_access_iterator_tag,Address> base_type;
		using typename base_type::difference_type;

		subnet_iterator() = default;
		subnet_iterator(const subnet_iterator&) = default;
		subnet_iterator(subnet_iterator&&) = default;
		~subnet_iterator() = default;
		subnet_iterator& operator=(const subnet_iterator&) = default;

		explicit constexpr
		subnet_iterator(rep_type idx) noexcept:
		_idx(idx),
		_addr(make_addr())
		{}

		explicit constexpr
		subnet_iterator(const addr_type& addr) noexcept:
		_idx(sys::to_host_format(addr.rep())),
		_addr(addr)
		{}

		constexpr bool
		operator==(const subnet_iterator& rhs) const noexcept {
			return _idx == rhs._idx;
		}

		constexpr bool
		operator!=(const subnet_iterator& rhs) const noexcept {
			return !operator==(rhs);
		}

		constexpr const addr_type&
		operator*() const noexcept {
			return _addr;
		}

		constexpr const addr_type*
		operator->() const noexcept {
			return &_addr;
		}

		subnet_iterator&
		operator++() noexcept {
			++_idx;
			_addr = make_addr();
			return *this;
		}

		subnet_iterator&
		operator++(int) noexcept {
			subnet_iterator tmp(*this);
			operator++();
			return tmp;
		}

		constexpr subnet_iterator
		operator+(difference_type rhs) const noexcept {
			return subnet_iterator(_idx + rhs);
		}

		constexpr subnet_iterator
		operator-(difference_type rhs) const noexcept {
			return subnet_iterator(_idx - rhs);
		}

		constexpr difference_type
		operator-(const subnet_iterator& rhs) const noexcept {
			return rhs._idx - _idx;
		}

	private:

		addr_type
		make_addr() const noexcept {
			return addr_type(sys::to_network_format(_idx));
		}

		rep_type _idx;
		addr_type _addr;

	};

}

#endif // SYS_SUBNET_ITERATOR_HH
