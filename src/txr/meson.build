lexer_lib = static_library('lexer', ['lexer.c'], include_directories: incs)

source = files('constant.c', 'value.c', 'vm.c')

txr_libs = [lexer_lib]

txr_lib = static_library('txr', source, include_directories: incs, link_with: txr_libs)
