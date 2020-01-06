#include <unistdx/ipc/process>

#include <unistdx/base/make_object>

std::ostream&
sys::operator<<(std::ostream& out, const process& rhs) {
    return out << make_object("id", rhs.id(), "gid", rhs.group_id());
}

#if defined(UNISTDX_HAVE_SETNS)
sys::fildes sys::this_process::get_namespace(const char* suffix) {
    char path[100];
    std::sprintf(path, "/proc/%d/ns/%s", id(), suffix);
    return sys::fildes(path);
}
#endif
