#ifndef TXTURE2_TXR_VM_H
#define TXTURE2_TXR_VM_H

#include "txr/chunk.h"
#include "txr/value.h"

typedef struct {
	tvalue* stack;
} VM;

VM*
make_vm ();

void
execute (VM* vm, Chunk* chunk);

void
vm_free (VM* vm);

#endif // !TXTURE2_TXR_VM_H
