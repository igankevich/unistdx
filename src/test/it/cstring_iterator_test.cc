#include <gtest/gtest.h>

#include <iterator>
#include <string>
#include <vector>

#include <unistdx/it/cstring_iterator>

TEST(cstring_iterator, basic) {
    const char* args[] = {
        "first",
        "second",
        nullptr
    };
    sys::cstring_iterator<const char*> first(args), last;
    std::vector<std::string> actual;
    std::copy(first, last, std::back_inserter(actual));
    EXPECT_EQ(2u, actual.size());
    EXPECT_EQ("first", actual[0]);
    EXPECT_EQ("second", actual[1]);
}

struct args_and_count {

    size_t count;
    std::vector<const char*> args;

    inline const char**
    get_args() const noexcept {
        return const_cast<const char**>(args.data());
    }

};

class cstring_iterator_test: public ::testing::TestWithParam<args_and_count> {};

TEST_P(cstring_iterator_test, All) {
    const args_and_count& param = GetParam();
    const size_t cnt =
        std::distance(
            sys::cstring_iterator<const char*>(param.get_args()),
            sys::cstring_iterator<const char*>()
        );
    EXPECT_EQ(param.count, cnt);
}

INSTANTIATE_TEST_CASE_P(
    AllSizes,
    cstring_iterator_test,
    ::testing::Values(
        args_and_count{2, {"1", "2", 0}},
        args_and_count{1, {"1", 0}},
        args_and_count{0, {0}}
    )
);
