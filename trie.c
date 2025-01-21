/**
 * Filename: trie.c
 * Creator: Yaokai Liu
 * Create Date: 2024-12-04
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "trie.h"
#include "array.h"
#include "avl-tree.h"
#include "traversal.h"
#include "trie-dump.h"

typedef struct TrieNode TrieNode;

typedef struct TrieNode {
  void *value;
  AVLTree *children;
} TrieNode;

typedef struct Trie {
  const Allocator *allocator;
  uint32_t key_size;
  uint64_t (*fn_key)(const void *);
  uint64_t count;
  TrieNode *root;
} Trie;

void delTrieNode(TrieNode *trie_node, const Allocator *allocator);

Trie *Trie_new(uint32_t key_size, uint64_t (*fn_key)(const void *), const Allocator *allocator) {
  if (!key_size || !fn_key) { return nullptr; }
  TrieNode *node = allocator->calloc(1, sizeof(TrieNode));
  Trie *tree = allocator->calloc(1, sizeof(Trie));
  tree->allocator = allocator;
  tree->key_size = key_size;
  tree->fn_key = fn_key;
  node->children = AVLTree_new(allocator, nullptr);
  tree->root = node;
  return tree;
}

uint64_t Trie_count(const Trie *tree) {
  return tree->count;
}

#define foreach_v_key()                                \
  for (uint64_t v_key = tree->fn_key(key); v_key != 0; \
       (key += tree->key_size), (v_key = tree->fn_key(key)))

void *Trie_get(const Trie *tree, const void *key) {
  if (!tree) { return nullptr; }
  const TrieNode *trie_node = tree->root;
  foreach_v_key() {
    if (!trie_node->children) { return nullptr; }
    trie_node = AVLTree_get(trie_node->children, v_key);
    if (!trie_node) { return nullptr; };
  }
  return trie_node->value;
}

void Trie_set(Trie *tree, const void *key, void *value) {
  if (!tree) { return; }
  TrieNode *trie_node = tree->root;
  foreach_v_key() {
    auto node = (TrieNode *) AVLTree_get(trie_node->children, v_key);
    if (!node) {
      node = tree->allocator->calloc(1, sizeof(TrieNode));
      node->children = AVLTree_new(tree->allocator, nullptr);
      AVLTree_set(trie_node->children, v_key, node);
    }
    trie_node = node;
  }
  if (!trie_node->value) { tree->count++; }
  trie_node->value = value;
}
void TrieNode_dump(
  const TrieNode *node, Array /*<TrieKeyMappingItem>*/ *key_array,
  Array /*<TrieNodeItem>*/ *node_array, const Allocator *allocator
);
void TrieNode_dump(
  const TrieNode *node, Array /*<TrieKeyMappingItem>*/ *key_array,
  Array /*<TrieNodeItem>*/ *node_array, const Allocator *allocator
) {
  // TODO: The current implementation may cause out-of-memory problem, please solve it.
  Array *child_array = AVLTree_inorder_traversal(node->children, -1, allocator);
  const uint32_t count = Array_length(child_array);
  if (count == 0) {
    struct TrieNodeItem node_item = {.offset = 0, .count = 0, .value = node->value};
    Array_append(node_array, &node_item, 1);
    releasePrimeArray(child_array);
    return;
  }
  Array *temp_key_array = Array_new(sizeof(struct TrieNodeItem), -1, allocator);
  const AVLPair * const children = Array_real_addr(child_array, 0);
  for (uint32_t i = 0; i < count; i++) {
    const TrieNode *child = children[i].value;
    TrieNode_dump(child, key_array, node_array, allocator);
    const uint64_t key = children[i].key;
    const uint32_t jump_node_offset = Array_length(node_array) - 1;
    struct TrieKeyItem key_item = {.key = key, .next_node = jump_node_offset};
    Array_append(temp_key_array, &key_item, 1);
  }
  const uint32_t jump_key_offset = Array_length(key_array);
  const struct TrieKeyItem *items = Array_real_addr(temp_key_array, 0);
  Array_append(key_array, items, count);
  struct TrieNodeItem node_item = {.offset = jump_key_offset, .count = count, .value = node->value};
  Array_append(node_array, &node_item, 1);
  releasePrimeArray(temp_key_array);
  releasePrimeArray(child_array);
}

void Trie_dump(
  Trie *trie, Array /*<TrieKeyMappingItem>*/ *key_array, Array /*<TrieNodeItem>*/ *node_array
) {
  if (!trie || !key_array || !node_array) { return; }
  TrieNode_dump(trie->root, key_array, node_array, trie->allocator);
}

void Trie_del(Trie *tree, const void *key, destruct_t *del_content) {
  TrieNode *trie_node = tree->root;
  if (!trie_node->children) { return; }
  uint64_t v_key = tree->fn_key(key);
  for (; v_key != 0; key += tree->key_size) {
    TrieNode *node = AVLTree_get(trie_node->children, v_key);
    if (!node) { return; }
    trie_node = node;
    v_key = tree->fn_key(key);
  }
  if (!trie_node->value) { return; }
  if (del_content) { del_content(trie_node->value, tree->allocator); }
  trie_node->value = nullptr;
  tree->count--;
}

void delTrieNode(TrieNode *trie_node, const Allocator *allocator) {
  if (!trie_node) { return; }
  AVLTree_destroy(trie_node->children, (destruct_t *) delTrieNode);
  allocator->free(trie_node);
}

void Trie_destroy(Trie *tree) {
  if (!tree) { return; }
  delTrieNode(tree->root, tree->allocator);
  tree->allocator->free(tree);
}

