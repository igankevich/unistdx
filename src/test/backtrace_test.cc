#include <unistdx/util/backtrace>
#include <unistdx/io/pipe>
#include <unistdx/io/fdstream>
#include <unistdx/ipc/process>
#include <unistdx/ipc/signal>
#include <stdexcept>
#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>

enum struct Test_type {
	Signal,
	Terminate
};

std::istream&
operator>>(std::istream& in, Test_type& rhs) {
	std::string s;
	in >> s;
	if (s == "signal") {
		rhs = Test_type::Signal;
	} else if (s == "terminate") {
		rhs = Test_type::Terminate;
	} else {
		throw std::invalid_argument("bad test type");
	}
	return in;
}

void
print_error() {
	sys::pipe p;
	p.in().unsetf(sys::fildes::non_blocking);
	p.out().unsetf(sys::fildes::non_blocking);
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
	if (!func1_found) {
		std::cerr << "func1 not found" << std::endl;
	}
	if (!func2_found) {
		std::cerr << "func2 not found" << std::endl;
	}
	if (!func1_found || !func2_found) {
		sys::backtrace(STDERR_FILENO);
	}
	std::exit(0);
}

void
print_error_signal(int) {
	print_error();
}

void
func2() {
	throw std::runtime_error("...");
}

void
func1() {
	func2();
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		throw std::invalid_argument("bad command line arguments");
	}
	Test_type type = Test_type::Terminate;
	std::stringstream str(argv[1]);
	str >> type;
	if (type == Test_type::Terminate) {
		std::set_terminate(print_error);
	} else if (type == Test_type::Signal) {
		using namespace sys::this_process;
		bind_signal(sys::signal::segmentation_fault, print_error_signal);
		send(sys::signal::segmentation_fault);
	}
	func1();
	return 1;
}
