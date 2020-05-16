\page execute Execute child process with arguments and environment (WIP)

Use the following code to execute \c errno command to display error message in
Russian locale.

```{.cpp}
sys::argstream args;
args.append("/bin/errno");
args.append(42);
sys::argstream env;
env.append("LANG=ru_RU.UTF-8");
sys::this_process::execute(args, env);
```

The above code is equivalent to running

    env LANG=ru_RU.UTF-8 errno 42

command in shell.


```{.cpp}
sys::argstream args;
args.append("errno");
args.append(42);
sys::argstream env;
env.append("LANG=ru_RU.UTF-8");
sys::this_process::execute_command(args, env);
```

```{.cpp}
uint32_t x = 123, y = 0;
sys::pstream str;
try {
    sys::opacket_guard<sys::pstream> g(str);
    str.begin_packet();
    str << x;
    str.end_packet();
} catch (const std::exception& err) {
    std::cerr << "write error: " << err.what() << std::endl;
}
str.flush();
if (str.read_packet()) {
    try {
        sys::ipacket_guard g2(str.rdbuf());
        str >> y;
    } catch (const std::exception& err) {
        std::cerr << "read error: " << err.what() << std::endl;
    }
}
return 0;
```
