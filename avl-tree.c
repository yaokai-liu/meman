/**
 * Filename: avl-tree.c
 * Creator: Yaokai Liu
 * Create Date: 2024-12-04
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "avl-tree.h"

typedef struct AVLNode AVLNode;
typedef struct AVLNode {
  uint64_t key;
  uint64_t height;
  void *value;
  AVLNode *left;
  AVLNode *right;
} AVLNode;

typedef struct AVLTree {
  const Allocator *allocator;
  compare_t *fn_cmp;
  AVLNode *root;
} AVLTree;

AVLNode *AVLNode_new(const uint64_t key, AVLTree *tree);

AVLNode *AVLNode_get(AVLNode *root, uint64_t key, const AVLTree *tree);
AVLNode *AVLNode_add(AVLNode **root, const uint64_t key, AVLTree *tree);
void AVLNode_del(AVLNode *root, destruct_t *del_content, AVLTree *tree);

inline AVLTree *AVLTree_new(const Allocator *allocator, compare_t *fn_compare) {
  AVLTree *tree = allocator->calloc(1, sizeof(AVLTree));
  tree->allocator = allocator;
  tree->fn_cmp = fn_compare;
  return tree;
}

inline uint64_t AVLTree_height(const AVLTree *tree) {
  return tree->root ? tree->root->height + 1 : 0;
}

inline void *AVLTree_get(const AVLTree *tree, uint64_t key) {
  if (!tree) { return nullptr; }
  const AVLNode * const node = AVLNode_get(tree->root, key, tree);
  return node ? node->value : nullptr;
}

inline int32_t AVLTree_set(AVLTree *tree, uint64_t key, void *value) {
  if (!tree) { return -1; }
  AVLNode *node = AVLNode_add(&tree->root, key, tree);
  return node ? (node->value = value, 0) : -1;
}

inline void AVLNode_del(AVLNode *root, destruct_t *del_content, AVLTree *tree) {
  if (!root) { return; }
  if (root->left) { AVLNode_del(root->left, del_content, tree); }
  if (root->right) { AVLNode_del(root->right, del_content, tree); }
  if (del_content) { del_content(root->value, tree->allocator); }
  tree->allocator->free(root);
}

inline void AVLTree_destroy(AVLTree *tree, destruct_t *del_value) {
  if (!tree) { return; }
  AVLNode_del(tree->root, del_value, tree);
  tree->allocator->free(tree);
}

inline AVLNode *AVLNode_new(const uint64_t key, AVLTree *tree) {
  AVLNode *node = tree->allocator->calloc(1, sizeof(AVLNode));
  node->key = key;
  return node;
}

inline AVLNode *AVLNode_get(AVLNode *root, uint64_t key, const AVLTree *tree) {
  if (!root) { return nullptr; }
  int32_t cmp =
    tree->fn_cmp ? tree->fn_cmp((void *) key, (void *) root->key) : (int32_t) (key - root->key);
  if (cmp > 0) { return AVLNode_get(root->right, key, tree); }
  if (cmp < 0) { return AVLNode_get(root->left, key, tree); }
  return root;
}

#define max(_a, _b) ((_a) > (_b) ? (_a) : (_b))
#define LL_rotate(_pNode)                                                                         \
  do {                                                                                            \
    AVLNode *temp = *_pNode;                                                                      \
    *_pNode = temp->left;                                                                         \
    temp->left = (*_pNode)->right;                                                                \
    (*_pNode)->right = temp;                                                                      \
    uint64_t tlh = temp->left ? temp->left->height + 1 : 0;                                       \
    uint64_t trh = temp->right ? temp->right->height + 1 : 0;                                     \
    temp->height = max(tlh, trh);                                                                 \
    (*_pNode)->height = max((*_pNode)->left ? (*_pNode)->left->height + 1 : 0, temp->height + 1); \
  } while (0)
#define RR_rotate(_pNode)                                                             \
  do {                                                                                \
    AVLNode *temp = *_pNode;                                                          \
    *_pNode = temp->right;                                                            \
    temp->right = (*_pNode)->left;                                                    \
    (*_pNode)->left = temp;                                                           \
    uint64_t tlh = temp->left ? temp->left->height + 1 : 0;                           \
    uint64_t trh = temp->right ? temp->right->height + 1 : 0;                         \
    temp->height = max(tlh, trh);                                                     \
    (*_pNode)->height =                                                               \
      1 + max((*_pNode)->right ? (*_pNode)->right->height + 1 : 0, temp->height + 1); \
  } while (0)

#define setNode(node)                                                                \
  do {                                                                               \
    int32_t cmp = tree->fn_cmp ? tree->fn_cmp((void *) key, (void *) (*root)->key) : \
                                 (int32_t) (key - (*root)->key);                     \
    if (cmp == 0) { return *root; }                                                  \
    AVLNode **pNode = (cmp < 0) ? &((*root)->left) : &((*root)->right);              \
    node = AVLNode_add(pNode, key, tree);                                            \
  } while (false)

inline AVLNode *AVLNode_add(AVLNode **root, const uint64_t key, AVLTree *tree) {
  if (!*root) {
    *root = AVLNode_new(key, tree);
    return *root;
  }
  AVLNode *node;
  setNode(node);

  uint64_t left_height = (*root)->left ? (*root)->left->height + 1 : 0;
  uint64_t right_height = (*root)->right ? (*root)->right->height + 1 : 0;
  if (left_height >= 2 + right_height) {
    const uint64_t llh = (*root)->left->left ? (*root)->left->left->height : 0;
    const uint64_t lrh = (*root)->left->right ? (*root)->left->right->height : 0;
    if (llh < lrh) { RR_rotate(&(*root)->left); }
    LL_rotate(root);
  } else if (right_height >= 2 + left_height) {
    const uint64_t rlh = (*root)->right->left ? (*root)->right->left->height : 0;
    const uint64_t rrh = (*root)->right->right ? (*root)->right->right->height : 0;
    if (rlh > rrh) { LL_rotate(&(*root)->right); }
    RR_rotate(root);
  }
  left_height = (*root)->left ? (*root)->left->height + 1 : 0;
  right_height = (*root)->right ? (*root)->right->height + 1 : 0;
  (*root)->height = max(left_height, right_height);

  return node;
}
