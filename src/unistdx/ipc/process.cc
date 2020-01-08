#include <limits.h>

#include <unistdx/base/make_object>
#include <unistdx/ipc/process>

std::ostream&
sys::operator<<(std::ostream& out, const process& rhs) {
    return out << make_object("id", rhs.id(), "gid", rhs.group_id());
}

sys::fildes sys::get_namespace(const char* suffix, pid_type p) {
    char path[100];
    std::sprintf(path, "/proc/%d/ns/%s", p, suffix);
    return sys::fildes(path);
}

std::string sys::this_process::hostname() {
    std::string name;
    name.resize(HOST_NAME_MAX);
    UNISTDX_CHECK(::gethostname(&name[0], HOST_NAME_MAX));
    name[HOST_NAME_MAX-1] = 0;
    name.resize(std::string::traits_type::length(name.data()));
    return name;
}
