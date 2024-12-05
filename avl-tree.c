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
  AVLNode *root;
} AVLTree;

AVLNode *newAVLNode(const uint64_t key, const Allocator *allocator);

AVLNode *getAVLNode(AVLNode *root, uint64_t key);
AVLNode *addAVLNode(AVLNode **root, const uint64_t key, const Allocator *allocator);
void delAVLNode(AVLNode *root, destruct_t *del_content, const Allocator * const ptr);

inline AVLTree *AVLTree_new(const Allocator *allocator) {
  AVLTree *tree = allocator->calloc(1, sizeof(AVLTree));
  tree->allocator = allocator;
  return tree;
}

inline uint64_t AVLTree_height(const AVLTree *tree) {
  return tree->root ? tree->root->height + 1 : 0;
}

inline void *AVLTree_get(const AVLTree *tree, uint64_t key) {
  if (!tree) { return nullptr; }
  const AVLNode * const node = getAVLNode(tree->root, key);
  return node ? node->value : nullptr;
}

inline int32_t AVLTree_set(AVLTree *tree, uint64_t key, void *value) {
  if (!tree) { return -1; }
  AVLNode *node = addAVLNode(&tree->root, key, tree->allocator);
  return node ? (node->value = value, 0) : -1;
}

inline void delAVLNode(AVLNode *root, destruct_t *del_content, const Allocator * const allocator) {
  if (!root) { return; }
  if (root->left) { delAVLNode(root->left, del_content, allocator); }
  if (root->right) { delAVLNode(root->right, del_content, allocator); }
  if (del_content) { del_content(root->value, allocator); }
  allocator->free(root);
}
inline void AVLTree_destroy(AVLTree *tree, void (*del_value)(void *, const Allocator *allocator)) {
  if (!tree) { return; }
  delAVLNode(tree->root, del_value, tree->allocator);
  tree->allocator->free(tree);
}

inline AVLNode *newAVLNode(const uint64_t key, const Allocator *allocator) {
  AVLNode *node = allocator->calloc(1, sizeof(AVLNode));
  node->key = key;
  return node;
}

inline AVLNode *getAVLNode(AVLNode *root, const uint64_t key) {
  if (!root) { return nullptr; }
  if (key == root->key) { return root; }
  if (key < root->key) { return getAVLNode(root->left, key); }
  return getAVLNode(root->right, key);
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

inline AVLNode *addAVLNode(AVLNode **root, const uint64_t key, const Allocator *allocator) {
  if (!*root) {
    *root = newAVLNode(key, allocator);
    return *root;
  }
  if (key == (*root)->key) { return *root; }
  AVLNode **pNode = (key < (*root)->key) ? &((*root)->left) : &((*root)->right);
  AVLNode *node = addAVLNode(pNode, key, allocator);

  uint64_t left_height = (*root)->left ? (*root)->left->height + 1 : 0;
  uint64_t right_height = (*root)->right ? (*root)->right->height + 1 : 0;
  if (left_height >= 2 + right_height) {
    const uint64_t llh = (*root)->left->left ? (*root)->left->left->height : 0;
    const uint64_t lrh = (*root)->left->right ? (*root)->left->right->height : 0;
    if (llh < lrh) {
      RR_rotate(&(*root)->left);
    }
    LL_rotate(root);
  } else if (right_height >= 2 + left_height) {
    const uint64_t rlh = (*root)->right->left ? (*root)->right->left->height : 0;
    const uint64_t rrh = (*root)->right->right ? (*root)->right->right->height : 0;
    if (rlh > rrh) {
      LL_rotate(&(*root)->right);
    }
    RR_rotate(root);
  }
  left_height = (*root)->left ? (*root)->left->height + 1 : 0;
  right_height = (*root)->right ? (*root)->right->height + 1 : 0;
  (*root)->height = max(left_height, right_height);

  return node;
}
