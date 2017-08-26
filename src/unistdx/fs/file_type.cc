#include "file_type"

#include <ostream>

namespace {

	inline char
	to_char(sys::file_type rhs) noexcept {
		switch (rhs) {
			case sys::file_type::regular: return '-';
			case sys::file_type::directory: return 'd';
			case sys::file_type::block_device: return 'b';
			case sys::file_type::character_device: return 'c';
			case sys::file_type::pipe: return 'p';
			case sys::file_type::socket: return 's';
			case sys::file_type::symbolic_link: return 'l';
			default: return '?';
		}
	}

}

std::ostream&
sys::operator<<(std::ostream& out, const file_type& rhs) {
	return out << to_char(rhs);
}

