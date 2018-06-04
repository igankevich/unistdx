#include <algorithm>
#include <iterator>
#include <map>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <unistdx/it/field_iterator>
#include <unistdx/it/iterator_pair>

template <class Container>
void
test_field_iterator() {
	typedef Container container_type;
	typedef sys::field_iterator<typename container_type::iterator,0>
		key_iterator;
	typedef sys::field_iterator<typename container_type::iterator,1>
		value_iterator;
	container_type data = {
		{"a", 1},
		{"b", 2}
	};
	std::vector<std::string> expected_keys = {"a", "b"}, keys;
	std::vector<int> expected_values = {1, 2}, values, values2;
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
	for (const auto& v : sys::make_view<1>(data)) {
		values2.push_back(v);
	}
	EXPECT_EQ(expected_values, values2);
}

TEST(FieldIterator, TraverseVector) {
	test_field_iterator<std::vector<std::pair<std::string,int>>>();
}

TEST(FieldIterator, TraverseMap) {
	test_field_iterator<std::map<std::string,int>>();
}