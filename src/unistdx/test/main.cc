/*
UNISTDX — C++ library for Linux system calls.
© 2020, 2021 Ivan Gankevich

This file is part of UNISTDX.

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

#include <regex>

#include <unistdx/base/log_message>
#include <unistdx/ipc/process>
#include <unistdx/ipc/signal>
#include <unistdx/test/test_executor>

using namespace sys::test;

inline void trim_right(std::string& s) {
    while (!s.empty() && std::isspace(s.back())) { s.pop_back(); }
}

inline void trim_left(std::string& s) {
    std::string::size_type i = 0;
    auto n = s.size();
    for (; i<n && std::isspace(s[i]); ++i) {}
    if (i != n) { s = s.substr(i); }
}

inline void trim_both(std::string& s) {
    trim_right(s);
    trim_left(s);
}

class Duration: public sys::test::Test_executor::duration {
public:
    using base_duration = std::chrono::system_clock::duration;
    using base_duration::duration;
    inline Duration(base_duration rhs): base_duration(rhs) {}
};

auto string_to_duration(std::string s) -> Duration;

auto string_to_duration(std::string s) -> Duration {
    using namespace std::chrono;
    using d = Duration::base_duration;
    using days = std::chrono::duration<Duration::rep,std::ratio<60*60*24>>;
    trim_both(s);
    std::size_t i = 0, n = s.size();
    Duration::rep value = std::stoul(s, &i);
    std::string suffix;
    if (i != n) { suffix = s.substr(i); }
    if (suffix == "ns") { return duration_cast<d>(nanoseconds(value)); }
    if (suffix == "us") { return duration_cast<d>(microseconds(value)); }
    if (suffix == "ms") { return duration_cast<d>(milliseconds(value)); }
    if (suffix == "s" || suffix.empty()) { return duration_cast<d>(seconds(value)); }
    if (suffix == "m") { return duration_cast<d>(minutes(value)); }
    if (suffix == "h") { return duration_cast<d>(hours(value)); }
    if (suffix == "d") { return duration_cast<d>(days(value)); }
    std::stringstream tmp;
    tmp << "unknown duration suffix \"" << suffix << "\"";
    throw std::invalid_argument(tmp.str());
}

std::string test_prefix = "test_";
std::string args_prefix = "args_";
std::regex test_filter{".*", std::regex::ECMAScript | std::regex::optimize};
sys::test::Test_executor::duration timeout = std::chrono::seconds(30);
sys::test::Test_executor tests;

void arguments(int argc, char** argv) {
    sys::process::flags process_flags =
        sys::process::flags::fork | sys::process::flags::signal_parent;
    for (int i=1; i<argc; ++i) {
        std::string arg(argv[i]);
        auto pos = arg.find('=');
        if (pos == std::string::npos) {
            throw std::invalid_argument(arg);
        }
        std::string name = arg.substr(0,pos);
        std::string value = arg.substr(pos+1);
        if (name == "prefix" || name == "test-prefix") {
            test_prefix = std::move(value);
        } else if (name == "prefix" || name == "args-prefix") {
            args_prefix = std::move(value);
        } else if (name == "filter") {
            test_filter = std::regex{value, test_filter.flags()};
        } else if (name == "catch-errors") {
            tests.catch_errors(bool(std::stoi(value)));
        } else if (name == "unshare") {
            value += ',';
            std::string subvalue;
            for (auto ch : value) {
                if (ch == ',') {
                    if (subvalue == "network") {
                        process_flags |= sys::process::flags::unshare_network;
                    } else if (subvalue == "users") {
                        process_flags |= sys::process::flags::unshare_users;
                    } else if (subvalue.empty()) {
                        process_flags = sys::process::flags::fork |
                            sys::process::flags::signal_parent;
                    } else {
                        throw std::invalid_argument(arg);
                    }
                    subvalue.clear();
                } else {
                    subvalue += ch;
                }
            }
        } else if (name == "timeout") {
            timeout = string_to_duration(value);
        } else if (name == "verbose") {
            tests.verbose(bool(std::stoi(value)));
        } else if (name == "redirect") {
            tests.redirect(bool(std::stoi(value)));
        } else {
            throw std::invalid_argument(name);
        }
    }
    tests.process_flags(process_flags);
}

void parent_signal_handlers() {
    using namespace sys::this_process;
    auto on_terminate = sys::signal_action([](int sig) {
        try {
            tests.send(sys::signal(sig));
        } catch (const std::exception& err) {
            const char* msg = "error in parent signal handler";
            ::write(2, msg, std::string::traits_type::length(msg));
            ::write(2, err.what(), std::string::traits_type::length(err.what()));
        }
        tests.wait();
        std::_Exit(sig);
    });
    using s = sys::signal;
    ignore_signal(s::broken_pipe);
    bind_signal(s::keyboard_interrupt, on_terminate);
    bind_signal(s::terminate, on_terminate);
    bind_signal(s::quit, on_terminate);
    bind_signal(s::hang_up, on_terminate);
}

int main(int argc, char* argv[]) {
    arguments(argc, argv);
    sys::string buf{4096};
    dl::for_each_shared_object([&] (const elf::shared_object& obj, size_t nobjects) {
        for (const auto& prg : obj) {
            if (prg.type() != elf::segment_types::dynamic) { continue; }
            auto dynamic_section = reinterpret_cast<elf::dynamic_section*>(
                obj.base_address() + prg.virtual_address());
            using tags = elf::dynamic_section_tags;
            elf::elf_word* hash{};
            elf::elf_word num_symbols{};
            char* strings{};
            elf::symbol* symbols{};
            for (; *dynamic_section; ++dynamic_section) {
                //std::clog << "tag: " << int(dynamic_section->tag()) << std::endl;
                switch (dynamic_section->tag()) {
                    case tags::hash:
                        hash = dynamic_section->address<elf::elf_word>();
                        num_symbols = hash[1];
                        break;
                    case tags::string_table:
                        strings = dynamic_section->address<char>();
                        break;
                    case tags::symbol_table:
                        symbols = dynamic_section->address<elf::symbol>();
                        break;
                    default:
                        break;
                }
            }
            if (symbols && strings) {
                // functions
                for (elf::elf_word i=0; i<num_symbols; ++i) {
                    const auto& sym = symbols[i];
                    auto name = &strings[sym.name()];
                    if (*name == 0) { continue; }
                    std::string demangled_name = sys::demangle(name, buf);
                    if (demangled_name.compare(0, test_prefix.size(), test_prefix) == 0) {
                        sys::test::Symbol s;
                        s.original_name(std::string(name));
                        s.demangled_name(std::move(demangled_name));
                        s.address(reinterpret_cast<void*>(sym.address()));
                        s.type(sym.type());
                        if (std::regex_search(s.short_name(), test_filter)) {
                            tests.emplace(std::move(s));
                        }
                    }
                }
                // arguments
                for (elf::elf_word i=0; i<num_symbols; ++i) {
                    const auto& sym = symbols[i];
                    auto name = &strings[sym.name()];
                    if (*name == 0) { continue; }
                    std::string demangled_name = sys::demangle(name, buf);
                    if (demangled_name.compare(0, args_prefix.size(), args_prefix) == 0) {
                        sys::test::Symbol s;
                        s.original_name(std::string(name));
                        s.demangled_name(std::move(demangled_name));
                        s.address(reinterpret_cast<void*>(sym.address()));
                        s.type(sym.type());
                        std::string test_name;
                        test_name.reserve(test_prefix.size() + args_prefix.size());
                        test_name += test_prefix;
                        test_name += s.short_name().substr(args_prefix.size());
                        tests.test_arguments(test_name, std::move(s));
                    }
                }
            }
        }
        return 1;
    });
    parent_signal_handlers();
    return tests.run();
}
