#include "family"

namespace {

	typedef uint8_t raw_family_type;

	enum struct portable_family_type: raw_family_type {
		inet = 0,
		inet6 = 1
	};

	inline constexpr portable_family_type
	map_family_type(sys::family_type t) {
		return t == sys::family_type::inet6
		       ? portable_family_type::inet6
			   : portable_family_type::inet;
	};

	inline constexpr sys::family_type
	map_family_type(portable_family_type t) {
		return t == portable_family_type::inet6
		       ? sys::family_type::inet6
			   : sys::family_type::inet;
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
