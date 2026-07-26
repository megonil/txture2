#ifndef TXTURE2_TXR_OPCODE_H
#define TXTURE2_TXR_OPCODE_H

#include <stdint.h>

enum OpCode : uint8_t {
	Expand,
	Const,
	Add,
	Sub,
	Mul,
	Div,
	Pow,
};

#endif // !TXTURE2_TXR_OPCODE_H
