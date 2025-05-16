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
  Array *elements;
  AVLTree *key_tree;
  unikey_t *fn_key;
  uint32_t dict_id;
  uint32_t ele_size;
} Dict;

inline Dict *
Dict_new(uint32_t ele_size, key_t *fn_key, uint32_t dict_id, const Allocator *allocator) {
  if (!ele_size) { return nullptr; }
  Dict *dict = allocator->calloc(1, sizeof(Dict));
  dict->allocator = allocator;
  dict->elements = Array_new(ele_size, dict_id, allocator);
  dict->keys = Array_new(sizeof(uint64_t), dict_id, allocator);
  dict->key_tree = AVLTree_new(allocator, nullptr);
  dict->fn_key = fn_key;
  dict->dict_id = dict_id;
  dict->ele_size = ele_size;
  return dict;
}

inline void Dict_set(Dict *dict, const void *key, const void *ele) {
  uint64_t i_key = dict->fn_key ? dict->fn_key(key) : (uint64_t) key;
  REFER(void) v_element = AVLTree_get(dict->key_tree, i_key);
  if (!v_element) {
    Array_append(dict->keys, &key, 1);
    Array_append(dict->elements, &ele, 1);
    v_element = Array_last_virt(dict->elements);
    AVLTree_set(dict->key_tree, i_key, v_element);
  } else {
    void *element = Array_virt2real(dict->elements, v_element);
    dict->allocator->memcpy(element, ele, dict->ele_size);
  }
}

inline uint32_t Dict_remove(Dict *dict, const void *keys[], uint32_t count) {
  for (uint32_t i = 0; i < count; i++) {
    uint64_t key = dict->fn_key ? dict->fn_key(keys[i]) : (uint64_t) keys[i];
    REFER(void) v_element = AVLTree_get(dict->key_tree, key);
    if (!v_element) { continue; }
    AVLTree_set(dict->key_tree, key, nullptr);
  }
  Dict_tidy(dict);
  return count;
}

inline void *Dict_get(const Dict *dict, const void *key) {
  uint64_t i_key = dict->fn_key ? dict->fn_key(key) : (uint64_t) key;
  return Array_virt2real(dict->elements, AVLTree_get(dict->key_tree, i_key));
}

inline uint32_t Dict_update(Dict *dest, const Dict *dict) {
  if (dict->ele_size != dest->ele_size) { return 0; }
  uint32_t updated = 0;
  uint32_t offset = Array_length(dest->keys);
  Array_concat(dest->elements, dict->elements);
  const uint32_t count = Array_length(dict->keys);
  const uint64_t *const keys = Array_first_real(dict->keys);
  for (uint32_t i = 0; i < count; i++) {
    REFER(void) v_element = AVLTree_get(dict->key_tree, keys[i]);
    if (!v_element) { continue; }
    AVLTree_set(dest->key_tree, keys[i], v_element + offset);
    updated ++;
  }
  Dict_tidy(dest);
  return updated;
}

inline void Dict_tidy(Dict *dict) {
  const uint64_t *keys = Array_first_real(dict->keys);
  const uint32_t count = Array_length(dict->keys);

  AVLTree *new_key_tree = AVLTree_new(dict->allocator, nullptr);
  Array *new_keys = Array_new(sizeof(uint64_t), dict->dict_id, dict->allocator);
  Array *new_elements = Array_new(dict->ele_size, dict->dict_id, dict->allocator);
  for (uint32_t i = 0; i < count; i++) {
    REFER(void) v_element = AVLTree_get(dict->key_tree, keys[i]);
    if (!v_element) { continue; }
    void *element = Array_virt2real(dict->elements, v_element);
    Array_append(new_keys, &keys[i], 1);
    Array_append(new_elements, element, 1);
    AVLTree_set(new_key_tree, keys[i], Array_last_virt(new_elements));
  }
  Dict_reset(dict);
  dict->keys = new_keys;
  dict->elements = new_elements;
  dict->key_tree = new_key_tree;
}

inline void Dict_reset(Dict *dict) {
  releasePrimeArray(dict->keys);
  releasePrimeArray(dict->elements);
  AVLTree_destroy(dict->key_tree, nullptr);
}

inline void Dict_destroy(Dict *dict) {
  Dict_reset(dict);
  dict->allocator->free(dict);
}

inline uint32_t Dict_count(Dict *dict) {
  return Array_length(dict->elements);
}

inline void *Dict_data(Dict *dict) {
  return Array_first_real(dict->elements);
}
