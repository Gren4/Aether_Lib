#ifndef __AETHER_SEQUENCE__
#define __AETHER_SEQUENCE__

#include "aether_nn_defines.h"
#include "aether_tensor.h"
#include "aether_layers.h"

typedef struct aether_sequence
{
    aether_nn_config config;
    uint32_t layers_quant;
    aether_layer *layers;
    aether_tensor **input;
    aether_tensor *output;
    aether_tensor loss;
    int32_t gc_idx;
} aether_sequence;

void init_ae_sequence(aether_sequence *sequence, bool is_train, float learning_rate);
void free_ae_sequence(aether_sequence *sequence);
void add_layer_ae_sequence(aether_sequence *sequence, aether_layer layer);
void forward_ae_sequence(aether_sequence *sequence, const aether_tensor * const input);
void backward_ae_sequence(aether_sequence *sequence);

#endif // __AETHER_SEQUENCE__
