#ifndef __AETHER_ACTIVATION_FUNCTIONS__
#define __AETHER_ACTIVATION_FUNCTIONS__

#include <math.h>
#include "aether_nn_defines.h"

static inline float Linear_ae_af(const aether_af_path path, const float *const x)
{
    switch(path)
    {
        case (FORWARD) :
            return *x;
        break;
        case (BACKWARD) :
            return 1.0f;
        break;
        default:
            return 0.0f;
        break;
    }
}

static inline float Sigmoid_ae_af(const aether_af_path path, const float *const x)
{
    switch(path)
    {
        case (FORWARD) :
            return 1.0f / (1.0f + expf(-(*x)));
        break;
        case (BACKWARD) :
            float temp = 1.0f / (1.0f + expf(-(*x)));
            return temp - temp * temp;
        break;
        default:
            return 0.0f;
        break;
    }
}

static inline float ReLU_ae_af(const aether_af_path path, const float *const x)
{
    switch(path)
    {
        case (FORWARD) :
            return (*x) > 0.0f ? (*x) : 0.0f;
        break;
        case (BACKWARD) :
            return (*x) > 0.0f ? 1.0f : 0.0f;
        break;
        default:
            return 0.0f;
        break;
    }
}

static inline float LeakyReLU_ae_af(const aether_af_path path, const float *const x)
{
    switch(path)
    {
        case (FORWARD) :
            return (*x) > 0.0f ? (*x) : 0.01f * (*x);
        break;
        case (BACKWARD) :
            return (*x) > 0.0f ? 1.0f : 0.01f;
        break;
        default:
            return 0.0f;
        break;
    }
}

static inline float TanH_ae_af(const aether_af_path path, const float *const x)
{
    switch(path)
    {
        case (FORWARD) :
            return (2.0f / (1.0f + expf(-2.0f * (*x)))) - 1.0f;
        break;
        case (BACKWARD) :
            float temp = (2.0f / (1.0f + expf(-2.0f * (*x)))) - 1.0f;
            return 1.0f - temp * temp;
        break;
        default:
            return 0.0f;
        break;
    }
}

static inline float SoftPlus_ae_af(const aether_af_path path, const float *const x)
{
    switch(path)
    {
        case (FORWARD) :
            return logf(1.0f + expf(*x));
        break;
        case (BACKWARD) :
            return 1.0f / (1.0f + expf(-(*x)));
        break;
        default:
            return 0.0f;
        break;
    }
}

#endif // __AETHER_ACTIVATION_FUNCTIONS__
