#include <gtest/gtest.h>

#include <fstream>
#include <iostream>
#include <vector>

#include <unistdx/test/random_string>
#include <unistdx/test/temporary_file>

#include <unistdx/io/memory_mapping>

TEST(memory_mapping, anonymous) {
    sys::memory_mapping<char> anon{1024};
}

TEST(memory_mapping, file) {
    test::temporary_file tmp(UNISTDX_TMPFILE);
    std::string expected_contents = test::random_string<char>(3333);
    { std::ofstream{tmp.path()} << expected_contents; }
    sys::memory_mapping<char> mapping(tmp.path());
    std::string actual{mapping.begin(), mapping.end()};
    EXPECT_EQ(expected_contents, actual);
}
