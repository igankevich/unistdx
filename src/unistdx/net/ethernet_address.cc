#include <iomanip>
#include <istream>
#include <ostream>

#include <unistdx/base/ios_guard>
#include <unistdx/bits/addr_parse>
#include <unistdx/net/ethernet_address>

namespace {

	inline void
	pad_stream(std::ostream& out, const std::streamsize padding) {
		const char ch = out.fill();
		for (std::streamsize i=0; i<padding; ++i) {
			out.put(ch);
		}
	}

}

std::ostream&
sys::operator<<(std::ostream& out, const ethernet_address& rhs) {
	typedef ethernet_address::size_type size_type;
	ios_guard g(out);
	std::streamsize padding = out.width(0) - 17;
	bool pad_left = (out.flags() & std::ios::adjustfield) != std::ios::left;
	if (padding > 0 && pad_left) {
		out.fill(' ');
		pad_stream(out, padding);
	}
	out.fill('0');
	out.setf(std::ios::hex, std::ios::basefield);
	out << std::setw(2) << static_cast<u32>(rhs[0]);
	for (size_type i=1; i<ethernet_address::size(); ++i) {
		out << ':' << std::setw(2) << static_cast<u32>(rhs[i]);
	}
	if (padding > 0 && !pad_left) {
		out.fill(' ');
		pad_stream(out, padding);
	}
	return out;
}

std::istream&
sys::operator>>(std::istream& in, ethernet_address& rhs) {
	typedef bits::Num<ethernet_address::value_type, u32> Octet;
	typedef ethernet_address::size_type size_type;
	using bits::Colon;
	ios_guard g(in);
	in.setf(std::ios::hex, std::ios::basefield);
	Octet octets[ethernet_address::size()];
	in >> octets[0];
	for (size_type i=1; i<ethernet_address::size(); ++i) {
		in >> Colon() >> std::hex >> octets[i];
	}
	if (!in.fail()) {
		for (size_type i=0; i<ethernet_address::size(); ++i) {
			rhs[i] = octets[i];
		}
	}
	return in;
}
