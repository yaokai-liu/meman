/**
* Filename: circ-queue.c
 * Creator: Yaokai Liu
 * Create Date: 2026-10-10
 * Copyright (c) 2026 Yaokai Liu. All rights reserved.
 **/

#include "circ-queue.h"

#include "array.h"
#include "_meman_builtin_.h"

typedef struct CircQueue /*extend Array*/ {
  const Allocator *allocator;
  void *  elements;
  uint32_t queue_id;
  uint32_t length;
  uint32_t count;
  uint32_t ele_size;
  uint32_t head;
  uint32_t tail;
} CircQueue;

const size_t sizeof_circ_queue = sizeof(CircQueue);

#ifndef ALLOC_LEN
#define ALLOC_LEN 32
#endif
#ifndef CIRC_QUEUE_ALLOC_SCALE
#define CIRC_QUEUE_ALLOC_SCALE 2
#endif
#define CIRC_QUEUE_ALLOC_LEN  (CIRC_QUEUE_ALLOC_SCALE * ALLOC_LEN)

static uint32_t CircQueue_extend_more_than(CircQueue *queue, uint32_t resize);
static void *CircQueue_real_addr(const CircQueue *queue, uint32_t index);
static void *CircQueue_virt_addr(const CircQueue *queue, uint32_t index);

CircQueue *CircQueue_new(const uint32_t ele_size, const uint32_t id, const Allocator * const allocator) {
  if (id == 0) { return nullptr; }
  if (ele_size == 0) { return nullptr; }
  CircQueue *queue = allocator->calloc(1, sizeof(CircQueue));
  CircQueue_init(queue, ele_size,  allocator);
  queue->queue_id = id;
  return queue;
}

uint32_t CircQueue_init(CircQueue *queue, const uint32_t ele_size, const Allocator *allocator) {
  queue->allocator = allocator;
  queue->ele_size = ele_size;
  queue->elements = allocator->malloc(ele_size * CIRC_QUEUE_ALLOC_LEN);
  queue->length = CIRC_QUEUE_ALLOC_LEN;
  queue->count = 0;
  queue->head = 0;
  queue->tail = 0;
  return ele_size;
}

#define min(a, b) ((a < b) ? (a) : (b))

inline uint32_t CircQueue_count(const CircQueue *queue) { return queue->count; }

#define aligned(count) (((count) / CIRC_QUEUE_ALLOC_LEN + 1) * CIRC_QUEUE_ALLOC_LEN)
#define extend_to_aligned(count)                                              \
  do {                                                                        \
    if ((count) >= queue->length) {                                           \
      uint32_t alloc_size = aligned(count) * queue->ele_size;                 \
      uint32_t new_size = (aligned(count) - queue->length) * queue->ele_size; \
      void *p = queue->allocator->realloc(queue->elements, alloc_size);       \
      if (!p) { return -1; }                                                  \
      queue->elements = p;                                                    \
      queue->length = aligned(count);                                         \
      queue->allocator->memset(queue->elements, 0, new_size);                 \
    }                                                                         \
  } while (false)

inline uint32_t CircQueue_extend_more_than(CircQueue *queue, const uint32_t resize) {
  if (resize <= queue->length) { return 0; }
  void *buffer = queue->allocator->malloc(queue->count * queue->ele_size);
  CircQueue_pop_array(queue, buffer, queue->count);
  extend_to_aligned(resize);
  queue->allocator->memcpy(queue->elements, buffer, queue->count * queue->ele_size);
  queue->length = aligned(resize);
  return resize;
}

inline uint32_t CircQueue_push(CircQueue *queue, const void *element) {
  CircQueue_extend_more_than(queue, queue->count + 1);
  queue->allocator->memcpy(CircQueue_tail_real(queue), element, queue->ele_size);
  queue->tail = (queue->tail + 1) % queue->length;
  queue->count += 1;
  return 1;
}

