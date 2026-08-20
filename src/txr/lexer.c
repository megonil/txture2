#include "txr/lexer.h"

#include "array.h"
#include "table.h"
#include "txr/macro.h"
#include "txr/macrosbuiltin.h"
#include "txr/token.h"
#include "utils.h"

#include <ctype.h>
#include <stdint.h>

/// value of float literal
double token_value = 0.0;

/// error which happened during lexing
LexError lexerr = LOK;

#define K(Kw, Str)                                                        \
	(KeywordTableItem){                                                   \
		.key   = Str,                                                     \
		.value = TT##Kw,                                                  \
	},

static KeywordTable keywords;

static void
builtin_alias (Lexer* l, char* name_, ExtendedToken code_)
{
	char*		   name = strdup (name_);
	ExtendedToken* code = array (ExtendedToken);
	push (code, code_);

	Macro macro = create_macro (code, 0, MacroAlias);
	MacroTableInsert (&l->macros, name, macro);
}

static void
builtin_macro (
	Lexer*		   l,
	char*		   name_,
	char*		   args_[BUILTIN_MACRO_MAX_ARGS],
	ExtendedToken* code_,
	uint		   code_n)
{
	char* name = strdup (name_);

	char** args = array (char*);

	for (uint i = 0; i < BUILTIN_MACRO_MAX_ARGS && args_[i] != 0; ++i) {
		char* arg = strdup (args_[i]);
		push (args, arg);
	}

	ExtendedToken* code = array (ExtendedToken);
	for (uint i = 0; i < code_n; ++i) { push (code, code_[i]); }

	Macro macro = create_macro (code, args, MacroFunction);
	MacroTableInsert (&l->macros, name, macro);
}

#define BM(Name, Args, Code)                                              \
	builtin_macro (l, #Name, Args, Code, static_len (Code));

#define BA(Name, Code) builtin_alias (l, #Name, Code);

void
lexer_init (Lexer* l, const char* source)
{
	l->current	   = source;
	l->lineno	   = 1;
	l->buffer	   = string ();
	l->macrobuffer = array (ExtendedToken);

	MacroTableInit (&l->macros);
	BUILTIN_MACROS;
	BUILTIN_ALIASES;

	TableInitList (Keyword, &keywords, KEYWORDS);
}

#undef K
#undef BM
#undef BA

void
lexer_free (Lexer* l)
{
	KeywordTableFree (&keywords);

	for (size_t i = 0; i < l->macros.cap; ++i) {
		// just a macro
		MacroTableItem* item = &l->macros.entries[i];

		if (item->key == 0) continue;
		free (item->key);

		if (item->value.args != 0) {
			foreach (item->value.args, i) free (item->value.args[i]);

			array_free (item->value.args);
		}

		// code is must-have
		array_free (item->value.code);
	}

	MacroTableFree (&l->macros);

	array_free (l->buffer);
	array_free (l->macrobuffer);
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
	const unsigned c = (unsigned char) curr;
	return ((c == '+' || c == '-') && isnext_digit ()) || isdigit (c);
}

static inline int
isid_start (Lexer* lexer)
{
	const unsigned c = (unsigned char) curr;
	return (c == '_' && isnext_alnum ()) || isalpha (c);
}

#define M(Ch, Chs, Variant, Ch2, Ch2s)                                    \
	case Ch:                                                              \
		next ();                                                          \
		if (curr == Ch2) {                                                \
			next ();                                                      \
			return TT##Variant;                                           \
		}                                                                 \
		return Ch;

#define A(Ch, Chs, Left, Ls, Right, Rs, V1, V2)                           \
	case Ch:                                                              \
		next ();                                                          \
		if (curr == Left) {                                               \
			next ();                                                      \
			return TT##V1;                                                \
		} else if (curr == Right) {                                       \
			next ();                                                      \
			return TT##V2;                                                \
		}                                                                 \
		return Ch;

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

	switch (c) {
		MULTIPLECHARS;
		AMBIGUOUS_MULTIPLECHARS;
		default: next (); return (TokenType) c;
	}
}

#undef A
#undef M

static void
preconsume (Lexer* lexer, TokenType expected)
{
	TokenType t = lex (lexer);
	if (t == expected) return;

	// I don't know how to deal with this right now
	error (
		"Expected `%s`, found `%s`",
		tok_to_string (expected),
		tok_to_string (t));
}

#define copybuf() strdup (lexer->buffer)
#define prenext() lex (lexer)

static TokenType
extended_extract (Lexer* lexer, ExtendedToken* ext)
{
	if (ext->type == TTNumber) {
		token_value = ext->token_value;
	} else if (ext->type == TTId) {
		copy_static (lexer->buffer, ext->buffer);
	}

	return ext->type;
}

