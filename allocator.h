/**
 * Filename: allocator.h
 * Creator: Yaokai Liu
 * Create Date: 2024-7-6
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef XIDE_ALLOCATOR_H
#define XIDE_ALLOCATOR_H

#include <stddef.h>

typedef struct {
  void *(* const malloc)(size_t size);

  void *(* const realloc)(void *ptr, size_t size);

  void *(* const calloc)(size_t count, size_t size);

  void (* const free)(void *ptr);

  void *(* const memcpy)(void * restrict dest, const void * restrict src, size_t size);

  void *(* const memset)(void *dest, int value, size_t size);
} Allocator;

extern const Allocator STDAllocator;

#endif  // XIDE_ALLOCATOR_H
