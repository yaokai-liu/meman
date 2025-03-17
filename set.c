/**
 * Filename: set.h
 * Creator: Yaokai Liu
 * Create Date: 2024-12-04
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "set.h"
#include "array.h"
#include "avl-tree.h"

typedef struct Set {
  const Allocator *allocator;
  Array *array;
  AVLTree *key_tree;
  unikey_t *fn_key;
} Set;

inline Set *Set_new(uint32_t set_id, const Allocator *allocator, unikey_t *fn_key) {
  Set *set = allocator->calloc(1, sizeof(Set));
  set->allocator = allocator;
  set->array = Array_new(sizeof(REFER(void)), set_id, allocator);
  set->key_tree = AVLTree_new(allocator, nullptr);
  set->fn_key = fn_key;
  return set;
}

inline uint32_t Set_add(Set *set, const REFER(void) v_ele) {
  uint64_t key = set->fn_key ? set->fn_key(v_ele) : (uint64_t) v_ele;
  if (!AVLTree_get(set->key_tree, key)) {
    uint32_t index = Array_length(set->array);
    Array_append(set->array, &v_ele, 1);
    AVLTree_set(set->key_tree, key, (void *) (uint64_t) index);
    return 1;
  }
  return 0;
}

inline uint32_t Set_has(Set *set, const REFER(void) v_ele) {
  uint64_t key = set->fn_key ? set->fn_key(v_ele) : (uint64_t) v_ele;
  return (uint32_t) (uint64_t) AVLTree_get(set->key_tree, key);
}

inline uint32_t Set_update(Set *dest, const Set *set) {
  uint32_t updated = 0;
  const REFER(void) *refers = Array_first_real(set->array);
  const uint32_t count = Array_length(set->array);
  for (uint32_t i = 0; i < count; i++) { updated += Set_add(dest, refers[i]); }
  return updated;
}

inline void Set_destroy(Set *set) {
  releasePrimeArray(set->array);
  AVLTree_destroy(set->key_tree, nullptr);
  set->allocator->free(set);
}

inline uint32_t Set_count(Set *set) {
  return Array_length(set->array);
}

inline REFER(void) Set_data(Set *set) {
  return Array_first_real(set->array);
}
