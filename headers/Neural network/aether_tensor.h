#ifndef __AETHER_TENSOR__
#define __AETHER_TENSOR__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "aether_gc.h"

#define INIT_AE_TENSOR                                          \
    {                                                           \
        .data = NULL, .gc_idx = 0,                              \
        .filters = 0, .height = 0, .width = 0,                  \
        .indx_coeff = 0, .max_quant = 0                         \
    }

typedef struct aether_tensor
{
    uint32_t batch;
    uint32_t filters;
    uint32_t height;
    uint32_t width;
    uint32_t indx_coeff_1;
    uint32_t indx_coeff_2;
    uint32_t max_quant;
    float* data;
    int32_t gc_idx;
} aether_tensor;

static inline void create_ae_tensor(aether_tensor *tensor, const uint32_t batch, const uint32_t filters, const uint32_t height, const uint32_t width)
{
    tensor->batch = batch;
    tensor->filters = filters;
    tensor->height = height;
    tensor->width = width;
    tensor->indx_coeff_1 = filters * height * width;
    tensor->indx_coeff_2 = height * width;
    tensor->max_quant = batch * filters * height * width;
    tensor->data = (float*)calloc(tensor->max_quant, sizeof(float));
    if (tensor->data == NULL)
        return;
#ifdef DEBUG_AE
    printf("Allocated memory at address %p\n", tensor->data);
#endif
    tensor->gc_idx = append_ae_gc(tensor->data);

    return;
}

static inline void create_empty_ae_tensor(aether_tensor *tensor, const uint32_t batch, const uint32_t filters, const uint32_t height, const uint32_t width)
{
    tensor->batch = batch;
    tensor->filters = filters;
    tensor->height = height;
    tensor->width = width;
    tensor->indx_coeff_1 = filters * height * width;
    tensor->indx_coeff_2 = height * width;
    tensor->max_quant = batch * filters * height * width;
    tensor->data = NULL;

    return;
}

static inline void create_random_ae_tensor(aether_tensor *tensor, const uint32_t batch, const uint32_t filters, const uint32_t height, const uint32_t width)
{
    create_ae_tensor(tensor, batch, filters, height, width);

    if (tensor->data == NULL)
        return;

    uint32_t i = 0;
    for (; i < tensor->max_quant; i++)
        tensor->data[i] = 2.0f * ((float)rand() / (float)RAND_MAX) - 1.0f;

    return;
}

static inline void create_zero_ae_tensor(aether_tensor *tensor, const uint32_t batch, const uint32_t filters, const uint32_t height, const uint32_t width)
{
    create_ae_tensor(tensor, batch, filters, height, width);

    if (tensor->data == NULL)
        return;

    memset(tensor->data, 0, tensor->max_quant * sizeof(float));

    return;
}

static inline void create_from_array_ae_tensor(aether_tensor *tensor, const uint32_t batch, const uint32_t filters, const uint32_t height, const uint32_t width, const float* const data)
{
    tensor->batch = batch;
    tensor->filters = filters;
    tensor->height = height;
    tensor->width = width;
    tensor->indx_coeff_1 = filters * height * width;
    tensor->indx_coeff_2 = height * width;
    tensor->max_quant = batch * filters * height * width;
    tensor->data = (float*)calloc(tensor->max_quant, sizeof(float));
    if (tensor->data == NULL)
        return;
#ifdef DEBUG_AE
    printf("Allocated memory at address %p\n", tensor->data);
#endif
    if (data != NULL)
        memcpy(tensor->data, data, tensor->max_quant * sizeof(float));
    tensor->gc_idx = append_ae_gc(tensor->data);

    return;
}

static inline void duplicate_ae_tensor(aether_tensor *tensor_to, aether_tensor *tensor_from)
{
    tensor_to->batch = tensor_from->batch;
    tensor_to->filters = tensor_from->filters;
    tensor_to->height = tensor_from->height;
    tensor_to->width = tensor_from->width;
    tensor_to->indx_coeff_1 = tensor_from->filters * tensor_from->height * tensor_from->width;
    tensor_to->indx_coeff_2 = tensor_from->height * tensor_from->width;
    tensor_to->max_quant = tensor_from->batch * tensor_from->filters * tensor_from->height * tensor_from->width;
    tensor_to->data = (float*)calloc(tensor_from->max_quant, sizeof(float));
    if (tensor_to->data == NULL)
        return;
#ifdef DEBUG_AE
    printf("Allocated memory at address %p\n", tensor_to->data);
#endif
    if (tensor_from->data != NULL)
        memcpy(tensor_to->data, tensor_from->data, tensor_from->max_quant * sizeof(float));
    tensor_to->gc_idx = append_ae_gc(tensor_from->data);

    return;
}

