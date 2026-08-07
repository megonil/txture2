#include "txr/parser.h"

#include "array.h"
#include "table.h"
#include "txr/chunk.h"
#include "txr/lexer.h"
#include "txr/opcode.h"
#include "txr/token.h"
#include "txr/vmbuiltin.h"
#include "utils.h"

#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define parsef(name) static void name (Parser* parser)
parsef (id_expr);
parsef (binary_r);
parsef (binary_l);
parsef (number);
parsef (unary);
parsef (grouping);
parsef (expression);
parsef (statement);
#undef parsef

typedef enum {
	PNone,
	POr,
	PAnd,
	PBinOr,
	PBinAnd,
	PBinXor,
	PEq,
	PCmp,
	PShift,
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

static ParseRule rules[TTEndTokens];

#define B(Variant, Ch, Str, Prec) rules[Ch] = binar (Prec);
#define U(Variant, Ch, Str, Op) rules[Ch] = unar ();

#define custom(Ch, rule) rules[Ch] = rule

static void
init_parsing_rules ()
{
	for (size_t i = 0; i < TTEndTokens; ++i) rules[i] = empty ();

	BINARY_INSTRUCTIONS
	UNARY_INSTRUCTIONS

	custom ('-', all (unary, binary_r, PTerm));
	custom ('^', infp (binary_l, PPow));
	custom ('(', pref (grouping));
	custom (TTNumber, pref (number));
	custom (TTId, pref (id_expr));
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

extern double	token_value;
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
	copy (parser->prev_buffer, parser->lexer.buffer);
	parser->current = preprocess_lex (&parser->lexer);
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
	VariableSetTableFree (&parser->var_set);
	array_free (parser->prev_buffer);

	free (parser);
}

static size_t
push_name (Parser* parser, const char* s)
{
	char* str = strdup (s);

	push (parser->chunk->strings_to_free, str);
	size_t idx = len (parser->chunk->strings_to_free) - 1;

	VariableSetTableInsert (&parser->var_set, str, idx);
	return idx;
}

#define VAR(Name, Default)                                                \
	VariableSetTableInsert (                                              \
		&parser->var_set, Name, push_name (parser, Name));

#define EVAR(Name, Init) VAR (Name, 0)

static void
builtin_variables (Parser* parser)
{
	BUILTIN_VARIABLES;
	EXTERNAL_BUILTIN_VARIABLES;
}

#undef VAR
#undef EVAR

Parser*
make_parser (const char* source, const char* sourcename, Chunk* ch)
{
	Parser* parser = malloc (sizeof (Parser));
	if (!parser) {
		perror ("cannot allocate parser");
		exit (1);
	}
	if (sourcename != 0) parser->sourcename = sourcename;

	parser->had_error	= 0;
	parser->panic_mode	= 0;
	parser->chunk		= ch;
	parser->prev_buffer = string ();

	lexer_init (&parser->lexer, source);
	ConstantTableInit (&parser->constants);
	VariableSetTableInit (&parser->var_set);

	builtin_variables (parser);

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
		default: unreachable ();
	}
}

#undef U

#define B(Variant, Ch, Str, Prec)                                         \
	case Ch: emit (parser, Variant); break;

static void
binary_impl (Parser* parser, Precedence next, TokenType op)
{
	prec (parser, next);

	switch ((uint) op) {
		BINARY_INSTRUCTIONS;
		default: unreachable ();
	}
}

static inline void
binary_r (Parser* parser)
{ binary_impl (parser, get_rule (prev)->prec + 1, prev); }

static inline void
binary_l (Parser* parser)
{ binary_impl (parser, get_rule (prev)->prec, prev); }

#undef B

static void
id_expr (Parser* parser)
{
	debug (
		"id_expr: prevbuffer=%s, curr=%d, currbuffer=%s",
		parser->prev_buffer,
		curr,
		parser->lexer.buffer);
	const char* varname = parser->prev_buffer;

	size_t idx;
	if (!VariableSetTableContains (&parser->var_set, varname)) {
		idx = push_name (parser, varname);
	} else {
		idx = *VariableSetTableGet (&parser->var_set, varname);
	}

	if (curr == '=') {
		next ();
		expression (parser);
		instruction_with_index (parser->chunk, Set, idx);
	} else {
		instruction_with_index (parser->chunk, Load, idx);
	}
}

static void
statement (Parser* parser)
{
	switch (curr) {
		default: expression (parser);
#ifndef TXTURE2_DEBUG
			emit (parser, Pop);
#endif
	}
}

int
parse (Parser* parser)
{
	debug ("Parser session ==>");
	while (curr != TTEof) statement (parser);

#ifdef TXTURE2_DEBUG
	disassemble (parser->chunk);
#endif
	debug ("==> end");

	int result = !parser->had_error;
	return result;
}
