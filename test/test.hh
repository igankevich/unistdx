#ifndef TEST_TEST_HH
#define TEST_TEST_HH

#include <string>
#include <random>
#include <functional>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <stdexcept>
#include <chrono>
#include <type_traits>
#include <iostream>
#include <iomanip>
#include <memory>
#include <cassert>
#include <array>

#include <stdx/random.hh>
#include <stdx/iterator.hh>

#include "color.hh"
#include "demangle.hh"

namespace test {

	struct Basic_test {

		explicit
		Basic_test(const std::string& name):
		_testname(name) {}

		Basic_test(const Basic_test&) = default;
		Basic_test& operator=(const Basic_test&) = default;
		virtual ~Basic_test() = default;

		virtual int
		run() {
			int ret = 0;
			try {
				std::clog << _testname << ' ' << std::flush;
				xrun();
				std::clog << Color::FG_LIGHT_GREEN << "PASSED" << Color::RESET << std::endl;
			} catch (std::exception& ex) {
				ret = -1;
				std::clog << Color::FG_LIGHT_RED << "FAILED" << Color::RESET
					<< '\n' << ex.what() << std::endl;
			} catch (...) {
				ret = -1;
				std::clog << Color::FG_LIGHT_RED << "FAILED" << Color::RESET << std::endl;
			}
			return ret;
		}

		const std::string&
		name() const {
			return _testname;
		}

	protected:

		virtual void xrun() = 0;

	private:

		std::string _testname;
	};

	template<class X>
	struct Test: public Basic_test {

		Test():
		Basic_test(demangle_name<X>()) {}

		explicit
		Test(const std::string& name):
		Basic_test(name) {}

	};


	template<class Testname, class Array>
	struct Parametric_test: public test::Test<Testname> {

		typedef Array array_type;
		typedef typename array_type::size_type size_type;
		typedef typename array_type::value_type value_type;

		Parametric_test(array_type&& minval, array_type&& maxval):
		_minval(minval),
		_maxval(maxval)
		{
			assert(minval.size() == maxval.size());
		}

		virtual ~Parametric_test() = default;

		void xrun() override {
			const size_type ndim = _minval.size();
			array_type idx = _minval;
			for (size_type dim=0; dim<ndim; ++dim) {
				const value_type x0 = _minval[dim];
				const value_type x1 = _maxval[dim];
				for (value_type i=x0; i<=x1; ++i) {
					idx[dim] = i;
					parametric_run(idx);
				}
			}
		}

		virtual void parametric_run(value_type i) { assert(false); }
		virtual void parametric_run(value_type i, value_type j) { assert(false); }
		virtual void parametric_run(value_type i, value_type j, value_type k) { assert(false); }

		virtual void
		parametric_run(array_type idx) {
			if (idx.size() == 1) parametric_run(idx[0]);
			else if (idx.size() == 2) parametric_run(idx[0], idx[1]);
			else if (idx.size() == 3) parametric_run(idx[0], idx[1], idx[2]);
			else assert(false);
		}

	private:
		array_type _minval;
		array_type _maxval;
	};

	struct Test_suite: public Test<Test_suite> {

		typedef std::unique_ptr<Basic_test> value_type;

		explicit
		Test_suite(const std::string& name):
		Test(name)
		{}

		Test_suite(const std::string& name, std::initializer_list<Basic_test*> tests):
		Test(name)
		{ init_tests(tests); }

		explicit
		Test_suite(std::initializer_list<Basic_test*> tests):
		Test("")
		{ init_tests(tests); }

		void
		add(Basic_test* tst) {
			_tests.emplace_back(tst);
		}

		int
		run() override {
			if (not this->name().empty()) {
				std::clog << this->name() << std::endl;
			}
			int ret = std::accumulate(
				stdx::front_popper(_tests),
				stdx::front_popper_end(_tests),
				0,
				[] (int sum, const value_type& tst) {
					return sum |= tst->run();
				}
			);
			return ret;
		}

		void xrun() override {}

	private:

		void
		init_tests(std::initializer_list<Basic_test*> tests) {
			std::transform(
				tests.begin(),
				tests.end(),
				std::back_inserter(_tests),
				[] (Basic_test* rhs) {
					return value_type(rhs);
				}
			);
		}

		std::deque<value_type> _tests;
	};

