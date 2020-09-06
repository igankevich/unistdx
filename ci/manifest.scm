(define unistdx-manifest
  (let ((v (getenv "UNISTDX_MANIFEST")))
    (if v (string-split v #\,) '())))

(define (if-enabled name lst)
  (if (member name unistdx-manifest) lst '()))

(packages->manifest
 (append
  (list
   (@ (gnu packages build-tools) meson)
   (@ (stables packages ninja) ninja/lfs)
   (@ (gnu packages check) googletest)
   (list (@ (gnu packages gcc) gcc) "lib")
   (@ (gnu packages commencement) gcc-toolchain)
   (@ (gnu packages pkg-config) pkg-config)
   (@ (gnu packages pre-commit) python-pre-commit)
   (@ (gnu packages python-xyz) python-chardet)
   (@ (gnu packages code) lcov)
   (@ (gnu packages gcovr) python-gcovr))
  (if-enabled "site"
              (list (@ (gnu packages curl) curl)
                    (@ (gnu packages graphviz) graphviz)
                    (@ (gnu packages documentation) doxygen)
                    (@ (gnu packages perl) perl)
                    (@ (gnu packages tex) texlive-bin)
                    (@ (gnu packages guile) guile-3.0)
                    (@ (gnu packages guile-xyz) haunt)
                    (@ (gnu packages guile-xyz) guile-syntax-highlight)
                    (@ (gnu packages groff) groff)
                    (@ (gnu packages xml) libxslt)
                    ))
  (if-enabled "ci"
              (list (@ (gnu packages rsync) rsync)
                    (@ (gnu packages ssh) openssh)
                    (@ (gnu packages base) coreutils)
                    (@ (gnu packages base) findutils) ;; lcov needs find
                    (@ (gnu packages bash) bash)))))
