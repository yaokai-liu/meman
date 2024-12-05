/**
 * Filename: trie.c
 * Creator: Yaokai Liu
 * Create Date: 2024-12-04
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "trie.h"
#include "avl-tree.h"

typedef struct TrieNode TrieNode;

typedef struct TrieNode {
  void *value;
  AVLTree *children;
} TrieNode;

typedef struct Trie {
  const Allocator *allocator;
  uint64_t count;
  TrieNode *root;
} Trie;

void delTrieNode(TrieNode *trie_node, const Allocator *allocator);

Trie *Trie_new(const Allocator *allocator) {
  TrieNode *node = allocator->calloc(1, sizeof(TrieNode));
  Trie *tree = allocator->calloc(1, sizeof(Trie));
  tree->allocator = allocator;
  node->children = AVLTree_new(allocator);
  tree->root = node;
  return tree;
}

uint64_t Trie_count(const Trie *tree) {
  return tree->count;
}
void *Trie_get(const Trie *tree, const char *key) {
  const TrieNode *trie_node = tree->root;
  for (int i = 0; key[i]; i++) {
    if (!trie_node->children) { return nullptr; }
    trie_node = AVLTree_get(trie_node->children, key[i]);
    if (!trie_node) { return nullptr; }
  }
  return trie_node->value;
}

void Trie_set(Trie *tree, const char *key, void *value) {
  TrieNode *trie_node = tree->root;
  for (int i = 0; key[i]; i++) {
    auto node = (TrieNode *) AVLTree_get(trie_node->children, key[i]);
    if (!node) {
      node = tree->allocator->calloc(1, sizeof(TrieNode));
      node->children = AVLTree_new(tree->allocator);
      AVLTree_set(trie_node->children, key[i], node);
    }
    trie_node = node;
  }
  if (!trie_node->value) { tree->count++; }
  trie_node->value = value;
}

void Trie_del(Trie *tree, const char *key, destruct_t *del_content) {
  TrieNode *trie_node = tree->root;
  if (!trie_node->children) { return; }
  for (int i = 0; key[i]; i++) {
    TrieNode * node = AVLTree_get(trie_node->children, key[i]);
    if (!node) { return; }
    trie_node = node;
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

