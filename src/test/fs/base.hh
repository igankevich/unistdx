#ifndef TEST_FS_BASE_HH
#define TEST_FS_BASE_HH

#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include <unistdx/test/tmpdir>

namespace test {

    template <class Stream, class Iterator>
    void
    test_file_list(
        const tmpdir& tdir,
        const std::vector<std::string>& files
    ) {
        Stream dir(tdir);
        std::set<std::string> files_orig(files.begin(), files.end());
        std::set<std::string> files_actual;
        std::for_each(
            Iterator(dir),
            Iterator(),
            [&files_actual] (const sys::directory_entry& ent) {
                files_actual.emplace(ent.name());
            }
        );
        EXPECT_EQ(files_orig, files_actual) << "bad file list in " << tdir.name();
    }

    template <class Stream, class Iterator>
    void
    test_file_count(
        const test::tmpdir& tdir,
        const std::vector<std::string>& files
    ) {
        Stream dir(tdir);
        size_t cnt = std::distance(Iterator(dir), Iterator());
        EXPECT_EQ(cnt, files.size()) << "bad total file count in " << tdir.name();
    }

    template <class Stream>
    inline void
    assert_good(const Stream& dir, bool is_open) {
        EXPECT_EQ(dir.is_open(), is_open);
        EXPECT_EQ(dir.good(), true);
        EXPECT_EQ(dir.bad(), false);
        EXPECT_EQ(dir.eof(), false);
        EXPECT_EQ(static_cast<bool>(dir), true);
    }

    template <class Stream>
    inline void
    test_open_close(Stream& dir, const test::tmpdir& tdir) {
        assert_good(dir, false);
        dir.open(tdir);
        assert_good(dir, true);
        dir.open(tdir);
        assert_good(dir, true);
        dir.close();
        assert_good(dir, false);
        dir.close();
        assert_good(dir, false);
    }

}

#endif // vim:filetype=cpp
