#include "heap.h"
#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

typedef struct tHeapNode
{
    void *data;
    double priority;
} tHeapNode;

typedef struct tHeap
{
    tHeapNode *nodes;
    int size;
    int allocated;
} tHeap;

tHeap *heap_create(int allocated)
{
    tHeap *heap = (tHeap *)calloc(1, sizeof(tHeap));
    heap->nodes = (tHeapNode *)calloc(allocated, sizeof(tHeapNode));
    heap->size = 0;
    heap->allocated = allocated;
    return heap;
}

void heapify_up(tHeap *heap, int idx)
{
    while (idx > 0)
    {
        int parent = (idx - 1) / 2;
        if (heap->nodes[parent].priority > heap->nodes[idx].priority)
        {
            swap_nodes(heap, idx, parent);
        }
        idx = parent;
    }
}

void heapify_down(tHeap *heap, int idx)
{
    int size = heap->size;
    while (true)
    {
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;
        int smallest = idx;

        if (left < size && heap->nodes[left].priority < heap->nodes[smallest].priority)
        {
            smallest = left;
        }
        if (right < size && heap->nodes[right].priority < heap->nodes[smallest].priority)
        {
            smallest = right;
        }

        if (smallest != idx)
        {
            swap_nodes(heap, idx, smallest);
            idx = smallest;
        }
        else
        {
            break;
        }
    }
}

void heap_sort(tHeap *heap)
{
    int original_size = heap->size;

    for (int i = heap->size - 1; i >= 0; i--)
    {
        swap_nodes(heap, 0, i);
        heap->size--;
        heapify_down(heap, 0);
    }

    heap->size = original_size;
}

void heap_push(tHeap *heap, void *data, double priority)
{
    if (heap->size == heap->allocated)
    {
        heap->allocated *= 2;
        heap->nodes = (tHeapNode *)realloc(heap->nodes, heap->allocated * sizeof(tHeapNode));
    }
    heap->nodes[heap->size].data = data;
    heap->nodes[heap->size].priority = priority;
    heapify_up(heap, heap->size);
    heap->size++;
}

void *heap_pop(tHeap *heap)
{
    if (heap_empty(heap))
    {
        exit(printf("Error::heap is empty\n"));
    }

    void *min_data = heap->nodes[0].data;

    if (heap->size >= 2)
    {
        swap_nodes(heap, 0, heap->size - 1);
    }

    heap->size--;
    heapify_down(heap, 0);

    return min_data;
}

bool heap_empty(tHeap *heap)
{
    return heap->size == 0;
}

int heap_size(tHeap *heap)
{
    return heap->size;
}

void *heap_peek(tHeap *heap, int idx)
{
    return heap->nodes[idx].data;
}

double heap_peek_priority(tHeap *heap, int idx)
{
    return heap->nodes[idx].priority;
}

void swap_nodes(tHeap *heap, int idx1, int idx2)
{
    tHeapNode temp = heap->nodes[idx1];
    heap->nodes[idx1] = heap->nodes[idx2];
    heap->nodes[idx2] = temp;
}

void heap_destroy(tHeap *heap, void (*destroy)(void *))
{
    if (destroy)
    {
        for (int i = 0; i < heap->size; i++)
        {
            destroy(heap->nodes[i].data);
        }
    }
    
    free(heap->nodes);
    free(heap);
}

void heap_update_priority(tHeap *heap, void *data, double new_priority)
{
    int index = -1;

    for (int i = 0; i < heap_size(heap); i++)
    {
        if (heap_peek(heap, i) == data)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        return;
    }

    double old_priority = heap_peek_priority(heap, index);
    heap->nodes[index].priority = new_priority;

    if (new_priority < old_priority)
    {
        heapify_up(heap, index);
    }
    else if (new_priority > old_priority)
    {
        heapify_down(heap, index);
    }
}