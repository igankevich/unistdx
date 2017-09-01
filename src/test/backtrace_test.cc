#include <unistdx/io/fdstream>
#include <unistdx/io/pipe>
#include <unistdx/ipc/process>
#include <unistdx/ipc/signal>
#include <unistdx/util/backtrace>

#include <chrono>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <unistd.h>

#if defined(__GNUC__)
#define NO_INLINE [[gnu::noinline]]
#else
#define NO_INLINE
#endif

enum struct Test_type {
	Signal,
	Terminate,
	Signal_thread,
	Terminate_thread
};

std::istream&
operator>>(std::istream& in, Test_type& rhs) {
	std::string s;
	in >> s;
	if (s == "signal") {
		rhs = Test_type::Signal;
	} else if (s == "signal_thread") {
		rhs = Test_type::Signal_thread;
	} else if (s == "terminate") {
		rhs = Test_type::Terminate;
	} else if (s == "terminate_thread") {
		rhs = Test_type::Terminate_thread;
	} else {
		throw std::invalid_argument("bad test type");
	}
	return in;
}

void
print_error() {
	sys::pipe p;
	p.in().unsetf(sys::open_flag::non_blocking);
	p.out().unsetf(sys::open_flag::non_blocking);
	sys::backtrace(p.out().get_fd());
	p.out().close();
	bool func1_found = false;
	bool func2_found = false;
	sys::ifdstream str(std::move(p.in()));
	std::string line;
	while (std::getline(str, line)) {
		if (line.find("func1") != std::string::npos) {
			func1_found = true;
		} else if (line.find("func2") != std::string::npos) {
			func2_found = true;
		}
	}
	int ret = EXIT_SUCCESS;
	if (!func1_found) {
		std::cerr << "func1 not found" << std::endl;
	}
	if (!func2_found) {
		std::cerr << "func2 not found" << std::endl;
	}
	if (!func1_found && !func2_found) {
		ret = EXIT_FAILURE;
		sys::backtrace(STDERR_FILENO);
	}
	std::exit(ret);
}

void
print_error_signal(int) {
	print_error();
}

NO_INLINE void
func2(Test_type type) {
	if (type == Test_type::Terminate) {
		std::set_terminate(print_error);
		throw std::runtime_error("...");
	} else if (type == Test_type::Signal) {
		using namespace sys::this_process;
		bind_signal(sys::signal::segmentation_fault, print_error_signal);
		send(sys::signal::segmentation_fault);
	} else if (type == Test_type::Terminate_thread) {
		std::set_terminate(print_error);
		sys::backtrace(2);
		throw std::runtime_error("...");
	}
}

NO_INLINE void
func1(Test_type type) {
	func2(type);
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		throw std::invalid_argument("bad command line arguments");
	}
	Test_type type = Test_type::Terminate;
	std::stringstream str(argv[1]);
	str >> type;
	if (type == Test_type::Terminate_thread || type == Test_type::Signal_thread) {
		std::thread t(
			[type] () {
				func1(type);
			}
		);
		if (t.joinable()) {
			t.join();
		}
		using namespace std::this_thread;
		using namespace std::chrono;
		sleep_for(milliseconds(123));
	} else {
		func1(type);
	}
	return 1;
}
