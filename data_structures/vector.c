#include "vector.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct tVector
{
    void **data;
    int allocated;
    int size;
} tVector;

tVector *vector_create(int allocated)
{
    tVector *v = (tVector *)calloc(1, sizeof(tVector));
    v->data = (void **)calloc(allocated, sizeof(void *));
    v->allocated = allocated;
    v->size = 0;

    return v;
}

void vector_push(tVector *vector, void *data)
{
    if (vector->size == vector->allocated)
    {
        vector->allocated *= 2;
        vector->data = (void **)realloc(vector->data, vector->allocated * sizeof(void *));
    }
    vector->data[vector->size++] = data;
}

int vector_size(tVector *vector)
{
    return vector->size;
}

int vector_allocated(tVector *vector)
{
    return vector->allocated;
}

void *vector_get(tVector *vector, int i)
{
    if (i < vector_size(vector) && i >= 0)
    {
        return vector->data[i];
    }

    printf("ERRO: indice invalido_VECTOR_GET\n");
    exit(1);
}

void *vector_pop_back(tVector *vector)
{
    if (vector_size(vector) > 0)
    {
        return vector->data[--vector->size];
    }

    printf("ERRO: vetor vazio\n");
    exit(1);
}

void *vector_remove(tVector *vector, int i)
{
    if (i < vector_size(vector) && i >= 0)
    {
        void *data = vector->data[i];
        for (int j = i; j < vector_size(vector) - 1; j++)
        {
            vector->data[j] = vector->data[j + 1];
        }
        vector->size--;
        return data;
    }

    printf("ERRO: indice invalido_VECTOR_REMOVE\n");
    exit(1);
}

void vector_sort(tVector *v, int (*cmp)(const void *, const void *)) {
    if (v == NULL || v->size <= 1 || v->data == NULL) {
        return;
    }
    
    qsort(v->data, v->size, sizeof(void*), cmp);
}

void vector_destroy(tVector *vector, void (*destroy)(void *))
{
    if (destroy)
    {
        for (int i = 0; i < vector->size; i++)
        {
            destroy(vector->data[i]);
        }
    }

    free(vector->data);
    free(vector);
}