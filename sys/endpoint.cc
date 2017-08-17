#include "endpoint"

namespace {

	inline int
	num_digits(sys::ipv4_addr::oct_type rhs) noexcept {
		return rhs >= 100 ? 3 :
		       rhs >= 10  ? 2 :
		       1;
	}

	inline std::streamsize
	width(sys::ipv4_addr a) noexcept {
		return num_digits(a[0]) +
		       num_digits(a[1]) +
		       num_digits(a[2]) +
		       num_digits(a[3]) +
		       3;
	}

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
sys::operator<<(std::ostream& out, ipv4_addr rhs) {
	using bits::Dot;
	const std::streamsize padding = out.width(0) - width(rhs);
	const bool pad_left = (out.flags() & std::ios_base::adjustfield) !=
	                      std::ios_base::left;
	if (padding > 0 and pad_left) {
		bits::pad_stream(out, padding);
	}
	out << ((rhs.addr >> 0)  & UINT32_C(0xff)) << Dot()
	    << ((rhs.addr >> 8)  & UINT32_C(0xff)) << Dot()
	    << ((rhs.addr >> 16) & UINT32_C(0xff)) << Dot()
	    << ((rhs.addr >> 24) & UINT32_C(0xff));
	if (padding > 0 and not pad_left) {
		bits::pad_stream(out, padding);
	}
	return out;
}

std::istream&
sys::operator>>(std::istream& in, ipv4_addr& rhs) {
	using bits::Dot; using bits::Octet;
	Octet o1, o2, o3, o4;
	in >> o1 >> Dot() >> o2 >> Dot() >> o3 >> Dot() >> o4;
	if (!in.fail()) {
		rhs.addr = ipv4_addr::from_octets(o1, o2, o3, o4);
	}
	return in;
}

std::ostream&
sys::operator<<(std::ostream& out, const ipv6_addr& rhs) {
	std::ostream::sentry s(out);
	if (s) {

		typedef ipv6_addr::hex_type hex_type;
		typedef std::ostream::char_type char_type;
		stdx::ios_guard g(out);
		out.setf(std::ios::hex, std::ios::basefield);
		std::copy(
			rhs.begin(),
			rhs.end(),
			stdx::intersperse_iterator<hex_type,char_type>(out, ':')
		);
	}
	return out;
}

std::istream&
sys::operator>>(std::istream& in, ipv6_addr& rhs) {
	std::istream::sentry s(in);
	if (!s) {
		return in;
	}

	typedef ipv6_addr::hex_type hex_type;
	stdx::ios_guard g(in);
	in.setf(std::ios::hex, std::ios::basefield);
	int field_no = 0;
	int zeros_field = -1;
	std::for_each(
		rhs.begin(),
		rhs.end(),
		[&field_no,&in,&zeros_field,&rhs] (hex_type& field) {
		    if (in.fail()) return;
		    // compressed notation
		    if (in.peek() == ':') {
		        in.get();
		        if (field_no == 0) {
		            in >> bits::Colon();
				}
		        if (zeros_field != -1) {
		            in.setstate(std::ios::failbit);
				} else {
		            zeros_field = field_no;
				}
			}
		    bits::Hextet h = 0;
		    if (in >> h) {
		        char ch = in.peek();
		        // if prefixed with ::ffff:
		        if (field_no >= 1 && rhs._hextets[0] == 0xffff && zeros_field ==
		            0) {
		            in >> bits::Dot();
				} else {
		            in >> bits::Colon();
				}
		        // put back the first character after the address
		        if (in.fail()) {
		            in.clear();
		            in.putback(ch);
				}
		        field = h;
		        ++field_no;
			}
		}
	);
	// push fields after :: towards the end
	if (zeros_field != -1) {
		in.clear();
		auto zeros_start = rhs.begin() + zeros_field;
		auto zeros_end = rhs.end() - (field_no - zeros_field);
		std::copy(zeros_start, rhs.begin() + field_no, zeros_end);
		std::fill(zeros_start, zeros_end, 0);
	}
	if (in.fail()) {
		std::fill(rhs.begin(), rhs.end(), 0);
	} else {
		// we do not need to change byte order here
		// rhs._bytes.to_network_format();
	}
	return in;
}

std::ostream&
sys::operator<<(std::ostream& out, const endpoint& rhs) {
	using bits::Left_br; using bits::Right_br;
	using bits::Colon;
	std::ostream::sentry s(out);
	if (s) {
		if (rhs.family() == family_type::inet6) {
			port_type port = to_host_format<port_type>(rhs.port6());
			out << Left_br() << rhs.addr6() << Right_br()
			    << Colon() << port;
		} else {
			port_type port = to_host_format<port_type>(rhs.port4());
			out << rhs.addr4() << Colon() << port;
		}
	}
	return out;
}

std::istream&
sys::operator>>(std::istream& in, endpoint& rhs) {
	using bits::Left_br; using bits::Right_br;
	using bits::Colon;
	std::istream::sentry s(in);
	if (s) {
		ipv4_addr host;
		port_type port = 0;
		stdx::ios_guard g(in);
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

sys::packetstream&
sys::operator<<(packetstream& out, const endpoint& rhs) {
	out << rhs.family();
	if (rhs.family() == family_type::inet6) {
		out << rhs.addr6() << make_bytes(rhs.port6());
	} else {
		out << rhs.addr4() << make_bytes(rhs.port4());
	}
	return out;
}

sys::packetstream&
sys::operator>>(packetstream& in, endpoint& rhs) {
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
