#ifndef TXTURE2_TXR_LEXER_H
#define TXTURE2_TXR_LEXER_H

#include "table.h"
#include "txr/macro.h"
#include "txr/token.h"

#include <stddef.h>

typedef struct {
	MacroTable	   macros;
	size_t		   lineno;
	char*		   buffer;
	const char*	   current;
	ExtendedToken* macrobuffer;
} Lexer;

#define LERROR                                                            \
	L (DoubleDot, "double dot in float literal")                          \
	L (WrongFloatLiteral, "wrong float literal")                          \
	L (LetterTouchingNumber, "letter touching number")                    \
	L (UnfinishedMacro, "unfinished macro")

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
