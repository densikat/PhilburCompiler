//
// Created by David Ensikat on 20/5/2023.
//

#include "compiler.h
#include "helpers/vector.h"
#include <assert.h>
#include <memory.h>
#include <stdlib.h>

struct scope {
  int flags;

  // void*
  struct vector *entities;

  // Total number of bytes in scope. Aligned to 16 bytes
  size_t size;

  // NULL if no parent
  struct scope *parent;
};

struct scope *scope_alloc() {
  struct scope *scope = calloc(1, sizeof(struct scope));
  scope->entities = vector_create(sizeof(void *));
  vector_set_peek_pointer_end(scope->entities);
  vector_set_flag(scope->entities, VECTOR_FLAG_PEEK_DECREMENT);
}