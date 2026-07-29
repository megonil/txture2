#ifndef TXTURE2_TXR_PARSER_H
#define TXTURE2_TXR_PARSER_H

#include "table.h"
#include "txr/chunk.h"
#include "txr/lexer.h"
#include "txr/token.h"

typedef struct {
	VariableTable vars;

	Lexer	  lexer;
	TokenType current, previous;
} Parser;

Parser*
parser (const char* source);

Chunk*
parse (Parser* parser);

void
parser_free (Parser* parser);

#endif // !TXTURE2_TXR_PARSER_H
