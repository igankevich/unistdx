#include <unistdx/ipc/process_status>

#include <ostream>

std::ostream&
sys::operator<<(std::ostream& out, const process_status& rhs) {
    out << "pid=" << rhs.pid() << ',';
    out << "status=" << rhs.status_string() << ',';
    if (rhs.exited()) {
        out << "exit_code=" << rhs.exit_code();
    } else {
        out << "signal=" << rhs.term_signal();
    }
    return out;
}
