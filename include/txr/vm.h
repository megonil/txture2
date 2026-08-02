#ifndef TXTURE2_TXR_VM_H
#define TXTURE2_TXR_VM_H

#include "table.h"
#include "txr/chunk.h"
#include "txr/value.h"

#define VMERRORS V (UnknownVariable, "unknown variable %s")

#define V(Variant, Str) Variant,
typedef enum { VMOK = -1, VMERRORS } VMErrorKind;
#undef V

typedef struct {
	size_t		line;
	VMErrorKind kind;
} VMError;

typedef struct {
	VMError		code;
	const char* args[4];
	tvalue		last;
} VMResult;

typedef struct {
	VariableTable variables;
	tvalue*		  stack;
} VM;

/// Init virtual machine
VM*
make_vm ();

/// Execute and return some result
VMResult
execute (VM* vm, const Chunk* chunk);

/// Print the result(if there's an error)
void vm_print (VMResult);

void
vm_free (VM* vm);

#endif // !TXTURE2_TXR_VM_H
