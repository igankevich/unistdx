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

#include <iostream>

#include <unistdx/system/error>
#include <unistdx/system/linker>

void test_function() {
    std::clog << "hello world" << std::endl;
}

int main(int argc, char* argv[]) {
    sys::for_each_shared_object([] (const sys::shared_object& obj, size_t nobjects) {
        sys::string buf(4096);
        for (const auto& prg : obj) {
            if (prg.type() != sys::segment_types::dynamic) { continue; }
            auto dynamic_section = reinterpret_cast<sys::dynamic_section*>(
                obj.base_address() + prg.virtual_address());
            using tags = sys::dynamic_section_tags;
            sys::elf_word* hash{};
            sys::elf_word num_symbols{};
            char* strings{};
            sys::symbol* symbols{};
            for (; *dynamic_section; ++dynamic_section) {
                //std::clog << "tag: " << int(dynamic_section->tag()) << std::endl;
                switch (dynamic_section->tag()) {
                    case tags::hash:
                        hash = dynamic_section->address<sys::elf_word>();
                        num_symbols = hash[1];
                        break;
                    case tags::string_table:
                        strings = dynamic_section->address<char>();
                        break;
                    case tags::symbol_table:
                        symbols = dynamic_section->address<sys::symbol>();
                        break;
                    default:
                        break;
                }
            }
            if (symbols && strings) {
                for (sys::elf_word i=0; i<num_symbols; ++i) {
                    auto name = &strings[symbols[i].name()];
                    std::clog << sys::demangle(name, buf) << std::endl;
                }
            }
        }
        return 1;
    });
    return 0;
}
