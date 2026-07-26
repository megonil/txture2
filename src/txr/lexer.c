#include "txr/lexer.h"

#include "array.h"
#include "table.h"
#include "txr/token.h"
#include "utils.h"

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

/// value of float literal
float token_value = 0.0;

/// error which happened during lexing
LexError lexerr = LOK;

#include <ctype.h>

#define K(Kw, Str)                                                        \
	(KeywordTableItem)                                                    \
	{ .key = Str, .value = Kw, }

static KeywordTable keywords;

void
lexer_init (Lexer* l, const char* source)
{
	l->current = source;
	l->lineno  = 1;
	l->buffer  = string ();

	TableInitList (Keyword, &keywords, KEYWORDS);
}

#undef K

void
lexer_free (Lexer* l)
{
	KeywordTableFree (&keywords);
	array_free (l->buffer);
}

#define curr *lexer->current
#define next() *lexer->current++
#define peek(i) *(lexer->current + i)

#define seterr(E)                                                         \
	do {                                                                  \
		lexerr = E;                                                       \
		return TTError;                                                   \
	} while (0)

#define save_next()                                                       \
	do {                                                                  \
		array_push (lexer->buffer, curr);                                 \
		next ();                                                          \
	} while (0)

static TokenType
number (Lexer* lexer)
{
	if (curr == '-' || curr == '+') save_next ();

	uint8_t dot = 0;

	while (isdigit (curr) || curr == '.' || curr == 'e' || curr == 'E') {
		if (curr == '.') {
			if (dot) seterr (DoubleDot);

			dot = 1;
		}

		if (curr == 'e' || curr == 'E') {
			save_next ();

			if (curr == '+' || curr == '-') save_next ();

			continue;
		}

		save_next ();
	}

	int ok;
	token_value = parse_float (lexer->buffer, &ok);
	if (!ok) seterr (WrongFloatLiteral);
	if (isalpha (curr)) seterr (LetterTouchingLetter);

	return TTNumber;
}

typedef TokenType (*lexfunction) (Lexer*);

/// note: this function is not used yet
/// this may be useful in the future
/// try lexing using function f
/// save result of that function to *result
/// function f MUST return TTError
/// return 1 if try is successful
/// return 0 if not
static int
try_lex (Lexer* lexer, TokenType* result, lexfunction f)
{
	char* saved = lexer->current;
	*result		= f (lexer); // try

	if (lexerr != LOK && *result == TTError) {
		// restore
		lexer->current = saved;
		string_clear (lexer->buffer); // just in case
		return 0;
	}

	// success
	return 1;
}

#define isnext_digit() isdigit (peek (1))

TokenType
lex (Lexer* lexer)
{
	string_clear (lexer->buffer);

	unsigned char c = (unsigned char) curr;

	switch (c) {
	case ' ':
	case '\t': next (); return lex (lexer);
	case '\0': return TTEof;

	case '\n':
	case '\r':
		lexer->lineno++;
		next ();
		break;

	default:
		if (isdigit (c) || (c == '+' && isnext_digit ())
			|| (c == '-' && isnext_digit ()))

			return number (lexer);

		next ();
		return (TokenType) c;
	}
}

#undef curr
