#ifndef __AETHER_LAYERS__
#define __AETHER_LAYERS__

#include "aether_nn_defines.h"
#include "aether_tensor.h"
#include "assert.h"

typedef struct aether_layer
{
    aether_layer_type type;
    aether_tensor **input;
    aether_tensor *output;
    aether_tensor **gradient_in;
    aether_tensor *gradient_out;
    void *layer;
    void (*layer_forward)(const aether_nn_config *const config, void* layer);
    void (*layer_backward)(const aether_nn_config *const config, void* layer);
    void (*layer_free)(void* layer);
} aether_layer;

#endif // __AETHER_LAYERS__
