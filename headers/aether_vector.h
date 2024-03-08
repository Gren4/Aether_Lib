#ifndef __AETHER_VECTOR__
#define __AETHER_VECTOR__

#include "aether_base.h"

#define INIT_AE_VECTOR                       \
    {                                        \
        .data = INIT_AE_BASE, .data_size = 0 \
    }

typedef struct ae_vector
{
    ae_base data;
    size_t data_size;
} ae_vector;

void create_gc_ae_vector(ae_vector *gc, size_t data_size);

ae_vector create_ae_vector(size_t data_size, size_t quant);

size_t gc_idx_ae_vector(ae_vector *const vector);

size_t quant_ae_vector(ae_vector *const vector);

size_t max_quant_ae_vector(ae_vector *const vector);

size_t data_size_ae_vector(ae_vector *const vector);

void free_ae_vector(ae_vector *const vector);

void resize_ae_vector(ae_vector *const vector, size_t new_size);

void *append_ae_vector(ae_vector *const vector);

void append_vector_ae_vector(ae_vector *const vector_to, ae_vector *const vector_from, size_t i, size_t n);

void set_vector_ae_vector(ae_vector *const vector_to, ae_vector *const vector_from, size_t i_to, size_t i_from, size_t n);

void *get_ae_vector(const ae_vector *const vector, size_t i);

ae_vector concat_ae_vector(ae_vector *c_vector, ae_vector *const vector_to, ae_vector *const vector_from);

void *insert_ae_vector(ae_vector *const vector, size_t i);

void insert_vector_ae_vector(ae_vector *const vector_to, ae_vector *const vector_from, size_t i_to, size_t i_from, size_t n);

void delete_ae_vector(ae_vector *const vector, size_t i, void *par);

void delete_vector_ae_vector(ae_vector *const vector, size_t i, size_t n, ae_vector *par);

void pop_ae_vector(ae_vector *const vector, void *par);

void pop_vector_ae_vector(ae_vector *const vector, size_t n, ae_vector *par);

ae_vector duplicate_ae_vector(ae_vector *d_vector, const ae_vector *const vector);

void invert_ae_vector(ae_vector *const vector);

size_t find_ae_vector(const ae_vector *const vector, void *par);

void sort_ae_vector(ae_vector *const vector, int (*comparator)(const void *, const void *));

void swap_ae_vector(ae_vector *const vector, size_t i, size_t j);

void optimize_ae_vector(ae_vector *const vector);

#endif // __AETHER_VECTOR__
