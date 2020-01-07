#include <unistdx/ipc/process>

#include <unistdx/base/make_object>

std::ostream&
sys::operator<<(std::ostream& out, const process& rhs) {
    return out << make_object("id", rhs.id(), "gid", rhs.group_id());
}

sys::fildes sys::get_namespace(const char* suffix, pid_type p) {
    char path[100];
    std::sprintf(path, "/proc/%d/ns/%s", p, suffix);
    return sys::fildes(path);
}
