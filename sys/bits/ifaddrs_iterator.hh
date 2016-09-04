#ifndef SYS_BITS_IFADDRS_ITERATOR_HH
#define SYS_BITS_IFADDRS_ITERATOR_HH

#include <ifaddrs.h>

namespace sys {

	typedef struct ::ifaddrs ifaddrs_type;

	struct ifaddrs_iterator: public std::iterator<std::input_iterator_tag, ifaddrs_type> {

		typedef ifaddrs_type* pointer;
		typedef const ifaddrs_type* const_pointer;
		typedef ifaddrs_type& reference;
		typedef const ifaddrs_type& const_reference;

		explicit constexpr
		ifaddrs_iterator(pointer rhs) noexcept: _ifa(rhs) {}

		constexpr
		ifaddrs_iterator() noexcept = default;

		inline
		~ifaddrs_iterator() = default;

		constexpr
		ifaddrs_iterator(const ifaddrs_iterator&) noexcept = default;

		inline ifaddrs_iterator&
		operator=(const ifaddrs_iterator&) noexcept = default;

		constexpr bool
		operator==(const ifaddrs_iterator& rhs) const noexcept {
			return this->_ifa == rhs._ifa;
		}

		constexpr bool
		operator!=(const ifaddrs_iterator& rhs) const noexcept {
			return !operator==(rhs);
		}

		inline reference
		operator*() noexcept {
			return *this->_ifa;
		}

		constexpr const_reference
		operator*() const noexcept {
			return *this->_ifa;
		}

		inline pointer
		operator->() noexcept {
			return this->_ifa;
		}

		constexpr const_pointer
		operator->() const noexcept {
			return this->_ifa;
		}

		inline ifaddrs_iterator&
		operator++() noexcept {
			this->next(); return *this;
		}

		inline ifaddrs_iterator
		operator++(int) noexcept {
			ifaddrs_iterator tmp(*this); this->next(); return tmp;
		}

	private:

		inline void
		next() noexcept {
			this->_ifa = this->_ifa->ifa_next;
		}

		pointer _ifa = nullptr;
	};


}

#endif // SYS_BITS_IFADDRS_ITERATOR_HH
