#include "txr/parser.h"

#include "table.h"
#include "txr/chunk.h"
#include "txr/opcode.h"
#include "txr/token.h"
#include "utils.h"

#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define parsef(name) static void name (Parser* parser)
parsef (binary_r);
parsef (binary_l);
parsef (number);
parsef (unary);
parsef (grouping);
parsef (expression);
#undef parsef

typedef enum {
	PNone,
	POr,
	PAnd,
	PEq,
	PCmp,
	PTerm,
	PFactor,
	PPow,
	PUnary,
	PPrimary,
} Precedence;

typedef void (*ParseFn) (Parser* parser);

typedef struct {
	ParseFn	   prefix;
	ParseFn	   infix;
	Precedence prec;
} ParseRule;

#define pr() (ParseRule)

#define prefp(f, p)                                                       \
	pr ()                                                                 \
	{ f, 0, p }
#define pref(f) prefp (f, PNone)
#define infp(f, p)                                                        \
	pr ()                                                                 \
	{ 0, f, p }
#define inf(f) infp (f, PNone)
#define binar(p) infp (binary_r, p)
#define unar() prefp (unary, PUnary)
#define all(i, f, p)                                                      \
	pr ()                                                                 \
	{ i, f, p }
#define empty()                                                           \
	pr ()                                                                 \
	{ 0, 0, PNone }

static ParseRule rules[TTEnd];

#define B(Variant, Ch, Str, Prec) rules[Ch] = binar (Prec);
#define U(Variant, Ch, Str, Op) rules[Ch] = unar ();

#define custom(Ch, rule) rules[Ch] = rule

static void
init_parsing_rules ()
{
	for (size_t i = 0; i < TTEnd; ++i) rules[i] = empty ();

	BINARY_INSTRUCTIONS
	UNARY_INSTRUCTIONS

	custom ('-', all (unary, binary_r, PTerm));
	custom ('^', infp (binary_l, PPow));
	custom ('(', pref (grouping));
	custom (TTNumber, pref (number));
}

static ParseRule*
get_rule (TokenType t)
{ return &rules[t]; }

#undef B
#undef U
#undef empty
#undef binar
#undef unar
#undef inf
#undef infp
#undef pref
#undef prefp
#undef custom
#undef all

extern float	token_value;
extern LexError lexerr;

static void
print_lexerror (Parser* parser)
{
	if (lexerr != LOK || parser->current == TTError) {
		eprintln (
			"%s at %s:%zu",
			lerr_to_str (lexerr),
			parser->sourcename,
			parser->lexer.lineno);

		parser->had_error  = 1;
		parser->panic_mode = 1;
	}
}

static void
move_tokens (Parser* parser)
{
	parser->previous = parser->current;
	parser->current	 = lex (&parser->lexer);
	debug ("With %s", tok_to_string (parser->current));

	print_lexerror (parser);
}

#define next() move_tokens (parser)

#define curr parser->current
#define prev parser->previous

void
parser_free (Parser* parser)
{
	lexer_free (&parser->lexer);
	ConstantTableFree (&parser->constants);
	VariableTableFree (&parser->vars);

	free (parser);
}

Parser*
make_parser (const char* source, const char* sourcename, Chunk* ch)
{
	Parser* parser = malloc (sizeof (Parser));
	// TODO: check for memory
	if (sourcename != 0) parser->sourcename = sourcename;

	parser->had_error  = 0;
	parser->panic_mode = 0;
	parser->chunk	   = ch;

	lexer_init (&parser->lexer, source);
	ConstantTableInit (&parser->constants);
	VariableTableInit (&parser->vars);

	next (); // initialize tokens

	init_parsing_rules ();
	return parser;
}

static void
simple_error (Parser* parser, const char* msg)
{
	eprintln (
		"syntax error: %s at %s:%zu",
		msg,
		parser->sourcename,
		parser->lexer.lineno);
}

static void
rich_error (Parser* parser, const char* fmt, ...)
{
	if (parser->panic_mode) return;
	parser->panic_mode = 1;
	parser->had_error  = 1;

	char buffer[128];

	va_list args;
	va_start (args, fmt);
	vsnprintf (buffer, sizeof (buffer), fmt, args);
	va_end (args);

	simple_error (parser, buffer);
}

// helpers
static void
consume (Parser* parser, TokenType t)
{
	if (curr != t)
		rich_error (
			parser,
			"expected `%s`, but got `%s`",
			tok_to_string (t),
			tok_to_string (curr));

	else
		next ();
}

static void
add_constant (Parser* parser, tvalue v)
{
	if (ConstantTableContains (&parser->constants, v)) {
		raw_constant (
			parser->chunk, *ConstantTableGet (&parser->constants, v));
	} else {
		size_t index = constant (parser->chunk, v);
		ConstantTableInsert (&parser->constants, v, index);
	}
}

static void
emit (Parser* parser, uint8_t bte)
{ byte (parser->chunk, bte); }

static void
number (Parser* parser)
{ add_constant (parser, token_value); }

static void
grouping (Parser* parser)
{
	expression (parser);
	consume (parser, ')');
}

static void
prec (Parser* parser, Precedence prec)
{
	next ();
	ParseFn pref = get_rule (prev)->prefix;
	if (pref == 0) {
		simple_error (parser, "Expected expression");
		return;
	}

	pref (parser);

	while (prec <= get_rule (curr)->prec
		   && get_rule (curr)->prec > PNone) {
		next ();
		ParseFn inf = get_rule (prev)->infix;
		inf (parser);
	}
}

static void
expression (Parser* parser)
{ prec (parser, PNone); }

#define U(Variant, Ch, Str, Op)                                           \
	case Ch: emit (parser, Variant); break;

static void
unary (Parser* parser)
{
	TokenType operator = prev;

	// operand
	prec (parser, PUnary);

	switch ((uint8_t) operator) {
		UNARY_INSTRUCTIONS;
	default: __builtin_unreachable ();
	}
}

#undef U

#define B(Variant, Ch, Str, Prec)                                         \
	case Ch: emit (parser, Variant); break;

static void
binary_impl (Parser* parser, Precedence next, TokenType op)
{
	prec (parser, next);

	switch ((uint8_t) op) {
		BINARY_INSTRUCTIONS;
	default: __builtin_unreachable ();
	}
}

static inline void
binary_r (Parser* parser)
{ binary_impl (parser, get_rule (prev)->prec + 1, prev); }

static inline void
binary_l (Parser* parser)
{ binary_impl (parser, get_rule (prev)->prec, prev); }

#undef B

int
parse (Parser* parser)
{
	debug ("Parser session ==>");
	while (curr != TTEof) { expression (parser); }

#ifdef TXTURE2_DEBUG
	disassemble (parser->chunk);
#endif
	debug ("==> end");

	int result = !parser->had_error;
	return result;
}
