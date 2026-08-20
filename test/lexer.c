#include "txr/lexer.h"

#include "test.h"
#include "txr/macrosbuiltin.h"
#include "txr/token.h"
#include "unity.h"
#include "utils.h"

extern LexError lexerr;
extern double	token_value;

#define prelude(source)                                                   \
	Lexer l;                                                              \
	lexer_init (&l, source);                                              \
	TokenType t

#define prelex() t = preprocess_lex (&l)
#define lex_one() t = lex (&l)

void
check_error ()
{
	if (lexerr != LOK) {
		eprintln ("lexer error: %s", lerr_to_str (lexerr));
	}
}

static void
expectedT (TokenType expected, TokenType found, int line)
{
	if (expected != found) {
		char buff[64];
		snprintf (
			buff,
			64,
			"expected %s, found %s",
			tok_to_string (expected),
			tok_to_string (found));

		UnityFail (buff, line);
	}
}

#define expectT(T) expectedT (T, t, __LINE__);
#define expectF(F) TEST_ASSERT_DOUBLE_WITHIN (1e-12, F, token_value)
#define expectE(E)                                                        \
	TEST_ASSERT_EQUAL_INT (E, lexerr);                                    \
	check_error ()

#define expect(T)                                                         \
	lex_one ();                                                           \
	expectT (T)

#define good() expectE (LOK)
#define expect_lit(F)                                                     \
	lex_one ();                                                           \
	expectT (TTNumber);                                                   \
	good ();                                                              \
	expectF (F)

#define fail(E)                                                           \
	lex_one ();                                                           \
	TEST_ASSERT_EQUAL_INT (TTError, t);                                   \
	expectE (E)

#define eof() expect (TTEof)

#define expect_lineno(L) TEST_ASSERT_EQUAL_INT (L, l.lineno)

#define id(s)                                                             \
	lex_one ();                                                           \
	good ();                                                              \
	expectT (TTId);                                                       \
	TEST_ASSERT_EQUAL_STRING (s, l.buffer)

#define end() lexer_free (&l);

#define char(c)                                                           \
	lex_one ();                                                           \
	good ();                                                              \
	expectT ((TokenType) c)

t (lexer_character)
{
	prelude ("() * = + - / $");
	char ('(');
	char (')');
	char ('*');
	char ('=');
	char ('+');
	char ('-');
	char ('/');
	char ('$');

	eof ();
	end ();
}

t (lexer_integer)
{
	prelude ("123");
	expect_lit (123.0);

	eof ();
	end ();
}

t (lexer_float)
{
	prelude ("123.53189");
	expect_lit (123.53189);

	eof ();
	end ();
}

t (lexer_exponential)
{
	prelude ("123e-10 123e+10");
	expect_lit (123e-10);
	expect_lit (123e+10);

	eof ();
	end ();
}

t (lexer_signed)
{
	prelude ("-150.0 -150e-10 -00000.0 +00000.0");
	expect_lit (-150.0);
	expect_lit (-150e-10);
	expect_lit (0.0);
	expect_lit (0.0);

	eof ();
	end ();
}

t (lexer_eof)
{
	prelude ("");

	eof ();
	end ()
}

t (lexer_spaces)
{
	prelude ("      \t\n\r");

	eof ();
	end ()
}

t (lexer_comments)
{
	prelude ("# This is a comment!\n 123 \n # This is also a comment!");
	expect_lit (123.0);
	eof ();

	expect_lineno (3);
	end ();
}

#define K(Kw, Str) Str " "

t (lexer_keyword)
{
	prelude (KEYWORDS);
#undef K

#define K(Kw, Str) expect (TT##Kw);

	KEYWORDS

#undef K

	eof ();
	end ();
}

t (lexer_id)
{
	prelude ("abcd _someide234ntifier");
	id ("abcd");
	id ("_someide234ntifier");

	eof ();
	end ();
}

t (lexer_binary)
{
	prelude ("123.5 + 5690e-10");
	expect_lit (123.5);
	expect ('+');
	expect_lit (5690e-10);

	eof ();
	end ();
}

t (lexer_complex_numbers)
{
	prelude ("-1234.56 + 12345 * 500");
	expect_lit (-1234.56);
	expect ('+');
	expect_lit (12345);
	expect ('*');
	expect_lit (500);

	eof ();
	end ();
}

// Preprocessing tests

#undef expect
#undef expect_lit
#undef fail
#undef id

