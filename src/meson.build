# Setup
cc = meson.get_compiler('c')

incs = [include_directories('../include')]
m_dep = cc.find_library('m') # math library, used by some modules

# Libraries
subdir('formats')
subdir('osc')

writer_lib = static_library('writer', ['writer.c'], include_directories: incs)
color_lib = static_library('color', ['color.c'], include_directories: incs)

libs = [formats_lib, osc_lib, writer_lib, color_lib]

executable(
	'txture2',
	'main.c',
	link_with: libs,
	install: true,
	include_directories: incs,
)
