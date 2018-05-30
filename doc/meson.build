# doxygen tag files {{{
std_tagfile_name = 'cppreference-doxygen-web.tag.xml'
std_tagfile_prefix = 'http://upload.cppreference.com/mwiki/images/f/f8/'
std_tagfile_url = std_tagfile_prefix + std_tagfile_name
std_tag_xml = join_paths(meson.build_root(), 'std.tag.xml')
std_tag_url = 'http://en.cppreference.com/w/'
wget = find_program('wget', required: false)
curl = find_program('curl', required: false)
if wget.found()
	run_target(
		'doc-std-tagfile',
		command: [wget, '-O', std_tag_xml, std_tagfile_url]
	)
elif curl.found()
	run_target(
		'doc-std-tagfile',
		command: [curl, '-v', '-o', std_tag_xml, std_tagfile_url]
	)
endif
# }}}
# generate Doxyfile {{{
cwd = meson.current_source_dir()
examples = join_paths(cwd, 'examples')
doxyfile = configuration_data()
doxyfile.set('EXAMPLE_PATH', examples)
doxyfile.set('LAYOUT_FILE', join_paths(cwd, 'layout.xml'))
doxyfile.set('OUTPUT_DIRECTORY', meson.build_root())
doxyfile.set('TAGFILES', std_tag_xml + '=' + std_tag_url)
excluded_files = [
	'meson.build',
	join_paths(meson.source_root(), 'src', 'unistdx', 'bits'),
	join_paths(meson.source_root(), 'src', 'unistdx', 'test')
]
doxyfile.set('EXCLUDE', ' '.join(excluded_files))
input_files = [
	join_paths(meson.source_root(), 'src', 'unistdx'),
	examples,
	join_paths(meson.current_source_dir(), 'pages'),
]
doxyfile.set('INPUT', ' '.join(input_files))
perl = find_program('perl', required: false)
if perl.found()
	doxyfile.set('PERL_PATH', perl.path())
endif
dot = find_program('dot', required: false)
if dot.found()
	doxyfile.set('DOT_PATH', dot.path())
	doxyfile.set('HAVE_DOT', 'YES')
else
	doxyfile.set('HAVE_DOT', 'NO')
endif
configure_file(
	input: 'Doxyfile.in',
	output: 'Doxyfile',
	configuration: doxyfile
)
# }}}

doxygen = find_program('doxygen', required: false)
if doxygen.found()
	run_target(
		'doc-doxygen',
		command: [doxygen, join_paths(meson.current_build_dir(), 'Doxyfile')]
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
			join_paths(meson.current_build_dir(), 'Doxyfile')
		]
	)
	run_target(
		'doc-rename',
		command: join_paths(meson.current_source_dir(), 'rename')
	)
	run_target('doc', command: join_paths(meson.current_source_dir(), 'all'))
endif


xsltproc = find_program('xsltproc', required: false)
if xsltproc.found()
	run_target('man', command: join_paths(meson.current_source_dir(), 'man'))
endif
