#include <sys/stat.h>

#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include <unistdx/base/bad_call>

TEST(bad_call, throw_error) {
    try {
        struct ::stat st;
        int ret = ::stat("non-existent-file", &st);
        EXPECT_EQ(-1, ret);
        throw sys::bad_call(__FILE__, __LINE__, __func__);
    } catch (const sys::bad_call& err) {
        EXPECT_EQ(std::errc::no_such_file_or_directory, err.errc());
        std::stringstream str;
        str << err;
        std::string filename(__FILE__);
        EXPECT_EQ(0, str.str().compare(0, filename.size(), filename));
    }
}
