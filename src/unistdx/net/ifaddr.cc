#include "ifaddr"

#include <unistdx/bits/addr_parse>

namespace {

	typedef sys::bits::Const_char<'/'> Slash;

}

template <class Addr>
std::ostream&
sys::operator<<(std::ostream& out, const ifaddr<Addr>& rhs) {
	return out << rhs._address << Slash() << rhs._netmask.to_prefix();
}

template <class Addr>
std::istream&
sys::operator>>(std::istream& in, ifaddr<Addr>& rhs) {
	sys::prefix_type prefix = 0;
	in >> rhs._address >> Slash() >> prefix;
	rhs._netmask = Addr::from_prefix(prefix);
	return in;
}

template <class Addr>
sys::pstream&
sys::operator<<(pstream& out, const ifaddr<Addr>& rhs) {
	return out << rhs._address << rhs._netmask;
}

template <class Addr>
sys::pstream&
sys::operator>>(pstream& in, ifaddr<Addr>& rhs) {
	return in >> rhs._address >> rhs._netmask;
}

template std::ostream&
sys::operator<<(std::ostream& out, const ifaddr<sys::ipv4_addr>& rhs);

template std::ostream&
sys::operator<<(std::ostream& out, const ifaddr<sys::ipv6_addr>& rhs);

template std::istream&
sys::operator>>(std::istream& in, ifaddr<ipv4_addr>& rhs);

template std::istream&
sys::operator>>(std::istream& in, ifaddr<ipv6_addr>& rhs);

template sys::pstream&
sys::operator<<(pstream& out, const ifaddr<ipv4_addr>& rhs);

template sys::pstream&
sys::operator<<(pstream& out, const ifaddr<ipv6_addr>& rhs);

template sys::pstream&
sys::operator>>(pstream& in, ifaddr<ipv4_addr>& rhs);

template sys::pstream&
sys::operator>>(pstream& in, ifaddr<ipv6_addr>& rhs);
