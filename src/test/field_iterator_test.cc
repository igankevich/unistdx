#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <unistdx/it/field_iterator>

TEST(FieldIterator, Traverse) {
	std::vector<std::pair<std::string,int>> data = {
		{"a", 1},
		{"b", 2}
	};
	typedef std::vector<std::pair<std::string,int>> container_type;
	typedef sys::field_iterator<container_type::iterator,0> key_iterator;
	typedef sys::field_iterator<container_type::iterator,1> value_iterator;
	std::vector<std::string> expected_keys = {"a", "b"}, keys;
	std::vector<int> expected_values = {1, 2}, values;
	std::copy(
		key_iterator(data.begin()),
		key_iterator(data.end()),
		std::back_inserter(keys)
	);
	EXPECT_EQ(expected_keys, keys);
	std::copy(
		value_iterator(data.begin()),
		value_iterator(data.end()),
		std::back_inserter(values)
	);
	EXPECT_EQ(expected_values, values);
}
