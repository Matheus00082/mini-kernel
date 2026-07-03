#ifndef PP_H
#define PP_H

typedef struct PP PP;

PP * priority_create(void (*destroy)(void *));
void priority_push(void * r, void * data);
void * priority_remove(void* r);
int priority_size(void * p);
void priority_destroy(void * r);

#endif