	template<class T>
	std::basic_string<T> random_string(size_t size, T min = std::numeric_limits<T>::min(),
		T max = std::numeric_limits<T>::max()) {
		static std::default_random_engine generator(
			std::chrono::high_resolution_clock::now().time_since_epoch().count()
		);
		std::uniform_int_distribution<T> dist(min, max);
		auto gen = std::bind(dist, generator);
		std::basic_string<T> ret(size, '_');
		std::generate(ret.begin(), ret.end(), gen);
		return ret;
	}

	void print_args(std::ostream& out) {}

	template<class T, class ... Args>
	void print_args(std::ostream& out, T&& value, Args&& ... args) {
		out << value;
		print_args(out, std::forward<Args>(args)...);
	}

	template<class X, class Y, class ... Args>
	void equal(X x, Y y, Args&& ... args) {
		if (!(x == y)) {
			std::stringstream msg;
			msg << "ERROR: values are not equal";
			msg << '\n';
			msg << "DEBUG: ";
			msg << "value=\"" << x << "\", expected=\"" << y << "\", ";
			print_args(msg, std::forward<Args>(args)...);
			throw std::runtime_error(msg.str());
		}
	}

	template<class Container1, class Container2, class Func, class ... Args>
	void do_compare(const Container1& cnt1, const Container2& cnt2, Func format, const char* text, Args&& ... args) {
		auto pair = std::mismatch(cnt1.begin(), cnt1.end(), cnt2.begin());
		if (pair.first != cnt1.end()) {
			auto pos = std::distance(cnt1.begin(), pair.first);
			std::stringstream msg;
			msg << "ERROR: " << text;
			msg << '\n';
			msg << "CAUSE: containers' contents do not match\n";
			msg << "DEBUG: ";
			msg << "i=" << pos << ",first=\"" << format(*pair.first) << "\",second=\"" << format(*pair.second) << "\",";
			print_args(msg, std::forward<Args>(args)...);
			throw std::runtime_error(msg.str());
		}
	}

	template<class Container1, class Container2, class ... Args>
	void compare(const Container1& cnt1, const Container2& cnt2, const char* text, Args&& ... args) {
		typedef typename Container1::value_type value_type;
		do_compare(
			cnt1,
			cnt2,
			[] (const value_type& rhs) { return rhs; },
			text,
			std::forward<Args>(args)...
		);
	}

	typedef std::chrono::nanoseconds::rep Time;

	Time time_seed() {
		return std::chrono::high_resolution_clock::now().time_since_epoch().count();
	}

	template<class T>
	T randomval() {
		typedef typename
			std::conditional<std::is_floating_point<T>::value,
				std::uniform_real_distribution<T>,
					std::uniform_int_distribution<T>>::type
						Distribution;
		typedef std::default_random_engine::result_type Res_type;
		static std::default_random_engine generator(static_cast<Res_type>(time_seed()));
		static Distribution distribution(std::numeric_limits<T>::min(),std::numeric_limits<T>::max());
		return distribution(generator);
	}

	template<class It, class Engine>
	void
	randomise(It first, It last, Engine& rng) {
		typedef typename std::decay<decltype(*first)>::type value_type;
		while (first != last) {
			*first = stdx::n_random_bytes<value_type>(rng);
			++first;
		}
	}

	template<class Func, class Object>
	void
	invar(Func invariant, Object& obj) {
		if (!invariant(obj)) {
			std::stringstream msg;
			msg << "invariant does not hold for "
				<< demangle_name<Object>()
				<< ": object=" << obj;
			throw std::runtime_error(msg.str());
		}
	}

	template<class T>
	void
	io_single(const T& expected) {
		T addr2;
		std::stringstream s;
		s << expected;
		s >> addr2;
		equal(addr2, expected);
	}

	template<class T, class Generator>
	void
	io_multiple(Generator gen) {
		std::vector<T> addrs(10);
		std::generate(addrs.begin(), addrs.end(), [&gen] () { return gen(); });

		// write
		std::stringstream os;
		std::copy(
			addrs.begin(), addrs.end(),
			std::ostream_iterator<T>(os, "\n")
		);

		// read
		std::vector<T> addrs2;
		std::copy(
			std::istream_iterator<T>(os),
			std::istream_iterator<T>(),
			std::back_inserter(addrs2)
		);

		test::compare(addrs, addrs2, "addrs does not match addrs2");
	}

}

namespace std {
	std::ostream& operator<<(std::ostream& out, const std::basic_string<unsigned char>& rhs) {
		std::ostream_iterator<char> it(out, "");
		std::copy(rhs.begin(), rhs.end(), it);
		return out;
	}
}

#endif // TEST_TEST_HH
