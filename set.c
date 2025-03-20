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
  Array *keys;
  Array *elements;
  AVLTree *key_tree;
  unikey_t *fn_key;
  uint32_t set_id;
} Set;

inline Set *Set_new(uint32_t set_id, const Allocator *allocator, unikey_t *fn_key) {
  Set *set = allocator->calloc(1, sizeof(Set));
  set->allocator = allocator;
  set->elements = Array_new(sizeof(REFER(void)), set_id, allocator);
  set->keys = Array_new(sizeof(uint64_t), set_id, allocator);
  set->key_tree = AVLTree_new(allocator, nullptr);
  set->fn_key = fn_key;
  set->set_id = set_id;
  return set;
}

inline uint32_t Set_add(Set *set, const REFER(void) v_ele) {
  uint64_t key = set->fn_key ? set->fn_key(v_ele) : (uint64_t) v_ele;
  if (!AVLTree_get(set->key_tree, key)) {
    Array_append(set->keys, &key, 1);
    Array_append(set->elements, &v_ele, 1);
    REFER(REFER(void)) v_offset = Array_last_virt(set->elements);
    AVLTree_set(set->key_tree, key, v_offset);
    return 1;
  }
  return 0;
}

inline uint32_t Set_remove(Set *set, const REFER(void) v_ele) {
  uint64_t key = set->fn_key ? set->fn_key(v_ele) : (uint64_t) v_ele;
  REFER(REFER(void)) v_offset = AVLTree_get(set->key_tree, key);
  if (!v_offset) { return 0; }
  AVLTree_set(set->key_tree, key, nullptr);
  return 1;
}

inline bool Set_has(const Set *set, const REFER(void) v_ele) {
  uint64_t key = set->fn_key ? set->fn_key(v_ele) : (uint64_t) v_ele;
  return AVLTree_get(set->key_tree, key) ? 1 : 0;
}

inline uint32_t Set_update(Set *dest, const Set *set) {
  uint32_t updated = 0;
  const REFER(void) *refers = Array_first_real(set->elements);
  const uint32_t count = Array_length(set->elements);
  for (uint32_t i = 0; i < count; i++) { updated += Set_add(dest, refers[i]); }
  return updated;
}

inline uint32_t Set_reduce(Set *set, const Set *red) {
  uint32_t removed = 0;
  const REFER(void) *refers = Array_first_real(red->elements);
  const uint32_t count = Array_length(red->elements);
  for (uint32_t i = 0; i < count; i++) { removed += Set_remove(set, refers[i]); }
  Set_tidy(set);
  return removed;
}

inline uint32_t Set_limit(Set *set, const Set *lim) {
  uint32_t removed = 0;
  const REFER(void) *refers = Array_first_real(set->elements);
  const uint32_t count = Array_length(set->elements);
  for (uint32_t i = 0; i < count; i++) {
    if (!Set_has(lim, refers[i])) { removed += Set_remove(set, refers[i]); }
  }
  Set_tidy(set);
  return removed;
}

inline void Set_tidy(Set *set) {
  const uint64_t *keys = Array_first_real(set->keys);
  const uint32_t count = Array_length(set->keys);

  AVLTree *new_key_tree = AVLTree_new(set->allocator, nullptr);
  Array *new_keys = Array_new(sizeof(uint64_t), set->set_id, set->allocator);
  Array *new_elements = Array_new(sizeof(REFER(void)), set->set_id, set->allocator);
  for (uint32_t i = 0; i < count; i++) {
    REFER(REFER(void)) v_offset = AVLTree_get(set->key_tree, keys[i]);
    if (v_offset) {
      uint32_t *key = Array_virt2real(set->keys, v_offset);
      REFER(void) *element = Array_virt2real(set->elements, v_offset);
      Array_append(new_keys, key, 1);
      Array_append(new_elements, element, 1);
      AVLTree_set(new_key_tree, keys[i], v_offset);
    }
  }
  Set_reset(set);
  set->keys = new_keys;
  set->elements = new_elements;
  set->key_tree = new_key_tree;
}

inline void Set_reset(Set *set) {
  releasePrimeArray(set->keys);
  releasePrimeArray(set->elements);
  AVLTree_destroy(set->key_tree, nullptr);
}

inline void Set_destroy(Set *set) {
  Set_reset(set);
  set->allocator->free(set);
}

inline uint32_t Set_count(Set *set) {
  return Array_length(set->elements);
}

inline REFER(void) Set_data(Set *set) {
  return Array_first_real(set->elements);
}
