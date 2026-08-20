#ifndef TXTURE2_TXR_MACRO_H
#define TXTURE2_TXR_MACRO_H

#include "txr/token.h"

#include <string.h>

#define EXTENDED_TOKEN_BUFFER_SIZE 32

typedef struct {
	char	  buffer[EXTENDED_TOKEN_BUFFER_SIZE]; // optional
	double	  token_value;						  // optional
	TokenType type;
} ExtendedToken;

#define _extended_tt(value, buf, kind)                                    \
	(ExtendedToken)                                                       \
	{ .token_value = value, .buffer = strcpy (buf), .type = kind }

#define _just_tt(kind)                                                    \
	(ExtendedToken)                                                       \
	{ .type = kind }

#define _value_tt(v)                                                      \
	(ExtendedToken)                                                       \
	{ .type = TTNumber, .token_value = v }

#define _id_tt(copy_from) id_tt (copy_from)

static inline ExtendedToken
id_tt (const char* str)
{
	ExtendedToken tok;
	tok.type = TTId;
	strcpy (tok.buffer, str);

	return tok;
}

typedef enum {
	MacroFunction,
	MacroAlias,
} MacroType;

typedef struct {
	char**		   args;
	ExtendedToken* code;
	MacroType	   type;
} Macro;

static inline Macro
create_macro (ExtendedToken* tokens, char** args, MacroType typ)
{ return (Macro) {.args = args, .code = tokens, .type = typ}; }

#endif // !TXTURE2_TXR_MACRO_H
