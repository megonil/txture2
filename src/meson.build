if get_option('txture2_debug')
	add_project_arguments('-DTXTURE2_DEBUG', language: ['c'])
endif

# Setup
incs = [include_directories('../include')]
cc = meson.get_compiler('c')
m_dep = cc.find_library('m') # math library, used by some modules

# Libraries
subdir('cargs')
subdir('formats')
subdir('osc')
subdir('txr')

utils_lib = static_library('utils', ['utils.c'], include_directories: incs)
writer_lib = static_library('writer', ['writer.c'], include_directories: incs)
color_lib = static_library('color', ['color.c'], include_directories: incs)
cli_lib = static_library('cli', ['cli.c'], include_directories: incs, link_with: cargs_lib)
array_lib = static_library('array', ['array.c'], include_directories: incs)
table_lib = static_library('table', ['table.c'], include_directories: incs)

libs = [
	utils_lib,
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

unity_dep = dependency('unity', fallback: ['unity', 'unity_dep'])
test_src = files(
	'../test/array.c',
	'../test/chunk.c',
	'../test/lexer.c',
	'../test/main.c',
	'../test/parser.c',
	'../test/table.c',
	'../test/vm.c',
)
test_exe = executable(
	'txture2test',
	test_src,
	dependencies: unity_dep,
	link_with: libs,
	include_directories: incs,
	c_args: [
		'-DUNITY_INCLUDE_DOUBLE',
		'-DUNITY_DOUBLE_PRECISION=1e-12',
	],
)

test('txture2', test_exe)
