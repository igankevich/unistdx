#include <algorithm>
#include <locale>
#include <ostream>

#include <unistdx/base/uint128>

namespace {

    constexpr static char alphabet[] = {
        '0','1','2','3','4','5','6','7','8','9',
        'a','b','c','d','e','f'};

    std::string
    u128_to_string(const sys::u128& rhs, int radix, size_t width, char fill, bool left,
                   bool plus) {
        std::string result;
        result.reserve(std::max(size_t(43),width));
        sys::u128 r(radix);
        sys::u128_div d{rhs,0};
        while (d.quot) {
            d = sys::div(d.quot, r);
            result += alphabet[d.rem.b()];
        }
        if (result.empty()) { result += '0'; }
        if (plus) { result += '+'; }
        std::reverse(result.begin(), result.end());
        auto s = result.size();
        if (s < width) { result.insert(left ? s : 0, width-s, fill); }
        return result;
    }

}

std::string sys::to_string(const u128& rhs) {
    return u128_to_string(rhs, 10, 0, ' ', false, false);
}

std::ostream& sys::operator<<(std::ostream& out, const u128& rhs) {
    int radix = 10;
    switch (out.flags() & std::ios::basefield) {
        case std::ios::oct: radix = 8; break;
        case std::ios::hex: radix = 16; break;
        default: break;
    }
    auto&& s = u128_to_string(rhs, radix, out.width(), out.fill(),
                              out.flags() & std::ios::left,
                              out.flags() & std::ios::showpos);
    if ((out.flags() & std::ios::uppercase) && (out.flags() & std::ios::hex)) {
        for (auto& ch : s) { ch = std::toupper(ch, out.getloc()); }
    }
    return out << s;
}

namespace std {
    constexpr int numeric_limits<sys::u128>::digits;
}