static void
extended_init (Lexer* lexer, TokenType t, ExtendedToken* ext)
{
	ext->type = t;
	if (t == TTNumber) {
		ext->token_value = token_value;
	} else if (t == TTId) {
		strcpy (ext->buffer, lexer->buffer);
	}
}

static void
macrofn (Lexer* lexer)
{
	preconsume (lexer, TTId);
	char* name = copybuf ();
	preconsume (lexer, '(');

	char**		   args = array (char*);
	ExtendedToken* code = array (ExtendedToken);

	TokenType t = lex (lexer);

	if (t == ')') goto break_loop;

	for (;; t = lex (lexer)) {
		char* arg = copybuf ();
		push (args, arg);

		TokenType t = lex (lexer);
		switch ((uint) t) {
			case ',': continue;
			case TTEof:
			case ')': goto break_loop;
		}
	}
break_loop:

	for (t = lex (lexer); t != TTEof && t != TTEnd; t = lex (lexer)) {
		ExtendedToken ext;
		extended_init (lexer, t, &ext);

		if (t == TTFn || t == TTAlias) {
			eprintln ("Defining a macro or alias in macro is forbidden");
			goto clear;
		}

		push (code, ext);
	}

	if (t == TTEof) {
		lexerr = UnfinishedMacro;
		goto clear;
	}

	Macro macro = create_macro (code, args, MacroFunction);
	MacroTableInsert (&lexer->macros, name, macro);
	return; // avoid getting to clear

clear:
	free (name);
	foreach (args, i) { free (args[i]); }

	array_free (args);
	array_free (code);
}

static void
alias (Lexer* lexer)
{
	preconsume (lexer, TTId);
	char* name = copybuf ();

	TokenType t = lex (lexer);

	ExtendedToken* code = array (ExtendedToken);
	ExtendedToken  ext;
	extended_init (lexer, t, &ext);

	push (code, ext);

	Macro macro = create_macro (code, 0, MacroAlias);
	MacroTableInsert (&lexer->macros, name, macro);
}

static int
is_macro_arg (Lexer* lexer, Macro* m, const char* key)
{
	foreach (m->args, i) {
		if (streq (m->args[i], key)) return 1;
	}

	return 0;
}

static void
parse_args (Lexer* lexer, Macro* macro, ArgTable* out)
{
	foreach (macro->args, i) {
		ExtendedToken* buff	 = array (ExtendedToken);
		uint		   depth = 0;

		for (;;) {
			TokenType t = lex (lexer);

			ExtendedToken ext;
			extended_init (lexer, t, &ext);

			if (depth == 0 && (t == ',' || t == ')')) {
				ArgTableInsert (out, macro->args[i], buff);

				if (t == ')') return;

				break;
			}

			switch ((uint) t) {
				case '(': depth++; break;
				case ')': depth--; break;
			}

			push (buff, ext);
		}
	}

	preconsume (lexer, ')');
}

static void
expand (Lexer* lexer, char* macroname)
{
	Macro macro = *MacroTableGet (&lexer->macros, macroname);

	// alias
	if (macro.type == MacroAlias) {
		push (lexer->macrobuffer, macro.code[0]);
		return; // already done
	}

	preconsume (lexer, '(');

	ArgTable arg_table;
	ArgTableInit (&arg_table);
	parse_args (lexer, &macro, &arg_table);

	for (ssize_t i = len (macro.code) - 1; i >= 0; --i) {
		ExtendedToken t = macro.code[i];
		if (t.type == TTId && is_macro_arg (lexer, &macro, t.buffer)) {
			ExtendedToken* arg_tokens
				= *ArgTableGet (&arg_table, t.buffer);
			for (ssize_t j = len (arg_tokens) - 1; j >= 0; --j) {
				push (lexer->macrobuffer, arg_tokens[j]);
			}
		} else {
			push (lexer->macrobuffer, t);
		}
	}

	for (size_t i = 0; i < arg_table.cap; ++i) {
		ArgTableItem* item = &arg_table.entries[i];

		if (item->key == 0) continue;

		array_free (item->value);
	}

	ArgTableFree (&arg_table);
}

TokenType
preprocess_lex (Lexer* lexer)
{
extended:
	if (len (lexer->macrobuffer) != 0) {
		ExtendedToken ext = pop (lexer->macrobuffer);
		if (ext.type == TTId
			&& MacroTableContains (&lexer->macros, ext.buffer)) {
			expand (lexer, ext.buffer);
			goto extended;
		}

		return extended_extract (lexer, &ext);
	}

	// loop to avoid recursion
	for (;;) {
		TokenType t = lex (lexer);
		switch (t) {
			case TTFn: macrofn (lexer); continue;
			case TTAlias: alias (lexer); continue;
			case TTId:
				// if this id is a macro
				if (MacroTableContains (&lexer->macros, lexer->buffer)) {
					expand (lexer, lexer->buffer);
					goto extended;
				}
				// otherwise just ID
			default: return t;
		}
	}
}
