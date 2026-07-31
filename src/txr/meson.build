lexer_lib = static_library('lexer', ['lexer.c', 'token.c'], include_directories: incs)
parser_lib = static_library('parser', ['parser.c'], include_directories: incs, link_with: [lexer_lib])

source = files('chunk.c', 'compiler.c', 'value.c', 'vm.c')

txr_libs = [parser_lib]

txr_lib = static_library('txr', source, include_directories: incs, link_with: txr_libs)
