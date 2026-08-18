#ifndef TXTURE2_TXR_VM_H
#define TXTURE2_TXR_VM_H

#include "color.h"
#include "globals.h"
#include "image.h"
#include "table.h"
#include "txr/chunk.h"
#include "txr/value.h"

#define VMERRORS                                                          \
	V (UnknownVariable, "unknown variable `%s`")                          \
	V (UnknownBuiltinFunction, "unknown builtin function `%s`")           \
	V (WrongArgumentQuantity,                                             \
	   "wrong argument quantity to function `%s`: expected %zu, found "   \
	   "%zu")

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
	Color		color;
	tvalue		last;
} VMResult;

typedef struct {
	VariableTable		  variables;
	BuiltinFunctionsTable functions;
	tvalue*				  stack;
} VM;

/// Init virtual machine
VM*
make_vm ();

/// Execute and return some result
VMResult
execute (
	VM*				  vm,
	const Chunk*	  chunk,
	tvalue			  x,
	tvalue			  y,
	const ImageProps* props,
	uint8_t			  do_pop);

/// Print runtime error(if there's one)
void vm_print (VMResult);

void
vm_free (VM* vm);

#endif // !TXTURE2_TXR_VM_H
