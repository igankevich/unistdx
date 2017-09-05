#include "endpoint"

#include <cstring>
#include <istream>
#include <ostream>
#include <sstream>
#include <unistdx/base/ios_guard>
#include <unistdx/bits/addr_parse>

namespace {

	typedef sys::bits::Const_char<'['> Left_br;
	typedef sys::bits::Const_char<']'> Right_br;

	inline void
	init_addr(sys::sockinet4_type& addr4, sys::ipv4_addr a, sys::port_type p) {
		using namespace sys;
		addr4.sin_family = static_cast<sa_family_type>(family_type::inet);
		addr4.sin_addr = a;
		addr4.sin_port = to_network_format<port_type>(p);
	}

	inline void
	init_addr(
		sys::sockinet6_type& addr6,
		const sys::ipv6_addr& a,
		sys::port_type p
	) {
		using namespace sys;
		addr6.sin6_family = static_cast<sa_family_type>(family_type::inet6);
		addr6.sin6_addr = a;
		addr6.sin6_port = to_network_format<port_type>(p);
	}

}

std::ostream&
sys::operator<<(std::ostream& out, const endpoint& rhs) {
	using bits::Colon;
	std::ostream::sentry s(out);
	if (s) {
		if (rhs.family() == family_type::inet6) {
			port_type port = to_host_format<port_type>(rhs.port6());
			out << Left_br() << rhs.addr6() << Right_br()
			    << Colon() << port;
		} else if (rhs.family() == family_type::inet) {
			port_type port = to_host_format<port_type>(rhs.port4());
			out << rhs.addr4() << Colon() << port;
		} else if (rhs.family() == family_type::unix) {
			const char* unix_socket_path =
				rhs._bytes.begin() + sizeof(sa_family_t);
			if (!*unix_socket_path) {
				out << "\\0";
				++unix_socket_path;
			}
			out << unix_socket_path;
		}
	}
	return out;
}

std::istream&
sys::operator>>(std::istream& in, endpoint& rhs) {
	using bits::Colon;
	std::istream::sentry s(in);
	if (s) {
		ipv4_addr host;
		port_type port = 0;
		ios_guard g(in);
		in.unsetf(std::ios_base::skipws);
		std::streampos oldg = in.tellg();
		if (in >> host >> Colon() >> port) {
			init_addr(rhs._addr4, host, port);
		} else {
			in.clear();
			in.seekg(oldg);
			ipv6_addr host6;
			if (in >> Left_br() >> host6 >> Right_br() >> Colon() >>
			    port) {
				init_addr(rhs._addr6, host6, port);
			}
		}
	}
	return in;
}

sys::pstream&
sys::operator<<(pstream& out, const endpoint& rhs) {
	out << rhs.family();
	if (rhs.family() == family_type::inet6) {
		out << rhs.addr6() << make_bytes(rhs.port6());
	} else {
		out << rhs.addr4() << make_bytes(rhs.port4());
	}
	return out;
}

sys::pstream&
sys::operator>>(pstream& in, endpoint& rhs) {
	family_type fam;
	in >> fam;
	rhs._addr6.sin6_family = static_cast<sa_family_type>(fam);
	bytes<port_type> port;
	if (rhs.family() == family_type::inet6) {
		ipv6_addr addr;
		in >> addr >> port;
		rhs._addr6.sin6_addr = addr;
		rhs._addr6.sin6_port = port;
	} else {
		ipv4_addr addr;
		in >> addr >> port;
		rhs._addr4.sin_addr = addr;
		rhs._addr4.sin_port = port;
	}
	return in;
}

void
sys::endpoint::addr(const char* host, port_type p) {
	ipv4_addr a4;
	std::stringstream tmp(host);
	if (tmp >> a4) {
		init_addr(this->_addr4, a4, p);
	} else {
		tmp.clear();
		tmp.seekg(0);
		ipv6_addr a6;
		if (tmp >> a6) {
			init_addr(this->_addr6, a6, p);
		}
	}
}

sys::endpoint::endpoint(const char* unix_socket_path) noexcept:
_sockaddr{AF_UNIX, 0} {
	constexpr const int max_size = this->_bytes.size() - sizeof(sa_family_t) - 1;
	const char* p = unix_socket_path;
	int n = 0;
	if (!*unix_socket_path) {
		++p;
		++n;
	}
	n += std::strlen(p);
	n = std::min(max_size, n);
	std::memcpy(
		this->_bytes.begin() + sizeof(sa_family_t),
		unix_socket_path,
		n
	);
	this->_bytes[n] = 0;
}

