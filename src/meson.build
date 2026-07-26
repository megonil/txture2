# Setup
incs = [include_directories('../include')]
cc = meson.get_compiler('c')
m_dep = cc.find_library('m') # math library, used by some modules

# Libraries
subdir('cargs')
subdir('formats')
subdir('osc')
subdir('txr')

writer_lib = static_library('writer', ['writer.c'], include_directories: incs)
color_lib = static_library('color', ['color.c'], include_directories: incs)
cli_lib = static_library('cli', ['cli.c'], include_directories: incs, link_with: cargs_lib)
array_lib = static_library('array', ['array.c'], include_directories: incs)
table_lib = static_library('table', ['table.c'], include_directories: incs)

libs = [
	formats_lib,
	osc_lib,
	writer_lib,
	color_lib,
	cli_lib,
	txr_lib,
	table_lib,
	array_lib,
]

executable(
	'txture2',
	'main.c',
	link_with: libs,
	install: true,
	include_directories: incs,
)

# Testing

unity_dep = dependency('unity') # subproject
test_src = files('../test/array.c', '../test/main.c', '../test/table.c')
test_exe = executable(
	'txture2test',
	test_src,
	dependencies: unity_dep,
	link_with: libs,
	include_directories: incs,
)

test('txture2', test_exe)
