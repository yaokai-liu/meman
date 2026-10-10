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
  const Allocator *allocator;
  uint32_t allocated;
  uint32_t used;
  void *stack;
};

#ifndef ALLOC_LEN
#define ALLOC_LEN 32
#endif
#ifndef STACK_ALLOC_SCALE
#define STACK_ALLOC_SCALE 1
#endif
#define STACK_ALLOC_SIZE (ALLOC_LEN * STACK_ALLOC_SCALE * sizeof(void *))
#define min(a, b) ((a) < (b) ? (a) : (b))

inline Stack *Stack_new(const Allocator *allocator) {
  Stack *stack = allocator->malloc(sizeof(Stack));
  stack->allocator = allocator;
  stack->stack = allocator->malloc(STACK_ALLOC_SIZE);
  stack->allocated = STACK_ALLOC_SIZE;
  stack->used = 0;
  return stack;
}

inline uint32_t Stack_size(const Stack *stack) {
  return stack->used;
}

inline void *Stack_get(const Stack *stack, uint32_t offset) {
  if (offset >= stack->used) { return nullptr; }
  return stack->stack + offset;
}

inline void Stack_clear(Stack *stack) {
  stack->allocator->free(stack->stack);
  stack->allocated = 0;
  stack->used = 0;
  stack->stack = nullptr;
}

inline uint32_t Stack_push(Stack *stack, const void *data, const uint32_t size) {
  if (!data || !size) { return 0; }
  if (stack->used + size >= stack->allocated) {
    const uint32_t length = ((stack->used + size) / STACK_ALLOC_SIZE + 1) * STACK_ALLOC_SIZE;
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
  if (stack->used < size) { return 0; }
  size = min(stack->used, size);
  stack->used -= size;
  if (dest) { memcpy(dest, stack->stack + stack->used, size); }
  return size;
}

inline uint32_t Stack_top(const Stack *stack, void *dest, uint32_t size) {
  size = min(stack->used, size);
  if (dest) { memcpy(dest, stack->stack + stack->used - size, size); }
  return size;
}

inline bool Stack_empty(const Stack *stack) {
  return stack->used <= 0;
}
