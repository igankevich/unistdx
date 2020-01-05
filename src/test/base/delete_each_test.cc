#include <gtest/gtest.h>

#include <unistdx/base/delete_each>

TEST(delete_each, null_pointers) {
    std::vector<char*> ptrs{nullptr, nullptr, nullptr};
    EXPECT_NO_THROW(sys::delete_each(ptrs.begin(), ptrs.end()));
}
