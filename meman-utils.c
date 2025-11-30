/*
 * Filename: meman-utils.c
 * Creator: Yaokai Liu
 * Create Date: 2025-10-21
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "meman-utils.h"

uint64_t char2u64(const char *key) {
  return *key;
}

uint64_t refer2u64(const REFER(void) *key) {
  return (uint64_t) *key;
}
