/**
 * Filename: avl-tree.h
 * Creator: Yaokai Liu
 * Create Date: 2024-12-04
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef LIU_AVL_TREE_H
#define LIU_AVL_TREE_H

#include "allocator.h"
#include <stdint.h>

typedef struct AVLTree AVLTree;

AVLTree *AVLTree_new(const Allocator *allocator);
void AVLTree_destroy(AVLTree *tree, destruct_t *del_value);

uint64_t AVLTree_height(const AVLTree *tree);
void *AVLTree_get(const AVLTree *tree, uint64_t key);
int32_t AVLTree_set(AVLTree *tree, uint64_t key, void *value);

#endif  // LIU_AVL_TREE_H
