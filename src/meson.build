subdir('formats')
subdir('osc')

inc = include_directories('../include')

writer_lib = static_library('writer', ['writer.c'], include_directories: [inc])

libs = [formats_lib, osc_lib, writer_lib]

executable(
	'txture2',
	'main.c',
	link_with: libs,
	install: true,
	include_directories: [inc],
)
