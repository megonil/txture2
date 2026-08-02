#include "txr/lexer.h"

#include "array.h"
#include "table.h"
#include "txr/token.h"
#include "utils.h"

#include <ctype.h>
#include <stdint.h>

/// value of float literal
double token_value = 0.0;

/// error which happened during lexing
LexError lexerr = LOK;

#define K(Kw, Str)                                                        \
	(KeywordTableItem)                                                    \
	{ .key = Str, .value = TT##Kw, }

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
		push (lexer->buffer, curr);                                       \
		next ();                                                          \
	} while (0)

#define clear_buffer() array_clear (lexer->buffer)

/// lex number
/// store result in token_value
static TokenType
number (Lexer* lexer)
{
	clear_buffer ();
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
	token_value = parse_double (lexer->buffer, &ok);
	if (!ok) seterr (WrongFloatLiteral);
	if (isalpha (curr)) seterr (LetterTouchingNumber);

	return TTNumber;
}

static inline int
isid_char (char ch)
{
	const unsigned char c = (unsigned char) ch;
	return isalnum (c) || c == '_';
}

/// lex keyword OR identifier
/// if it is keyword returns it's TokenType
/// if it is identifier stores it in lexer->buffer
/// and returns TTId
static TokenType
keyword_or_identifier (Lexer* lexer)
{
	clear_buffer ();
	save_next (); // start of id

	while (isid_char (curr)) save_next ();
	push (lexer->buffer, '\0'); // end the string

	if (KeywordTableContains (&keywords, lexer->buffer)) {
		return *KeywordTableGet (&keywords, lexer->buffer);
	}

	return TTId;
}

typedef TokenType (*lexfunction) (Lexer*);

#define isnext(f) f ((unsigned char) peek (1))
#define isnext_digit() isnext (isdigit)
#define isnext_alnum() isnext (isalnum)

/// Skip whitespaces and comments
static inline void
skip (Lexer* lexer)
{
	for (;;) {
		while (isspace ((unsigned char) curr)) {
			if (curr == '\n') lexer->lineno++;

			next ();
		}

		if (curr == '#') {
			do {
				next ();
			} while (curr != '\n' && curr != '\0');

			continue;
		}

		break;
	}
}

static inline int
isnumber_start (Lexer* lexer)
{
	const char c = (unsigned char) curr;
	return ((c == '+' || c == '-') && isnext_digit ()) || isdigit (c);
}

static inline int
isid_start (Lexer* lexer)
{
	const char c = (unsigned char) curr;
	return (c == '_' && isnext_alnum ()) || isalpha (c);
}

TokenType
lex (Lexer* lexer)
{
	skip (lexer);

	// c is guaranteed to be either
	// '\0' or not-whitespace regular character
	const unsigned char c = (unsigned char) curr;

	if (c == '\0') return TTEof;

	if (isnumber_start (lexer)) return number (lexer);
	if (isid_start (lexer)) return keyword_or_identifier (lexer);

	next ();
	return (TokenType) c;
}
