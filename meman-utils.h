/*
 * Filename: meman-utils.h
 * Creator: Yaokai Liu
 * Create Date: 2025-10-21
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_UTILS_H
#define MACHINE_UTILS_H

#include <stdint.h>
#include "allocator.h"
#include "key_t.h"

uint64_t char2u64(const char *key);
uint64_t refer2u64(const REFER(void) *key);

#endif  // MACHINE_UTILS_H
