#ifndef __AETHER_CONVOLUTON__
#define __AETHER_CONVOLUTON__

#include "aether_layers.h"
#include "aether_activation_functions.h"

typedef struct aether_conv2d
{
    uint32_t in_channels;
    uint32_t out_channels;
    uint32_t kernel_size[2];
    uint32_t stride[2];
    uint32_t padding[2];
    uint32_t dilation[2];
    float (*activation_function)(const aether_af_path path, const float *const x);

    aether_tensor *input;
    aether_tensor output;
    aether_tensor weights;
    float *bias;
    aether_tensor af_cache;
    aether_tensor *gradient_in;
    aether_tensor gradient_out;
    aether_tensor d_weights;
    float *d_bias;
    int32_t gc_idx;
} aether_conv2d;

aether_layer init_ae_conv2d(const aether_nn_config *const config, void* layer);
void free_ae_conv2d(void* layer);
void forward_ae_conv2d(const aether_nn_config *const config, void* layer);
void backward_ae_conv2d(const aether_nn_config *const config, void* layer);


#endif // __AETHER_CONVOLUTON__
