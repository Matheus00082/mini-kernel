#ifndef RR_H
#define RR_H

typedef struct RR RR;

RR * round_robin_create(void (*destroy)(void *));
void round_robin_push(void * r, void * data);
int round_robin_size(void * r);
void * round_robin_remove(void* r);
void round_robin_destroy(void * r);

#endif