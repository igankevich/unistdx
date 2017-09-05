#include "family"

namespace {

	typedef uint8_t raw_family_type;

	enum struct portable_family_type: raw_family_type {
		inet = 0,
		inet6 = 1,
		unix = 2
	};

	inline portable_family_type
	map_family_type(sys::family_type t) {
		switch (t) {
			case sys::family_type::inet: return portable_family_type::inet;
			case sys::family_type::inet6: return portable_family_type::inet6;
			case sys::family_type::unix: return portable_family_type::unix;
			default: return portable_family_type(0);
		}
	};

	inline sys::family_type
	map_family_type(portable_family_type t) {
		switch (t) {
			case portable_family_type::inet: return sys::family_type::inet;
			case portable_family_type::inet6: return sys::family_type::inet6;
			case portable_family_type::unix: return sys::family_type::unix;
			default: return sys::family_type(0);
		}
	};

}

sys::pstream&
sys::operator<<(pstream& out, family_type rhs) {
	return out << static_cast<raw_family_type>(map_family_type(rhs));
}

sys::pstream&
sys::operator>>(pstream& in, family_type& rhs) {
	raw_family_type raw;
	in >> raw;
	rhs = map_family_type(static_cast<portable_family_type>(raw));
	return in;
}
