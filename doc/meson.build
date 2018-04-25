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
