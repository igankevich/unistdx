#include <algorithm>
#include <iterator>
#include <string>

#include <unistdx/it/queue_popper>
#include <unistdx/it/queue_pusher>

/**
\example queue_pusher_and_popper.cc
\details
Push all command line arguments to a queue,
pop each element and print it to stdout.
*/
int main(int arch, char* argv[]) {
	std::queue<std::string> q;
	/// [Push elements]
	std::copy(
		sys::cstring_iterator<char*>(argv),
		sys::cstring_iterator<char*>(),
		sys::queue_pusher(q)
	);
	/// [Push elements]
	/// [Pop elements]
	std::copy(
		sys::queue_popper(q),
		sys::queue_popper(),
		std::ostream_iterator<std::string>(std::cout, "\n")
	);
	/// [Pop elements]
	return 0;
}
