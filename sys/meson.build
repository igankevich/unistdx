libunistdx_src = [
	'system.cc'
]

libunistdx_shared = shared_library(
	'unistdx',
	sources: libunistdx_src,
	install: true
)
libunistdx_static = static_library(
	'unistdx',
	sources: libunistdx_src,
	install: true
)

unistdx = declare_dependency(
	link_with: libunistdx_static,
	include_directories: include_directories('.')
)
