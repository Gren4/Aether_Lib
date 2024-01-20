#ifndef __AETHER_DEQUE__
#define __AETHER_DEQUE__

#include "aether_node.h"

typedef struct
{
    ae_node2w *front;
    ae_node2w *back;
    size_t data_size;
    size_t quant;
} ae_deque;

ae_deque create_ae_deque(size_t data_size);

uint8_t push_front_ae_deque(ae_deque *deque, void *par);

uint8_t push_back_ae_deque(ae_deque *deque, void *par);

uint8_t pop_front_ae_deque(ae_deque *deque, void *par);

uint8_t pop_back_ae_deque(ae_deque *deque, void *par);

uint8_t front_ae_deque(ae_deque *deque, void *par);

uint8_t back_ae_deque(ae_deque *deque, void *par);

uint8_t insert_ae_deque(ae_deque *deque, size_t i, void *par);

uint8_t delete_ae_deque(ae_deque *deque, size_t i, void *par);

uint8_t free_ae_deque(ae_deque *deque);

#endif // __AETHER_DEQUE__
