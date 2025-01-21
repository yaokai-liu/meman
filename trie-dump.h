/**
 * Project Name: machine
 * Module Name: meman
 * Filename: trie-dump.h
 * Creator: Yaokai Liu
 * Create Date: 2025-01-21
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_TRIE_DUMP_H
#define MACHINE_TRIE_DUMP_H

#include "array.h"
#include "trie.h"

struct TrieKeyItem {
  uint64_t key;
  uint64_t next_node;
};

struct TrieNodeItem {
  uint32_t offset;
  uint32_t count;
  void *value;
};

void Trie_dump(
  Trie *trie, Array /*<TrieKeyMappingItem>*/ *key_array, Array /*<TrieNodeItem>*/ *node_array
);

#endif  // MACHINE_TRIE_DUMP_H
