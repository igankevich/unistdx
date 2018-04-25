doxygen = find_program('doxygen', required: false)
if doxygen.found()
	run_target(
		'doc',
		command: [doxygen, join_paths(meson.current_source_dir(), 'Doxyfile')]
	)
endif

xsltproc = find_program('xsltproc', required: false)
if xsltproc.found()
	run_target('man', command: join_paths(meson.current_source_dir(), 'man'))
endif

sphinx_build = find_program('sphinx-build', required: false)
if not sphinx_build.found()
	sphinx_build = find_program('sphinx-build-3', required: false)
endif
if sphinx_build.found()
	run_target(
		'sphinx-html',
		command: [
			sphinx_build,
			'-M',
			'html',
			meson.current_source_dir(),
			meson.current_build_dir(),
		]
	)
endif
