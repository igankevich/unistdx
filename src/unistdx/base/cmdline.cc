#include "cmdline"

void
sys::parse_arguments(
	int argc,
	const char* argv[],
	input_operator_type* input_operators
) {
	for (int i=1; i<argc; ++i) {
		std::string arg(argv[i]);
		input_operator_type* op = input_operators;
		while (*op && !(*op)(i, arg)) {
			++op;
		}
		if (!*op) {
			throw sys::bad_argument(argv[i]);
		}
	}
}