inline uint32_t CircQueue_pop(CircQueue *queue, void *dest) {
  if (!queue->count) { return 0; }
  queue->allocator->memcpy(dest, CircQueue_head_real(queue), queue->ele_size);
  queue->head = (queue->head + 1) % queue->length;
  queue->count -= 1;
  return 1;
}

inline uint32_t CircQueue_push_array(CircQueue *queue, const void *elements, const uint32_t count) {
  if (!count) { return 0; }
  CircQueue_extend_more_than(queue, queue->count + count);

  if (queue->tail + count <= queue->length) {
    queue->allocator->memcpy(CircQueue_tail_real(queue), elements, count * queue->ele_size);
  } else {
    const uint32_t head_copy_size = (queue->length - queue->tail) * queue->ele_size;
    queue->allocator->memcpy(CircQueue_tail_real(queue), elements, head_copy_size);
    const uint32_t tail_copy_size = (count - (queue->length - queue->tail)) * queue->ele_size;
    queue->allocator->memcpy(queue->elements, elements + head_copy_size, tail_copy_size);
  }
  queue->tail = (queue->tail + count) % queue->length;
  queue->count += count;

  return count;
}

inline uint32_t CircQueue_pop_array(CircQueue *queue, void *dest, uint32_t count) {
  count = min(queue->count, count);
  if (!count) { return 0; }
  if (queue->head <= queue->tail) {
    const uint32_t move_size = (queue->tail - queue->head) * queue->ele_size;
    queue->allocator->memcpy(dest, CircQueue_head_real(queue), move_size);
  } else {
    const uint32_t move_tail_size = (queue->length - queue->head) * queue->ele_size;
    queue->allocator->memcpy(dest, CircQueue_head_real(queue), move_tail_size);
    const uint32_t move_head_size = (count - (queue->length - queue->head)) * queue->ele_size;
    queue->allocator->memcpy(dest + move_tail_size, queue->elements, move_head_size);
  }
  queue->head = (queue->head + count) % queue->length;
  queue->count -= count;

  return count;
}

inline void *CircQueue_real_addr(const CircQueue *queue, const uint32_t index) {
  if (index >= queue->length) { return nullptr; }
  return (void *) queue->elements + queue->ele_size * index;
}

inline void *CircQueue_virt_addr(const CircQueue *queue, const uint32_t index) {
  if (index >= queue->length) { return nullptr; }
  const uint64_t id = ((uint64_t) queue->queue_id) << 32;
  return (void *) (id | index);
}

inline void *CircQueue_head_virt(const CircQueue *queue) {
  return CircQueue_virt_addr(queue, queue->head);
}

inline void *CircQueue_head_real(const CircQueue *queue) {
  return CircQueue_real_addr(queue, queue->head);
}

inline void *CircQueue_tail_virt(const CircQueue *queue) {
  return CircQueue_virt_addr(queue, queue->tail);
}

inline void *CircQueue_tail_real(const CircQueue *queue) {
  return CircQueue_real_addr(queue, queue->tail);
}

inline void *CircQueue_offs_virt(const CircQueue *queue, const uint32_t seed, const uint32_t offset) {
  switch (seed) {
    case CIRC_QUEUE_SEED_HEAD: { return CircQueue_virt_addr(queue, (queue->head + offset) % queue->length); }
    case CIRC_QUEUE_SEED_TAIL: { return CircQueue_virt_addr(queue, (queue->tail - offset) % queue->length); }
    default:{ return nullptr; }
  }
}

inline void *CircQueue_offs_real(const CircQueue *queue, const uint32_t seed, const uint32_t offset) {
  switch (seed) {
    case CIRC_QUEUE_SEED_HEAD: { return CircQueue_real_addr(queue, (queue->head + offset) % queue->length); }
    case CIRC_QUEUE_SEED_TAIL: { return CircQueue_real_addr(queue, (queue->tail - offset) % queue->length); }
    default:{ return nullptr; }
  }
}
