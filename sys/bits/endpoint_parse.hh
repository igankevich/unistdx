#ifndef SYS_BITS_ENDPOINT_PARSE_HH
#define SYS_BITS_ENDPOINT_PARSE_HH

#include <limits>

#include <sys/net/pstream>

namespace sys {

	namespace bits {

		template<unsigned int radix, class Ch>
		constexpr Ch
		to_int(Ch ch) noexcept {
			return radix == 16 && ch >= 'a' ? ch-'a'+10 : ch-'0';
		}

		template<char C>
		struct Const_char {
			friend std::ostream&
			operator<<(std::ostream& out, Const_char) {
				return out << C;
			}
			friend std::istream&
			operator>>(std::istream& in, Const_char) {
				if (in.get() != C) in.setstate(std::ios::failbit);
				return in;
			}
		};

		template<class Base, class Rep>
		struct Num {
			constexpr Num(): n(0) {}
			constexpr Num(Rep x): n(x) {}
			friend std::ostream&
			operator<<(std::ostream& out, Num rhs) {
				return out << rhs.n;
			}
			friend std::istream&
			operator>>(std::istream& in, Num& rhs) {
				in >> rhs.n;
				if (rhs.n > std::numeric_limits<Base>::max()) {
					in.setstate(std::ios::failbit);
				}
				return in;
			}
			constexpr operator Rep() const { return n; }
			constexpr Rep rep() const { return n; }
		private:
			Rep n;
		};

		typedef Const_char<':'> Colon;
		typedef Const_char<'.'> Dot;
		typedef Const_char<'['> Left_br;
		typedef Const_char<']'> Right_br;
		typedef Num<uint16_t, uint32_t> Hextet;
		typedef Num<uint8_t, uint32_t> Octet;

/*
		template<unsigned int radix, class Ch>
		constexpr Ch
		to_int(Ch ch) {
			return radix == 16 && ch >= 'a' ? ch-'a'+10 : ch-'0';
		}
*/

		template<class addr4_type>
		constexpr addr4_type
		shift_octet(addr4_type octet, addr4_type nshift) {
			return
				nshift == 0  ? ((octet << nshift) & UINT32_C(0xff)) :
				nshift == 8  ? ((octet << nshift) & UINT32_C(0xff00)) :
				nshift == 16 ? ((octet << nshift) & UINT32_C(0xff0000)) :
				nshift == 24 ? ((octet << nshift) & UINT32_C(0xff000000)) :
				0;
		}

		template<class addr4_type, addr4_type radix=10, class It>
		constexpr addr4_type
		do_parse_ipv4_addr(It first, It last,
			addr4_type val=0,
			addr4_type octet=0,
			addr4_type nshift=0,
			addr4_type ndots=0) {
			return ndots > 3 || octet > 255 ? 0 :
				first == last ?  (val | shift_octet(octet, nshift)) :
				*first == '.' ?  do_parse_ipv4_addr<addr4_type>(first+1, last,
					val | shift_octet(octet, nshift),
					0, nshift + 8, ndots + 1) :
				(*first >= '0' && *first <= '9') ?
				do_parse_ipv4_addr<addr4_type>(first+1, last, val,
					octet*radix + to_int<radix>(*first),
					nshift, ndots) :
				0;
		}

		typedef uint8_t raw_family_type;
		typedef ::sa_family_t legacy_family_type;

		enum struct family_type: legacy_family_type {
			inet = AF_INET,
			inet6 = AF_INET6
		};

		enum struct portable_family_type: raw_family_type {
			inet = 0,
			inet6 = 1
		};

		constexpr portable_family_type
		map_family_type(family_type t) {
			return t == family_type::inet6
				? portable_family_type::inet6
				: portable_family_type::inet;
		};

		constexpr family_type
		map_family_type(portable_family_type t) {
			return t == portable_family_type::inet6
				? family_type::inet6
				: family_type::inet;
		};

		inline pstream&
		operator<<(pstream& out, family_type rhs) {
			return out << static_cast<raw_family_type>(map_family_type(rhs));
		}

		inline pstream&
		operator>>(pstream& in, family_type& rhs) {
			raw_family_type raw;
			in >> raw;
			rhs = map_family_type(static_cast<portable_family_type>(raw));
			return in;
		}

		inline void
		pad_stream(std::ostream& out, const std::streamsize padding) {
			const char ch = out.fill();
			for (std::streamsize i=0; i<padding; ++i) {
				out.put(ch);
			}
		}

	}

}

#endif // SYS_BITS_ENDPOINT_PARSE_HH
