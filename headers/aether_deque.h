#ifndef __AETHER_DEQUE__
#define __AETHER_DEQUE__

#include "aether_base.h"

#define INIT_AE_DEQUE                                                                                                                  \
    {                                                                                                                                  \
        .pointers = INIT_AE_BASE, .blocks = INIT_AE_BASE, .front_i = 0, .back_i = 0, .front_block = 0, .back_block = 0, .data_size = 0 \
    }

#define AETHER_BLOCKS_ADD_SIZE 8
#define AETHER_DEQUE_ADD_SIZE 2

typedef struct
{
    ae_base pointers;
    ae_base blocks;
    int front_i;
    int back_i;
    size_t front_block;
    size_t back_block;
    size_t data_size;
} ae_deq;

ae_deq create_ae_deq(size_t data_size);

inline size_t quant_ae_deq(const ae_deq *const deque)
{
    return ((deque->back_block - deque->front_block) - 1) * AETHER_BLOCKS_ADD_SIZE + deque->back_i + AETHER_BLOCKS_ADD_SIZE - deque->front_i + 1;
}

void free_ae_deq(ae_deq *const deque);

void *push_front_ae_deq(ae_deq *const deque);

void pop_front_ae_deq(ae_deq *const deque, void *const par);

void *front_ae_deq(const ae_deq *const deque);

void *push_back_ae_deq(ae_deq *const deque);

void pop_back_ae_deq(ae_deq *const deque, void *const par);

void *back_ae_deq(const ae_deq *const deque);

void *get_ae_deq(const ae_deq *const deque, size_t i);

void *insert_ae_deq(ae_deq *const deque, size_t i);

void delete_ae_deq(ae_deq *const deque, size_t i, void *const par);

#endif // __AETHER_DEQUE__
