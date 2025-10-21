/**
 * Filename: dict.c
 * Creator: Yaokai Liu
 * Create Date: 2025-05-16
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "dict.h"
#include "array.h"
#include "avl-tree.h"
#include "set.h"


typedef struct Dict {
  const Allocator *allocator;
  Array *keys;
  Array *eles;
  AVLTree *map_tree;
  key_t *  fn_key;
  uint32_t dict_id;
  uint32_t key_size;
  uint32_t ele_size;
  bool     tidied;
  destruct_t *fn_rel_key;
  destruct_t *fn_rel_ele;
} Dict;

inline Dict *
Dict_new(uint32_t key_size, uint32_t ele_size, key_t *fn_key, uint32_t dict_id,
         destruct_t *fn_rel_key, destruct_t *fn_rel_ele, const Allocator *allocator) {
  if (!ele_size) { return nullptr; }
  Dict *dict = allocator->calloc(1, sizeof(Dict));
  dict->allocator = allocator;
  dict->eles = Array_new(ele_size, dict_id, allocator);
  dict->keys = Array_new(key_size, dict_id, allocator);
  dict->map_tree = AVLTree_new(allocator, nullptr);
  dict->fn_key = fn_key;
  dict->dict_id = dict_id;
  dict->ele_size = ele_size;
  dict->fn_rel_key = fn_rel_key;
  dict->fn_rel_ele = fn_rel_ele;
  dict->tidied = true;
  return dict;
}

inline void Dict_set(Dict *dict, const void *key, const void *ele) {
  uint64_t i_key = dict->fn_key ? dict->fn_key(key) : (uint64_t) key;
  REFER(void) v_element = AVLTree_get(dict->map_tree, i_key);
  if (!v_element) {
    Array_append(dict->keys, key, 1);
    Array_append(dict->eles, ele, 1);
    v_element = Array_last_virt(dict->eles);
    AVLTree_set(dict->map_tree, i_key, v_element);
  } else {
    void *element = Array_virt2real(dict->eles, v_element);
    dict->allocator->memcpy(element, ele, dict->ele_size);
  }
}

inline uint32_t Dict_remove(Dict *dict, const void *keys[], uint32_t count) {
  for (uint32_t i = 0; i < count; i++) {
    uint64_t i_key = dict->fn_key ? dict->fn_key(keys[i]) : (uint64_t) keys[i];
    REFER(void) v_element = AVLTree_get(dict->map_tree, i_key);
    if (!v_element) { continue; }
    if (dict->fn_rel_key) { dict->fn_rel_key((void *) keys[i], dict->allocator); }
    if (dict->fn_rel_ele) {
      void *element = Array_virt2real(dict->eles, v_element);
      dict->fn_rel_ele(element, dict->allocator);
    }
    AVLTree_set(dict->map_tree, i_key, nullptr);
  }
  dict->tidied = false;
  return count;
}

inline void *Dict_get(const Dict *dict, const void *key) {
  uint64_t i_key = dict->fn_key ? dict->fn_key(key) : (uint64_t) key;
  const REFER(void) v_ele = AVLTree_get(dict->map_tree, i_key);
  return Array_virt2real(dict->eles, v_ele);
}

inline uint32_t Dict_update(Dict *dest, const Dict *dict, bool override) {
  if (dict->fn_key != dest->fn_key) { return 0; }
  if (dict->key_size != dest->key_size) { return 0; }
  if (dict->ele_size != dest->ele_size) { return 0; }
  uint32_t updated = 0;
  uint32_t old_count = Array_length(dest->keys);
  Array_concat(dest->keys, dict->keys);
  Array_concat(dest->eles, dict->eles);
  const uint32_t new_count = Array_length(dest->keys);
  const void *const keys = Array_first_real(dest->keys);
  for (uint32_t i = old_count; i < new_count; i++) {
    uint64_t i_key = dest->fn_key((keys + i * dest->key_size));
    if (!override && AVLTree_get(dest->map_tree, i_key)) { continue; }
    REFER(void) v_element = AVLTree_get(dict->map_tree, i_key);
    if (v_element) {
      AVLTree_set(dest->map_tree, i_key, v_element + old_count);
      updated ++;
    }
  }
  return updated;
}

inline void Dict_tidy(Dict *dict) {
  const uint64_t *keys = Array_first_real(dict->keys);
  const uint32_t count = Array_length(dict->keys);

  AVLTree *new_map_tree = AVLTree_new(dict->allocator, nullptr);
  Array *new_keys = Array_new(dict->key_size, dict->dict_id, dict->allocator);
  Array *new_eles = Array_new(dict->ele_size, dict->dict_id, dict->allocator);
  for (uint32_t i = 0; i < count; i++) {
    uint64_t i_key = dict->fn_key(&keys[i]);
    REFER(void) v_element = AVLTree_get(dict->map_tree, i_key);
    if (v_element) {
      void *element = Array_virt2real(dict->eles, v_element);
      Array_append(new_keys, &keys[i], 1);
      Array_append(new_eles, element, 1);
      AVLTree_set(new_map_tree, keys[i], Array_last_virt(new_eles));
    }
  }
  Dict_reset(dict);
  dict->keys = new_keys;
  dict->eles = new_eles;
  dict->map_tree = new_map_tree;
  dict->tidied = true;
}

inline void Dict_reset(Dict *dict) {
  if (dict->fn_rel_key) {
    Array_reset(dict->keys, dict->fn_rel_key);
    Array_destroy(dict->keys);
  } else {
    releasePrimeArray(dict->keys);
  }
  if (dict->fn_rel_ele) {
    Array_reset(dict->eles, dict->fn_rel_ele);
    Array_destroy(dict->eles);
  } else {
    releasePrimeArray(dict->eles);
  }
  AVLTree_destroy(dict->map_tree, nullptr);
  dict->tidied = true;
}

inline void Dict_destroy(Dict *dict) {
  Dict_reset(dict);
  dict->allocator->free(dict);
}

inline uint32_t Dict_count(Dict *dict) {
  if (!dict->tidied) { Dict_tidy(dict); }
  return Array_length(dict->keys);
}

inline void *Dict_keys(Dict *dict) {
  if (!dict->tidied) { Dict_tidy(dict); }
  return Array_first_real(dict->keys);
}

inline void *Dict_elements(Dict *dict) {
  if (!dict->tidied) { Dict_tidy(dict); }
  return Array_first_real(dict->eles);
}

uint64_t refer2u64(const REFER(void) *key) {
  return (uint64_t) *key;
}
