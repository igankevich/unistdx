(use-package-modules build-tools check code commencement curl documentation
                     gcc gcovr graphviz ninja pkg-config pre-commit)

(packages->manifest
 (list
  meson
  ninja
  googletest
  (list gcc "lib")
  gcc-toolchain
  pkg-config
  python-pre-commit
  curl graphviz doxygen
  lcov python-gcovr))
