/**
 * Project Name: machine
 * Module Name: meman
 * Filename: traversal.h
 * Creator: Yaokai Liu
 * Create Date: 2025-01-21
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_TRAVERSAL_H
#define MACHINE_TRAVERSAL_H

#include "array.h"
#include "avl-tree.h"

typedef struct AVLPair {
  uint64_t key;
  const void *value;
} AVLPair;

typedef void traverse_t(const AVLPair *, ...);

Array /*<AVLPair>*/ *
  AVLTree_inorder_traversal(AVLTree *tree, uint32_t id, const Allocator *allocator);

#endif  // MACHINE_TRAVERSAL_H
