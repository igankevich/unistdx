#include "ipv6_address"

#include <istream>
#include <ostream>

#include <unistdx/base/ios_guard>
#include <unistdx/base/types>
#include <unistdx/bits/addr_parse>

std::ostream&
sys::operator<<(std::ostream& out, const ipv6_address& rhs) {
	std::ostream::sentry s(out);
	if (s) {
		typedef ipv6_address::hex_type hex_type;
		typedef std::ostream::char_type char_type;
		ios_guard g(out);
		out.setf(std::ios::hex, std::ios::basefield);
		std::copy(
			rhs.begin(),
			rhs.end(),
			intersperse_iterator<hex_type,char_type>(out, ':')
		);
	}
	return out;
}

std::istream&
sys::operator>>(std::istream& in, ipv6_address& rhs) {
	std::istream::sentry s(in);
	if (!s) {
		return in;
	}
	typedef ipv6_address::hex_type hex_type;
	typedef bits::Num<u16, u32> Hextet;
	ios_guard g(in);
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
		    Hextet h = 0;
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

