#include <unistdx/ipc/process_group>

#include <unistdx/base/log_message>
#include <unistdx/it/intersperse_iterator>

int
sys::process_group::wait() {
    int ret = 0;
    for (process& p : this->_procs) {
        sys::process_status x = p.wait();
        #ifndef NDEBUG
        log_message("sys", "process _ terminated with status _", p, x);
        #endif
        ret |= x.exit_code() | signal_type(x.term_signal());
    }
    return ret;
}

std::ostream&
sys::operator<<(std::ostream& out, const sys::process_group& rhs) {
    out << "{gid=" << rhs._gid << ",[";
    std::copy(rhs._procs.begin(), rhs._procs.end(),
        intersperse_iterator<process>(out, ","));
    out << "]}";
    return out;
}
