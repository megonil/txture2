#ifndef TXTURE2_TXR_MACRO_H
#define TXTURE2_TXR_MACRO_H

#include "txr/token.h"

#define EXTENDED_TOKEN_BUFFER_SIZE 32

typedef struct {
	char	  buffer[EXTENDED_TOKEN_BUFFER_SIZE]; // optional
	double	  token_value;						  // optional
	TokenType type;
} ExtendedToken;

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
