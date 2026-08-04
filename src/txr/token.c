#include "txr/token.h"

const char*
tok_to_string (TokenType t)
{
	static char s[32];

	if (t < TTStart && isprint (t)) {
		snprintf (s, 32, "%c", t);
	} else if (t < TTStart) {
		snprintf (s, 32, "\\%d", t);
	} else {
		assert (t < TTEndTokens && t > TTStart);
		return toks_strs[t - TTError];
	}

	return s;
}
