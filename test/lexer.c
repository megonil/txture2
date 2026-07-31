#include "txr/lexer.h"

#include "test.h"
#include "txr/token.h"
#include "utils.h"

extern LexError lexerr;
extern float	token_value;

#define prelude(source)                                                   \
	Lexer l;                                                              \
	lexer_init (&l, source);                                              \
	TokenType t

#define lex_one() t = lex (&l)

void
check_error ()
{
	if (lexerr != LOK) {
		eprintln ("lexer error: %s", lerr_to_str (lexerr));
	}
}

#define expectT(T) TEST_ASSERT_EQUAL_INT (T, t)
#define expectF(F) TEST_ASSERT_EQUAL_FLOAT (F, token_value)
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
	expect_lit (123.0f);

	eof ();
	end ();
}

t (lexer_float)
{
	prelude ("123.53189");
	expect_lit (123.53189f);

	eof ();
	end ();
}

t (lexer_exponential)
{
	prelude ("123e-10 123e+10");
	expect_lit (123e-10f);
	expect_lit (123e+10f);

	eof ();
	end ();
}

t (lexer_signed)
{
	prelude ("-150.0 -150e-10 -00000.0 +00000.0");
	expect_lit (-150.0f);
	expect_lit (-150e-10f);
	expect_lit (0.0f);
	expect_lit (0.0f);

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
	expect_lit (123.0f);
	eof ();

	expect_lineno (3);
	end ();
}

t (lexer_keyword)
{
	prelude ("alias");
	expect (TTAlias);

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
	expect_lit (123.5f);
	expect ('+');
	expect_lit (5690e-10);

	eof ();
	end ();
}

t (lexer_complex_numbers)
{
	prelude ("-1234.56 + 12345 * 500");
	expect_lit (-1234.56f);
	expect ('+');
	expect_lit (12345);
	expect ('*');
	expect_lit (500);

	eof ();
	end ();
}
