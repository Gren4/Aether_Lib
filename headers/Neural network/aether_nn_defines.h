#ifndef __AETHER_NN_DEFINES__
#define __AETHER_NN_DEFINES__

#include <stdbool.h>

typedef enum
{
    FORWARD,
    BACKWARD
} aether_af_path;

typedef enum
{
    CONV2D,
    CONV_TR2D
} aether_layer_type;

typedef struct aether_nn_config
{
    bool is_train;
    float learning_rate;
} aether_nn_config;

#endif // __AETHER_NN_DEFINES__
