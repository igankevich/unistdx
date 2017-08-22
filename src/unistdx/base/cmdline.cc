#include "cmdline"

#include <sstream>
#include <stdexcept>

void
sys::parse_arguments(
	int argc,
	const char* argv[],
	input_operator_type* input_operators
) {
	for (int i=1; i<argc; ++i) {
		std::string arg(argv[i]);
		std::string::size_type eq_pos = arg.find('=');
		if (eq_pos == std::string::npos) {
			throw std::invalid_argument("bad command line argument");
		}
		std::string key = arg.substr(0, eq_pos);
		std::stringstream value(arg.substr(eq_pos+1));
		input_operator_type* op = input_operators;
		while (*op && !(*op)(key, value)) {
			++op;
		}
		if (!*op || value.fail()) {
			throw std::invalid_argument("bad command line argument");
		}
	}
}
