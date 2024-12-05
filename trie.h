/**
 * Filename: trie.h
 * Creator: Yaokai Liu
 * Create Date: 2024-12-04
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef LIU_TRIE_H
#define LIU_TRIE_H

#include "allocator.h"
#include <stdint.h>

typedef struct Trie Trie;

Trie *Trie_new(const Allocator *allocator);
void Trie_destroy(Trie *tree);

uint64_t Trie_count(const Trie *tree);
void *Trie_get(const Trie *tree, const char *key);
void Trie_set(Trie *tree, const char *key, void *value);
void Trie_del(Trie *tree, const char *key, destruct_t *del_content);

#endif  // LIU_TRIE_H
