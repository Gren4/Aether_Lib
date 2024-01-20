#ifndef __AETHER_QUEUE__
#define __AETHER_QUEUE__

#include "aether_node.h"

typedef struct
{
    ae_node1w *front;
    ae_node1w *back;
    size_t data_size;
    size_t quant;
} ae_queue;

ae_queue create_ae_queue(size_t data_size);

uint8_t push_ae_queue(ae_queue *queue, void *par);

uint8_t pop_ae_queue(ae_queue *queue, void *par);

uint8_t front_ae_queue(ae_queue *queue, void *par);

uint8_t back_ae_queue(ae_queue *queue, void *par);

uint8_t free_ae_queue(ae_queue *queue);

#endif // __AETHER_QUEUE__
