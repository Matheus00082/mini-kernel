#ifndef VECTOR_H
#define VECTOR_H

typedef struct tVector tVector;

tVector *vector_create(int size);
void vector_push(tVector *vector, void *data);
void *vector_get(tVector *vector, int i);
void *vector_remove(tVector *vector, int i);
void *vector_pop_back(tVector *vector);
int vector_size(tVector *vector);
int vector_allocated(tVector *vector);
void vector_sort(tVector *v, int (*cmp)(const void *, const void *));
void vector_destroy(tVector *vector, void (*destroy)(void *));

#endif