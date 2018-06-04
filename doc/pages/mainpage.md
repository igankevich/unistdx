\mainpage

Unistdx is a C++ library that offers *object-oriented* interface to Linux
system calls. It is built on several core principles:
\arg use UNIX API where possible, but optimise for Linux,
\arg use standard C++ abstractions to map system calls (containers, iterators,
  input/output streams, traits classes, guards etc.),
\arg inherit standard libc structures for maximal transparency,
\arg throw an exception when system call fails,
\arg use asynchronous input/output by default.