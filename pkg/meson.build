# archive {{{
archive_name = meson.project_name() + '-' + meson.project_version()
full_archive_name = archive_name + '.tar.xz'
run_target(
	'archive',
	command: [
		'git',
		'-C',
		meson.source_root(),
		'archive',
		'-o',
		join_paths(meson.build_root(), full_archive_name),
		'--prefix',
		archive_name + '/',
		'HEAD'
	]
)
# }}}
# RPM spec {{{
rpm_spec = configuration_data()
rpm_spec.set('name', meson.project_name())
rpm_spec.set('version', meson.project_version())
rpm_spec.set('release', '1')
rpm_spec.set('archive_name', full_archive_name)
rpm_spec.set('arch', build_machine.cpu_family())
rpm_spec.set('license', meson.project_license())
configure_file(
	input: 'unistdx.spec.in',
	output: 'unistdx.spec',
	configuration: rpm_spec
)
# }}}
# RPM build {{{
configure_file(
	input: 'rpm.in',
	output: 'rpm',
	configuration: rpm_spec
)
rpm_setup_tree = find_program('rpmdev-setuptree', required: false)
if not rpm_setup_tree.found()
	rpm_setup_tree = find_program('rpmsetuptree', required: false)
endif
if rpm_setup_tree.found()
	run_target('rpm-setup-tree', command: rpm_setup_tree)
endif
rpm_build = find_program('rpmbuild', required: false)
if rpm_build.found()
	run_target(
		'rpm-build',
		command: [
			rpm_build,
			'-ba',
			join_paths(meson.current_build_dir(), 'unistdx.spec')
		]
	)
endif
run_target(
	'rpm',
	command: join_paths(meson.current_build_dir(), 'rpm')
)
# }}}
