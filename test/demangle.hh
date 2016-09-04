#ifndef TEST_DEMANGLE_HH
#define TEST_DEMANGLE_HH

#if defined(__GLIBCXX__) || defined(__GLIBCPP__) || (defined(_LIBCPP_VERSION) && defined(__APPLE__))
#define STDX_TEST_HAVE_CXXABI
#endif

#if defined(STDX_TEST_HAVE_CXXABI)
#include <stdlib.h>
#include <cxxabi.h>
#include <utility>
#include <string>
#endif

namespace test {

	std::string
	demangle_name(const char* mangled_name) {
	#if defined(STDX_TEST_HAVE_CXXABI)
		int status;
		char* buf = abi::__cxa_demangle(mangled_name, 0, 0, &status);
		std::string name{buf};
		std::free(buf);
		return std::move(name);
	#else
		return std::string(mangled_name);
	#endif
	}

	template<class T>
	std::string
	demangle_name() {
		return demangle_name(typeid(T).name());
	}

	std::string
	short_name(std::string tmp) {

		if (tmp.size() == 0) return tmp;

		/// try to remove template arguments
		if (tmp.back() == '>') {
			auto last = tmp.end()-2;
			auto first = tmp.begin();
			int checksum = 1;
			while (last != first && checksum != 0) {
				if (*last == '>') ++checksum;
				else if (*last == '<') --checksum;
				--last;
			}
			if (checksum == 0) {
				tmp.erase(last+1, tmp.end());
			}
		}

		/// remove namespaces or nested classes
		size_t idx = tmp.find_last_of(':');
		if (idx != std::string::npos) {
			tmp.erase(0, idx+1);
		}

		return tmp;
	}

}

#endif // TEST_DEMANGLE_HH
