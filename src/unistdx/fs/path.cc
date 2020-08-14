#include <unistdx/base/check>
#include <unistdx/fs/path>

#if defined(UNISTDX_HAVE_SYS_XATTR_H)
#include <sys/types.h>
#include <sys/xattr.h>

template <> std::string
sys::path::attribute(c_string name, file_attribute_options f) const {
    std::string value;
    auto get = bool(f & file_attribute_options::no_follow) ? ::lgetxattr : ::getxattr;
    auto size = get(data(), name, nullptr, 0);
    UNISTDX_CHECK(size);
    if (size == 0) { return std::string(); }
    while (true) {
        value.resize(size);
        auto new_size = get(data(), name, &value[0], size);
        UNISTDX_CHECK(new_size);
        if (new_size == size) { break; }
        size = new_size;
    }
    return value;
}

template <> void
sys::path::attribute(c_string name, const_string value, file_attribute_flags f1,
                     file_attribute_options f) {
    auto set = bool(f & file_attribute_options::no_follow) ? ::lsetxattr : ::setxattr;
    UNISTDX_CHECK(set(data(), name, value.data(), value.size(), int(f1)));
}

template <> void
sys::path::remove_attribute(c_string name, file_attribute_options f) {
    auto remove = bool(f & file_attribute_options::no_follow) ? ::lremovexattr : ::removexattr;
    UNISTDX_CHECK(remove(data(), name));
}

template <>
sys::file_attributes sys::path::attributes(file_attribute_options f) const {
    auto list = bool(f & file_attribute_options::no_follow) ? ::llistxattr : ::listxattr;
    std::unique_ptr<char[]> names;
    auto size = list(data(), nullptr, 0);
    if (size == 0) { return file_attributes(); }
    UNISTDX_CHECK(size);
    while (true) {
        names.reset(new char[size]);
        auto new_size = list(data(), names.get(), size);
        UNISTDX_CHECK(new_size);
        if (new_size == size) { break; }
        size = new_size;
    }
    return file_attributes(std::move(names), size);
}
#endif
