#include <unistdx/ipc/process_group>

int
sys::process_group::wait() {
    int ret = 0;
    for (auto& p : this->_procs) {
        auto x = p.wait();
        ret |= x.exit_code() | signal_type(x.term_signal());
    }
    return ret;
}
