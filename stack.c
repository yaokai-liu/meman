/**
 * Filename: stack.c
 * Creator: Yaokai Liu
 * Create Date: 2024-10-27
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "stack.h"
#include "allocator.h"
#include <string.h>

struct Stack {
  uint32_t allocated;
  uint32_t used;
  void *stack;
  const Allocator *allocator;
};

#define ALLOC_LEN (32 * sizeof(void *))
#define min(a, b) ((a) < (b) ? (a) : (b))

inline Stack *Stack_new(const Allocator *allocator) {
  Stack *stack = allocator->malloc(sizeof(Stack));
  stack->allocator = allocator;
  stack->stack = allocator->malloc(ALLOC_LEN);
  stack->allocated = ALLOC_LEN;
  stack->used = 0;
  return stack;
}

inline void Stack_clear(Stack *stack) {
  stack->allocator->free(stack->stack);
  stack->allocated = 0;
  stack->used = 0;
  stack->stack = nullptr;
}

inline uint32_t Stack_push(Stack *stack, const void *data, uint32_t size) {
  if (!data || !size) { return 0; }
  if (stack->used + size >= stack->allocated) {
    uint32_t length = ((stack->used + size) / ALLOC_LEN + 1) * ALLOC_LEN;
    void *p = stack->allocator->realloc(stack->stack, length);
    if (!p) { return -1; }
    stack->stack = p;
    stack->allocated = length;
  }
  memcpy(stack->stack + stack->used, data, size);
  stack->used += size;
  return size;
}

inline uint32_t Stack_pop(Stack *stack, void *dest, uint32_t size) {
  size = min(stack->used, size);
  stack->used -= size;
  if (dest) { memcpy(dest, stack->stack + stack->used, size); }
  return size;
}

inline uint32_t Stack_top(Stack *stack, void *dest, uint32_t size) {
  size = min(stack->used, size);
  if (dest) { memcpy(dest, stack->stack + stack->used - size, size); }
  return size;
}

inline bool Stack_empty(Stack *stack) {
  return !(stack->used > 0);
}
