/*
UNISTDX — C++ library for Linux system calls.
© 2017, 2018, 2020 Ivan Gankevich

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

#include <unistdx/ipc/signal>

#include <ostream>

namespace {

    constexpr const int nsignals = 32;

    struct init_signal_names {

        const char* signal_names[nsignals] = {0};

        init_signal_names() {
            signal_names[SIGHUP] = "hang_up";
            signal_names[SIGINT] = "keyboard_interrupt";
            signal_names[SIGQUIT] = "quit";
            signal_names[SIGILL] = "illegal_instruction";
            signal_names[SIGABRT] = "abort";
            signal_names[SIGFPE] = "floating_point_exception";
            signal_names[SIGKILL] = "kill";
            signal_names[SIGSEGV] = "segmentation_fault";
            signal_names[SIGPIPE] = "broken_pipe";
            signal_names[SIGALRM] = "alarm";
            signal_names[SIGTERM] = "terminate";
            signal_names[SIGUSR1] = "user_defined_1";
            signal_names[SIGUSR2] = "user_defined_2";
            signal_names[SIGCHLD] = "child";
            signal_names[SIGCONT] = "resume";
            signal_names[SIGSTOP] = "stop";
            signal_names[SIGTSTP] = "terminal_stop";
            signal_names[SIGTTIN] = "terminal_input";
            signal_names[SIGTTOU] = "terminal_output";
            signal_names[SIGBUS] = "bad_memory_access";
            #if defined(UNISTDX_HAVE_SIGPOLL)
            signal_names[SIGPOLL] = "poll";
            #endif
            signal_names[SIGPROF] = "profile";
            signal_names[SIGSYS] = "bad_argument";
            signal_names[SIGTRAP] = "breakpoint";
            signal_names[SIGURG] = "urgent";
            signal_names[SIGVTALRM] = "virtual_alarm";
            signal_names[SIGXCPU] = "cpu_time_limit_exceeded";
            signal_names[SIGXFSZ] ="file_size_limit_exceeded";
            #if defined(UNISTDX_HAVE_SIGSTKFLT)
            signal_names[SIGSTKFLT] = "coprocessor_stack_fault";
            #endif
            #if defined(UNISTDX_HAVE_SIGPWR)
            signal_names[SIGPWR] = "power_failure";
            #endif
            #if defined(UNISTDX_HAVE_SIGWINCH)
            signal_names[SIGWINCH] ="terminal_window_resize";
            #endif
        }

        inline const char*
        operator[](sys::signal_type s) {
            return this->signal_names[s];
        }

    } signal_names;

}

const char* sys::to_string(signal rhs) noexcept {
    auto s = signal_type(rhs);
    if (s >= 0 && s < nsignals) { return signal_names[s]; }
    return "unknown";
}

std::ostream&
sys::operator<<(std::ostream& out, const signal rhs) {
    return out << to_string(rhs) << '(' << signal_type(rhs) << ')';
}
