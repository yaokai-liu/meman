/**
 * Filename: dict.h
 * Creator: Yaokai Liu
 * Create Date: 2025-05-16
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#ifndef LIU_DICT_H
#define LIU_DICT_H


#include <stdint.h>
#include "allocator.h"
#include "key_t.h"

typedef struct Dict Dict;

Dict * Dict_new(uint32_t key_size, uint32_t ele_size, key_t *fn_key, uint32_t dict_id,
                destruct_t *fn_rel_key, destruct_t *fn_rel_ele, const Allocator *allocator);

void Dict_set(const Dict *dict, const void *key, const void *ele);

// Notice: after remove elements, must tidy the dict before access its data
uint32_t Dict_remove(Dict *dict, const void *keys[], uint32_t count);

// dest = dest | dict
// @return: count of adds
uint32_t Dict_update(Dict *dest, const Dict *dict, bool cover_update);
// dict = dict - red
// @return: count of removes
uint32_t Dict_reduce(Dict *dict, const Dict *red);
// dict = dict & lim
// @return: count of removes
uint32_t Dict_limit(Dict *dict, const Dict *lim);

uint32_t Dict_count(Dict *dict);
void *Dict_keys(Dict *dict);
void *Dict_elements(Dict *dict);
void * Dict_get(const Dict *dict, const void *key);

void Dict_tidy(Dict *dict);
void Dict_reset(Dict *dict);
void Dict_destroy(Dict *dict);

#endif //LIU_DICT_H
