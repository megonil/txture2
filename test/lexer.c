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

#define end() lexer_free (&l);

void
test_lexer_integer ()
{
	prelude ("123");
	expect_lit (123.0f);

	end ();
}

void
test_lexer_float ()
{
	prelude ("123.53189");
	expect_lit (123.53189f);

	end ();
}

void
test_lexer_exponential ()
{
	prelude ("123e-10 123e+10");
	expect_lit (123e-10f);
	expect_lit (123e+10f);

	end ();
}

void
test_lexer_signed ()
{
	prelude ("-150.0 -150e-10 -00000.0 +00000.0");
	expect_lit (-150.0f);
	expect_lit (-150e-10f);
	expect_lit (0.0f);
	expect_lit (0.0f);

	end ();
}

void
test_lexer_eof ()
{
	prelude ("");

	eof ();
	end ()
}

void
test_lexer_spaces ()
{
	prelude ("      \t\n\r");

	eof ();
	end ()
}

void
test_lexer_comments ()
{
	prelude ("# This is a comment!\n 123 \n # This is also a comment!");
	expect_lit (123.0f);
	eof ();

	expect_lineno (3);
	end ();
}
