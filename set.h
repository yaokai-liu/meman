/**
 * Filename: set.h
 * Creator: Yaokai Liu
 * Create Date: 2024-12-04
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef LIU_SET_H
#define LIU_SET_H

#include <stdint.h>
#include "allocator.h"
#include "key_t.h"

typedef struct Set Set;
extern const uint32_t sizeof_set;

Set *Set_new(uint32_t ele_size, uint32_t set_id, key_t *fn_key,
             destruct_t *fn_release, const Allocator *allocator);

uint32_t Set_add(const Set *set, const void * v_ele);

// Notice: after remove elements, must tidy the set before access its data
uint32_t Set_remove(const Set *set, const void * v_ele);

// dest = dest | set
// @return: count of adds
uint32_t Set_update(const Set *dest, const Set *set);
// set = set - red
// @return: count of removes
uint32_t Set_reduce(Set *set, const Set *red);
// set = set & lim
// @return: count of removes
uint32_t Set_limit(Set *set, const Set *lim);

void * Set_data(const Set *set);
uint32_t Set_count(const Set *set);
bool Set_has(const Set *set, const void * ele);

void Set_tidy(Set *set);
void Set_reset(const Set *set);
void Set_destroy(Set *set);

#endif  // LIU_SET_H
