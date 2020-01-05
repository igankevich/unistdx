#include <unistdx/base/command_line>
#include <gtest/gtest.h>

TEST(CommandLine, Parse) {
    const char* argv[] = {
        "progname",
        "arg1=123",
        "arg2=hello"
    };
    int arg1 = 0;
    std::string arg2;
    sys::input_operator_type options[] = {
        sys::ignore_first_argument(),
        sys::make_key_value("arg1", arg1),
        sys::make_key_value("arg2", arg2),
        nullptr
    };
    sys::parse_arguments(3, argv, options);
    EXPECT_EQ(123, arg1);
    EXPECT_EQ("hello", arg2);
}

TEST(CommandLine, InvalidArgument) {
    const char* argv[] = {
        "progname",
        "arg3=123"
    };
    int arg1 = 0;
    std::string arg2;
    sys::input_operator_type options[] = {
        sys::ignore_first_argument(),
        sys::make_key_value("arg1", arg1),
        sys::make_key_value("arg2", arg2),
        nullptr
    };
    try {
        sys::parse_arguments(2, argv, options);
        FAIL();
    } catch (const sys::bad_argument& err) {
        EXPECT_TRUE(err.what() != nullptr);
    }
    EXPECT_EQ(0, arg1);
    EXPECT_TRUE(arg2.empty());
}