#define expect(T)                                                         \
	prelex ();                                                            \
	expectT (T)

#define create_ext()                                                      \
	(lex_one (),                                                          \
	 (ExtendedToken){                                                     \
		 .type = t, .buffer = l.buffer, .value = token_value})

#define good() expectE (LOK)
#define expect_lit(F)                                                     \
	prelex ();                                                            \
	expectT (TTNumber);                                                   \
	good ();                                                              \
	expectF (F)

#define fail(E)                                                           \
	prelex ();                                                            \
	TEST_ASSERT_EQUAL_INT (TTError, t);                                   \
	expectE (E)

#define id(s)                                                             \
	prelex ();                                                            \
	good ();                                                              \
	expectT (TTId);                                                       \
	TEST_ASSERT_EQUAL_STRING (s, l.buffer)

t (lexer_preprocess_alias)
{
	prelude ("alias A 1230 X = A");
	id ("X");
	expect ('=');
	expect_lit (1230.0);
	eof ();

	end ();
}

t (lexer_preprocess_macrofn)
{
	prelude ("fn macro() 123 end macro() 1258951251");
	expect_lit (123.0);
	expect_lit (1258951251.0);

	eof ();
	end ();
}

t (lexer_preprocess_macrofn_arguments)
{
	prelude ("fn add(x, y) x + y end add(5, 2)");
	expect_lit (5.0);
	expect ('+');
	expect_lit (2.0);

	eof ();
	end ();
}

t (lexer_preprocess_macrofn_depth)
{
	prelude (
		"alias CONSTANT 200.589\n fn constant_mul(X) X * CONSTANT end\n "
		"constant_mul(50.0)");
	expect_lit (50.0);
	expect ('*');
	expect_lit (200.589);

	eof ();
	end ();
}

t (lexer_preprocess_macrofn_arguments_depth)
{
	prelude ("fn add(x, y) x + y end add((5 + 8) * 10, 30)");
	expect ('(');
	expect_lit (5.0);
	expect ('+');
	expect_lit (8.0);
	expect (')');
	expect ('*');
	expect_lit (10.0);
	expect ('+');
	expect_lit (30.0);

	eof ();
	end ();
}

#define M(Ch, Chs, Variant, Ch2, Ch2s) Chs Ch2s " "

t (lexer_multiplechars)
{
	prelude (MULTIPLECHARS);
#undef M

#define M(Ch, Chs, Variant, Ch2, Ch2s) expect (TT##Variant);

	MULTIPLECHARS;

#undef M

	eof ();
	end ();
}

#define A(Ch, Chs, Left, Ls, Right, Rs, V1, V2) Chs Ls " " Chs Rs " "

t (lexer_ambiguous_multiplechars)
{
	prelude (AMBIGUOUS_MULTIPLECHARS);

#undef A

#define A(Ch, Chs, Left, Ls, Right, Rs, V1, V2)                           \
	expect (TT##V1);                                                      \
	expect (TT##V2);

	AMBIGUOUS_MULTIPLECHARS

#undef A

	eof ();
	end ();
}

#define BA(Name, Code) #Name " "

void
expect_ext_impl (Lexer* l, TokenType t, ExtendedToken x)
{
	good ();
	TEST_ASSERT_EQUAL_UINT (x.type, t);

	if (x.type == TTId) {
		TEST_ASSERT_EQUAL_STRING (x.buffer, l->buffer);
	} else if (x.type == TTNumber) {
		TEST_ASSERT_EQUAL_DOUBLE (x.token_value, token_value);
	}
}

#define expect_ext(Ext)                                                   \
	prelex ();                                                            \
	expect_ext_impl (&l, t, Ext)

t (lexer_builtin_aliases)
{
	prelude (BUILTIN_ALIASES);
#undef BA
#define BA(Name, Code) expect_ext ((Code));
	BUILTIN_ALIASES
#undef BA

	eof ();
	end ();
}

t (lexer_builtin_macros)
{
	prelude ("expand_v(100)");
	expect_lit (100.0);
	expect ('*');
	id ("M");

	eof ();
	end ();
}

t (lexer_macros_random)
{
	prelude ("expand_v(sin(PI/2))");

	id ("sin");
	expect ('(');
	expect_lit (M_PI);
	expect ('/');
	expect_lit (2);
	expect (')');
	expect ('*');
	id ("M");

	eof ();
	end ();
}
