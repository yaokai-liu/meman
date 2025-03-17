/**
 * Filename: set.h
 * Creator: Yaokai Liu
 * Create Date: 2024-12-04
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef LIU_SET_H
#define LIU_SET_H

#include "allocator.h"
#include <stdint.h>
typedef uint64_t unikey_t(const void *);

typedef struct Set Set;

Set *Set_new(uint32_t set_id, const Allocator *allocator, unikey_t *fn_key);
uint32_t Set_add(Set *set, const REFER(void) v_ele);
uint32_t Set_has(Set *set, const REFER(void) v_ele);
uint32_t Set_update(Set *dest, const Set *set);
REFER(void) Set_data(Set *set);
uint32_t Set_count(Set *set);
void Set_destroy(Set *set);

#endif  // LIU_SET_H
