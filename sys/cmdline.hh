#ifndef SYS_CMDLINE_HH
#define SYS_CMDLINE_HH

#include <algorithm>
#include <iterator>
#include <string>
#include <istream>
#include <iomanip>
#include <sstream>
#include <functional>
#include <stdexcept>
#include <initializer_list>

namespace sys {

	struct basic_cmdline {

		typedef int pos_type;
		typedef std::string arg_type;
		typedef std::stringstream stream_type;

		basic_cmdline(int argc, char* argv[]) noexcept:
		_cmdline()
		{
			std::copy_n(
				argv, argc,
				std::ostream_iterator<char*>(_cmdline, "\n")
			);
		}

		basic_cmdline(basic_cmdline&&) = default;
		basic_cmdline() = delete;
		basic_cmdline(const basic_cmdline&) = delete;
		basic_cmdline& operator=(const basic_cmdline&) = delete;

		template<class F>
		void
		parse(F handle) {
			arg_type arg;
			pos_type pos = 0;
			while (_cmdline >> std::ws >> arg) {
				handle(arg, pos, _cmdline);
				++pos;
			}
		}

	private:
		stream_type _cmdline;
	};

	struct invalid_cmdline_argument: public std::exception {

		typedef basic_cmdline::arg_type arg_type;

		invalid_cmdline_argument(const char* what, const arg_type& arg):
		_what(what),
		_arg(arg)
		{}

		explicit
		invalid_cmdline_argument(const arg_type& arg):
		_what("bad command line argument"),
		_arg(arg)
		{}

		const arg_type&
		arg() const noexcept {
			return _arg;
		}

		const char*
		what() const noexcept override {
			return _what;
		}

	private:
		const char* _what;
		arg_type _arg;
	};

	struct cmdline: public basic_cmdline {

		using basic_cmdline::pos_type;
		using basic_cmdline::arg_type;
		using basic_cmdline::stream_type;
		typedef std::function<bool (const arg_type&, pos_type, stream_type&)> rule_type;

		cmdline(int argc, char* argv[], std::initializer_list<rule_type>&& rules) noexcept:
		basic_cmdline(argc, argv), _rules(std::move(rules))
		{}

		cmdline(cmdline&&) = default;
		cmdline() = delete;
		cmdline(const cmdline&) = delete;
		cmdline& operator=(const cmdline&) = delete;

		void
		parse() {
			using namespace std::placeholders;
			basic_cmdline::parse(
				std::bind(&cmdline::parse_arg, this, _1, _2, _3)
			);
		}

	private:

		void
		parse_arg(const arg_type& arg, pos_type pos, stream_type& cmdline) {
			auto first = _rules.begin();
			auto last = _rules.end();
			while (first != last && !first->operator()(arg, pos, cmdline)) {
				++first;
			}
			if (first == last && !_rules.empty()) {
				throw invalid_cmdline_argument(arg);
			}
		}

		std::vector<rule_type> _rules;

	};

	namespace cmd {

		template<class Value>
		struct option {

			typedef basic_cmdline::pos_type pos_type;
			typedef basic_cmdline::arg_type arg_type;
			typedef basic_cmdline::stream_type stream_type;

			option(std::initializer_list<std::string>&& keys, Value& val):
			_val(val), _keys(std::move(keys))
			{}

			bool
			operator()(const arg_type& arg, pos_type pos, stream_type& cmdline) {
				return std::find(_keys.begin(), _keys.end(), arg) != _keys.end() && cmdline >> _val;
			}

		private:
			Value& _val;
			std::vector<std::string> _keys;
		};

		template<class Value>
		option<Value>
		make_option(std::initializer_list<std::string>&& keys, Value& val) {
			return option<Value>(std::move(keys), val);
		}

		struct ignore_first_arg {

			typedef basic_cmdline::pos_type pos_type;
			typedef basic_cmdline::arg_type arg_type;
			typedef basic_cmdline::stream_type stream_type;

			bool
			operator()(const arg_type&, pos_type pos, stream_type&) {
				return pos == 0;
			}

		};

		struct ignore_arg {

			typedef basic_cmdline::pos_type pos_type;
			typedef basic_cmdline::arg_type arg_type;
			typedef basic_cmdline::stream_type stream_type;

			explicit
			ignore_arg(std::string&& key):
			_key(std::move(key))
			{}

			bool
			operator()(const arg_type& arg, pos_type, stream_type& str) {
				std::string dummy;
				return arg == _key and str >> dummy;
			}

		private:

			std::string _key;

		};

	}

}

#endif // SYS_CMDLINE_HH
