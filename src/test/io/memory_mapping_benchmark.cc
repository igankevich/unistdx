#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <unistdx/io/memory_mapping>
#include <unistdx/test/random_string>
#include <unistdx/test/temporary_file>

typedef std::chrono::high_resolution_clock clock_type;

std::string
file_to_string(const std::string& filename, size_t file_size) {
	const size_t n = 4096;
	char buf[n];
	std::filebuf fbuf;
	fbuf.open(filename, std::ios::in);
//	std::stringstream tmp;
//	tmp << &fbuf;
	std::string s;
	s.reserve(file_size);
	std::streamsize nread;
	do {
		nread = fbuf.sgetn(buf, n);
		s.append(buf, nread);
	} while (nread != 0);
	return s;
}

std::string
simple_copy(const std::string& filename, size_t file_size) {
	std::string result;
	result.reserve(file_size);
	const size_t n = 4096;
	char buf[n];
	sys::fildes in{
		filename.data(),
		sys::open_flag::read_only |
		sys::open_flag::close_on_exec
	};
	ssize_t m;
	do {
		m = in.read(buf, n);
		result.append(buf, m);
	} while (m != 0);
	return result;
}

int main() {
	using namespace std::chrono;
	const size_t
		min_power = 10,
		max_power = 24;
	std::cout
		<< std::setw(10) << "size"
		<< std::setw(20) << "streambuf"
		<< std::setw(20) << "mmap"
		<< std::setw(20) << "read"
		<< '\n';
	for (size_t i=min_power; i<=max_power; ++i) {
		size_t file_size = 1 << i;
		test::temporary_file tmp("memory_mapping_benchmark.dat");
		std::string expected_contents = test::random_string<char>(file_size);
		{ std::ofstream{tmp.path()} << expected_contents; }
		microseconds count1, count2, count3;
		// dry run
		{ std::stringstream s; s << std::ifstream{tmp.path()}.rdbuf(); }
		// stream buffers
		{
			auto t0 = clock_type::now();
			std::string actual{file_to_string(tmp.path(), file_size)};
			if (actual != expected_contents) {
				throw std::runtime_error{"bad content"};
			}
			auto t1 = clock_type::now();
			count1 = duration_cast<microseconds>(t1-t0);
		}
		// memory mapping
		{
			auto t0 = clock_type::now();
			sys::memory_mapping<char> mapping{
				tmp.path(),
				0,
				file_size,
				sys::page_flag::read,
				sys::map_flag::shared
			};
			std::string actual{mapping.begin(), mapping.end()};
			if (actual != expected_contents) {
				throw std::runtime_error{"bad content"};
			}
			auto t1 = clock_type::now();
			count2 = duration_cast<microseconds>(t1-t0);
		}
		// simple copy
		{
			auto t0 = clock_type::now();
			std::string actual{simple_copy(tmp.path(), file_size)};
			if (actual != expected_contents) {
				throw std::runtime_error{"bad content"};
			}
			auto t1 = clock_type::now();
			count3 = duration_cast<microseconds>(t1-t0);
		}
		std::cout
			<< std::setw(10) << file_size
			<< std::setw(20) << count1.count()
			<< std::setw(20) << count2.count()
			<< std::setw(20) << count3.count()
			<< '\n';
	}
	return 0;
}
