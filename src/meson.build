src = include_directories('.')

subdir('valgrind')
if with_debug or with_backtrace or with_tests
    #subdir('unwindx')
    subdir('dwx')
endif
subdir('unistdx')
