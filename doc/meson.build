doxygen = find_program('doxygen', required: false)
if doxygen.found()
	run_target(
		'doc-doxygen',
		command: [doxygen, join_paths(meson.current_source_dir(), 'Doxyfile')]
	)
	run_target(
		'doc-temp',
		command: [
			doxygen,
			'-w',
			'html',
			join_paths(meson.build_root(), 'header.html'),
			join_paths(meson.build_root(), 'footer.html'),
			join_paths(meson.build_root(), 'doxygen.css'),
			join_paths(meson.current_source_dir(), 'Doxyfile')
		]
	)
	run_target(
		'doc-rename',
		command: join_paths(meson.current_source_dir(), 'rename')
	)
	run_target('doc', command: join_paths(meson.current_source_dir(), 'all'))
endif

# doxygen tag files {{{
std_tagfile_name = 'cppreference-doxygen-web.tag.xml'
std_tagfile_prefix = 'http://upload.cppreference.com/mwiki/images/f/f8/'
std_tagfile_url = std_tagfile_prefix + std_tagfile_name
wget = find_program('wget', required: false)
curl = find_program('curl', required: false)
if wget.found()
	run_target(
		'doc-std-tagfile',
		command: [
			wget,
			'-O',
			join_paths(meson.build_root(), 'std.tag.xml'),
			std_tagfile_url
		]
	)
elif curl.found()
	run_target(
		'doc-std-tagfile',
		command: [
			curl,
			'-v',
			'-o',
			join_paths(meson.build_root(), 'std.tag.xml'),
			std_tagfile_url
		]
	)
endif
# }}}

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
