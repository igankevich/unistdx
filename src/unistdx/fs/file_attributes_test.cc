#include <gtest/gtest.h>

#include <unistdx/fs/path>
#include <unistdx/fs/temporary_file>

TEST(file_attributes, _) {
    sys::temporary_file file("file_attributes");
    sys::path path(file.name());
    auto attrs = path.attributes();
    EXPECT_THROW(path.attribute("user.unistdx.x"), sys::bad_call);
    EXPECT_EQ(0, std::distance(attrs.begin(), attrs.end()));
    EXPECT_EQ(nullptr, attrs.data());
    EXPECT_EQ(0, attrs.size());
    path.attribute("user.unistdx.x", "1");
    attrs = path.attributes();
    EXPECT_NE(nullptr, attrs.data());
    EXPECT_NE(0, attrs.size());
    EXPECT_EQ("1", path.attribute("user.unistdx.x"));
    EXPECT_STREQ("user.unistdx.x", attrs.front());
    EXPECT_EQ(1, std::distance(attrs.begin(), attrs.end()));
    file.attribute("user.unistdx.x", "2");
    attrs = file.attributes();
    EXPECT_NE(nullptr, attrs.data());
    EXPECT_NE(0, attrs.size());
    EXPECT_EQ("2", file.attribute("user.unistdx.x"));
    EXPECT_STREQ("user.unistdx.x", attrs.front());
    EXPECT_EQ(1, std::distance(attrs.begin(), attrs.end()));
}
