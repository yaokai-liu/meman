/**
 * Filename: stack.h
 * Creator: Yaokai Liu
 * Create Date: 2024-10-27
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef LIU_STACK_H
#define LIU_STACK_H

#include "allocator.h"
#include <stdint.h>

typedef struct Stack Stack;

Stack *Stack_new(const Allocator *allocator);
uint32_t Stack_size(Stack *stack);
void *Stack_get(Stack *stack, uint32_t offset);
void Stack_clear(Stack *stack);
uint32_t Stack_push(Stack *stack, const void *data, uint32_t size);
uint32_t Stack_pop(Stack *stack, void *dest, uint32_t size);
uint32_t Stack_top(Stack *stack, void *dest, uint32_t size);
bool Stack_empty(Stack *stack);
#endif  // LIU_STACK_H
