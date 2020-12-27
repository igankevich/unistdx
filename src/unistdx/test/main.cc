/*
UNISTDX — C++ library for Linux system calls.
© 2020 Ivan Gankevich

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

#include <unistdx/test/test_executor>


using namespace sys::test;

std::string prefix = "test_";
bool catch_errors = true;

void arguments(int argc, char** argv) {
    for (int i=1; i<argc; ++i) {
        std::string arg(argv[i]);
        auto pos = arg.find('=');
        if (pos == std::string::npos) {
            throw std::invalid_argument(arg);
        }
        std::string name = arg.substr(0,pos);
        std::string value = arg.substr(pos+1);
        if (name == "prefix") {
            prefix = std::move(value);
        } else if (name == "catch-errors") {
            catch_errors = bool(std::stoi(value));
        } else if (name == "filter") {
            // TODO glob regex filter
        }
    }
}

int main(int argc, char* argv[]) {
    arguments(argc, argv);
    sys::test::Test_executor tests;
    tests.catch_errors(catch_errors);
    dl::for_each_shared_object([&] (const elf::shared_object& obj, size_t nobjects) {
        sys::string buf(4096);
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
                for (elf::elf_word i=0; i<num_symbols; ++i) {
                    const auto& sym = symbols[i];
                    auto name = &strings[sym.name()];
                    if (*name == 0) { continue; }
                    std::string demangled_name = sys::demangle(name, buf);
                    if (demangled_name.compare(0, prefix.size(), prefix) == 0) {
                        sys::test::Symbol s;
                        s.demangled_name(std::move(demangled_name));
                        s.original_name(std::string(name));
                        s.address(reinterpret_cast<void*>(sym.address()));
                        tests.emplace(std::move(s));
                    }
                }
            }
        }
        return 1;
    });
    return tests.run();
}
