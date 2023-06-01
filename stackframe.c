//
// Created by David Ensikat on 1/6/2023.
//

#include "compiler.h"
#include <assert.h>

#define STACK_PUSH_SIZE 4

void stackframe_sub(struct node *func_node, int type, const char *name, size_t amount) {
  assert((amount % STACK_PUSH_SIZE) == 0);
}