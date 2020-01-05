#include <unistdx/net/ipv4_address>

#include <istream>
#include <ostream>

#include <unistdx/base/types>
#include <unistdx/bits/addr_parse>

namespace {

    inline int
    num_digits(sys::ipv4_address::value_type rhs) noexcept {
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

    typedef sys::bits::Number<sys::u8, sys::u16> Octet;

}

std::ostream&
sys::operator<<(std::ostream& out, ipv4_address rhs) {
    using bits::Dot;
    const std::streamsize padding = out.width(0) - width(rhs);
    const bool pad_left = (out.flags() & std::ios_base::adjustfield) !=
                          std::ios_base::left;
    if (padding > 0 && pad_left) {
        pad_stream(out, padding);
    }
    out << Octet{rhs._octets[0]} << Dot{};
    out << Octet{rhs._octets[1]} << Dot{};
    out << Octet{rhs._octets[2]} << Dot{};
    out << Octet{rhs._octets[3]};
    if (padding > 0 && !pad_left) {
        pad_stream(out, padding);
    }
    return out;
}

std::istream&
sys::operator>>(std::istream& in, ipv4_address& rhs) {
    using bits::Dot;
    in >> Octet{rhs._octets[0]} >> Dot{};
    in >> Octet{rhs._octets[1]} >> Dot{};
    in >> Octet{rhs._octets[2]} >> Dot{};
    in >> Octet{rhs._octets[3]};
    if (in.fail()) {
        rhs.clear();
    }
    return in;
}
