#include <unistdx/fs/file_status>

std::ostream&
sys::operator<<(std::ostream& out, const file_status& rhs) {
    return out << rhs.type() << rhs.mode() << ' '
        << rhs.owner() << ':' << rhs.group() << ' '
        << rhs.size();
}
