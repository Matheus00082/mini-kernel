#ifndef HEAP_H
#define HEAP_H

#include <stdbool.h>

typedef struct tHeapNode tHeapNode;
typedef struct tHeap tHeap;

tHeap *heap_create(int allocated);
void heapify_up(tHeap *heap, int idx);
void heapify_down(tHeap *heap, int idx);
void heap_sort(tHeap *heap);
void heap_push(tHeap *heap, void *data, double priority);
bool heap_empty(tHeap *heap);
void *heap_pop(tHeap *heap);
void swap_nodes(tHeap *heap, int idx1, int idx2);
int heap_size(tHeap *heap);
void *heap_peek(tHeap *heap, int idx);
double heap_peek_priority(tHeap *heap, int idx);
void heap_destroy(tHeap *heap);
void *get_data(tHeap *heap, int idx);
void heap_update_priority(tHeap *heap, void *data, double new_priority);

#endif