#include <unistdx/base/log_message>
#include <unistdx/base/sha1>
#include <unistdx/net/bytes>

#include <chrono>

std::string a = "aaaaaaaaaa";
sys::sha1 sha;

void
compute_sha1() {
    for (int i=0; i<100000; i++) {
        sha.put(a.data(), a.size());
    }
    sha.compute();
    sha.reset();
    //sys::log_message("bnch", "sha1=_", result);
}

int main() {
    using namespace std::chrono;
    typedef high_resolution_clock clock_type;
    for (int i=0; i<1000; ++i) {
        auto t0 = clock_type::now();
        compute_sha1();
        auto t1 = clock_type::now();
//		sys::log_message("bnch", "t=_", (t1-t0).count());
        std::clog << duration_cast<microseconds>(t1-t0).count() << '\n';
    }
    return 0;
}
