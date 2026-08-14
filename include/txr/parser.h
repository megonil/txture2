#ifndef TXTURE2_TXR_PARSER_H
#define TXTURE2_TXR_PARSER_H

#include "table.h"
#include "txr/chunk.h"
#include "txr/lexer.h"
#include "txr/token.h"

typedef struct {
	Lexer lexer;

	ConstantTable	 constants;
	VariableSetTable var_set;
	PragmaTable		 pragma_table;

	const char* sourcename;
	char*		prev_buffer;
	Chunk*		chunk;

	TokenType current, previous;

	uint8_t panic_mode, had_error;
} Parser;

Parser*
make_parser (const char* source, const char* sourcename, Chunk* ch);

// parse
// returns 0 if something ain't right
int
parse (Parser* parser);

void
parser_free (Parser* parser);

#endif // !TXTURE2_TXR_PARSER_H
