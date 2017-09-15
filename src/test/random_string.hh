#ifndef TEST_RANDOM_STRING_HH
#define TEST_RANDOM_STRING_HH

#include <algorithm>
#include <functional>
#include <random>
#include <string>

std::default_random_engine rng;

template <class T>
std::basic_string<T>
random_string(size_t n) {
	std::uniform_int_distribution<T> dist('a', 'z');
	std::basic_string<T> str(n, '_');
	std::generate(str.begin(), str.end(), std::bind(dist, rng));
	return str;
}

#endif // vim:filetype=cpp
