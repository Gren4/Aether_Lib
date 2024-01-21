#ifndef __AETHER_DEQUE__
#define __AETHER_DEQUE__

#include "aether_base.h"

#define AETHER_LINK_ARRAY_ADD_SIZE 10
#define AETHER_DEQUE_ADD_SIZE 5

typedef struct
{
    ae_base pointers;
    int front_i;
    int back_i;
    size_t front_block;
    size_t back_block;
    size_t data_size;
} ae_deque;

ae_deque create_ae_deque(size_t data_size);

void prepare_ae_deque(ae_deque *deque);

uint8_t push_front_ae_deque(ae_deque *deque, void *par);

uint8_t push_back_ae_deque(ae_deque *deque, void *par);

uint8_t pop_front_ae_deque(ae_deque *deque, void *par);

uint8_t pop_back_ae_deque(ae_deque *deque, void *par);

uint8_t create_front_array_link_ae_deque(ae_deque *deque);

uint8_t create_back_array_link_ae_deque(ae_deque *deque);

uint8_t delete_front_array_link_ae_deque(ae_deque *deque);

uint8_t delete_back_array_link_ae_deque(ae_deque *deque);

uint8_t front_ae_deque(ae_deque *deque, void *par);

uint8_t back_ae_deque(ae_deque *deque, void *par);

uint8_t get_ae_deque(ae_deque *deque, size_t i, void *par);

uint8_t set_ae_deque(ae_deque *deque, size_t i, void *par);

uint8_t insert_ae_deque(ae_deque *deque, size_t i, void *par);

uint8_t delete_ae_deque(ae_deque *deque, size_t i, void *par);

uint8_t free_ae_deque(ae_deque *deque);

#endif // __AETHER_DEQUE__
