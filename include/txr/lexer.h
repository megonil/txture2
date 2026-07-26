#ifndef TXTURE2_TXR_LEXER_H
#define TXTURE2_TXR_LEXER_H

#include "txr/token.h"

#include <stddef.h>

typedef struct {
	size_t		lineno;
	char*		buffer;
	const char* current;
} Lexer;

#define LERROR                                                            \
	L (DoubleDot, "double dot in float literal")                          \
	L (WrongFloatLiteral, "wrong float literal")                          \
	L (LetterTouchingLetter, "letter touching letter")

#define L(E, M) E,
typedef enum { LOK = -1, LERROR } LexError;
#undef L

#define L(E, M) M,
static const char* const lerrorstrs[] = {LERROR};
#undef L

#define lerr_to_str(e) lerrorstrs[e]

void
lexer_init (Lexer* l, const char* source);

TokenType
lex (Lexer* lexer);

TokenType
preprocess_lex (Lexer* lexer);

void
lexer_free (Lexer* l);

#endif // !TXTURE2_TXR_LEXER_H