static inline void free_ae_tensor(aether_tensor *tensor)
{
    if (tensor == NULL || tensor->data == NULL)
        return;
#ifdef DEBUG_AE
    printf("Freed memory at address %p\n", tensor->data);
#endif
    free(tensor->data);
    remove_ae_gc(tensor->gc_idx);

    return;
}

static inline void update_ae_tensor(aether_tensor *tensor, const uint32_t batch, const uint32_t filters, const uint32_t height, const uint32_t width)
{
    if (tensor != NULL)
    {
        tensor->batch = batch;
        tensor->filters = filters;
        tensor->height = height;
        tensor->width = width;
        tensor->indx_coeff_1 = filters * height * width;
        tensor->indx_coeff_2 = height * width;
        tensor->max_quant = batch * filters * height * width;
        float *p = (float*)realloc(tensor->data, tensor->max_quant * sizeof(float));
        if (p == NULL)
            return;
#ifdef DEBUG_AE
        printf("Reallocated memory from address %p to %p\n", tensor->data, p);
#endif
        tensor->data = p;
        update_ae_gc(tensor->gc_idx, tensor->data);
    }
    else
    {
        create_ae_tensor(tensor, batch, filters, height, width);
    }

    return;
}

static inline float *get_element_ae_tensor(const aether_tensor* const tensor, const uint32_t batch, const uint32_t filter, const uint32_t height, const uint32_t width)
{
    return &tensor->data[batch * tensor->indx_coeff_1 + filter * tensor->indx_coeff_2 + height * tensor->width + width];
}

static inline bool check_dims_ae_tensor(const aether_tensor* const tensor, const uint32_t batch, const uint32_t filter, const uint32_t height, const uint32_t width)
{
    return tensor->batch == batch && tensor->filters == filter && tensor->height == height && tensor->width == width;
}

static inline bool compare_tensors_ae_tensor(const aether_tensor* const tensor_1, const aether_tensor* const tensor_2)
{
    return tensor_1->batch == tensor_2->batch && tensor_1->filters == tensor_2->filters && tensor_1->height == tensor_2->height && tensor_1->width == tensor_2->width;
}

static inline void copy_ae_tensor(aether_tensor *tensor_to, aether_tensor *tensor_from)
{
    if ((tensor_to->data == NULL) || compare_tensors_ae_tensor(tensor_to, tensor_from) == false)
        update_ae_tensor(tensor_to, tensor_from->batch, tensor_from->filters, tensor_from->height, tensor_from->width);

    if (tensor_from->data != NULL)
        memcpy(tensor_to->data, tensor_from->data, tensor_from->max_quant * sizeof(float));

    return;
}

static inline void set_from_array_ae_tensor(aether_tensor *tensor, const uint32_t batch, const uint32_t filters, const uint32_t height, const uint32_t width, const float* const data)
{
    if ((tensor->data == NULL) || check_dims_ae_tensor(tensor, batch, filters, height, width) == false)
        update_ae_tensor(tensor, batch, filters, height, width);

    if (data != NULL)
        memcpy(tensor->data, data, tensor->max_quant * sizeof(float));

    return;
}

static inline void print_ae_tensor(aether_tensor *tensor)
{
    for (uint32_t b = 0; b < tensor->batch; b++)
    {
        printf("Batch #%d:\n", b + 1);
        for (uint32_t f = 0; f < tensor->filters; f++)
        {
            printf("Filter #%d:\n", f + 1);
            for (uint32_t h = 0; h < tensor->height; h++)
            {
                for (uint32_t w = 0; w < tensor->width; w++)
                {
                    printf("% 3.1f ", *get_element_ae_tensor(tensor, b, f, h, w));
                }
                printf("\n");
            }
            printf("\n\n");
        }
        printf("\n\n");
    }

    return;
}

#endif // __AETHER_TENSOR__
