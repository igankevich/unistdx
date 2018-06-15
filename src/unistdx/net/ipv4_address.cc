#include "ipv4_address"

#include <istream>
#include <ostream>

#include <unistdx/base/types>
#include <unistdx/bits/addr_parse>

namespace {

	inline int
	num_digits(sys::ipv4_address::oct_type rhs) noexcept {
		return rhs >= 100 ? 3 :
		       rhs >= 10  ? 2 :
		       1;
	}

	inline std::streamsize
	width(sys::ipv4_address a) noexcept {
		return num_digits(a[0]) +
		       num_digits(a[1]) +
		       num_digits(a[2]) +
		       num_digits(a[3]) +
		       3;
	}

	inline void
	pad_stream(std::ostream& out, const std::streamsize padding) {
		const char ch = out.fill();
		for (std::streamsize i=0; i<padding; ++i) {
			out.put(ch);
		}
	}

}

std::ostream&
sys::operator<<(std::ostream& out, ipv4_address rhs) {
	using bits::Dot;
	const std::streamsize padding = out.width(0) - width(rhs);
	const bool pad_left = (out.flags() & std::ios_base::adjustfield) !=
	                      std::ios_base::left;
	if (padding > 0 and pad_left) {
		pad_stream(out, padding);
	}
	out << ((rhs.addr >> 0)  & UINT32_C(0xff)) << Dot()
	    << ((rhs.addr >> 8)  & UINT32_C(0xff)) << Dot()
	    << ((rhs.addr >> 16) & UINT32_C(0xff)) << Dot()
	    << ((rhs.addr >> 24) & UINT32_C(0xff));
	if (padding > 0 and not pad_left) {
		pad_stream(out, padding);
	}
	return out;
}

std::istream&
sys::operator>>(std::istream& in, ipv4_address& rhs) {
	typedef bits::Num<u8, u32> Octet;
	using bits::Dot;
	Octet o1, o2, o3, o4;
	in >> o1 >> Dot() >> o2 >> Dot() >> o3 >> Dot() >> o4;
	if (!in.fail()) {
		rhs.addr = ipv4_address::from_octets(o1, o2, o3, o4);
	}
	return in;
}

