\page execute Execute child process with arguments and environment (WIP)

Use the following code to execute \c errno command to display error message in
Russian locale.

\snippet execute.cc Execute child process

The above code is equivalent to running

    env LANG=ru_RU.UTF-8 errno 42

command in shell.

\unistdx_example_link{execute,cc}
