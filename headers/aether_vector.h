#ifndef __AETHER_VECTOR__
#define __AETHER_VECTOR__

#include "aether_base.h"

#define INIT_AE_VECTOR                       \
    {                                        \
        .data = INIT_AE_BASE, .data_size = 0 \
    }

typedef struct ae_vec
{
    ae_base data;
    size_t data_size;
} ae_vec;

void create_gc_ae_vec(ae_vec *const gc, size_t data_size);

ae_vec create_ae_vec(size_t data_size, size_t quant);

inline size_t gc_idx_ae_vec(ae_vec *const vector)
{
    return gc_idx_ae_base(&vector->data);
}

inline size_t quant_ae_vec(ae_vec *const vector)
{
    return quant_ae_base(&vector->data);
}

inline size_t max_quant_ae_vec(ae_vec *const vector)
{
    return max_quant_ae_base(&vector->data);
}

inline size_t data_size_ae_vec(ae_vec *const vector)
{
    return vector->data_size;
}

void free_ae_vec(ae_vec *const vector);

void resize_ae_vec(ae_vec *const vector, size_t new_size);

void *append_ae_vec(ae_vec *const vector);

void append_vector_ae_vec(ae_vec *const vector_to, ae_vec *const vector_from, size_t i, size_t n);

void set_vector_ae_vec(ae_vec *const vector_to, ae_vec *const vector_from, size_t i_to, size_t i_from, size_t n);

void *get_ae_vec(const ae_vec *const vector, size_t i);

ae_vec concat_ae_vec(ae_vec *const c_vector, ae_vec *const vector_to, ae_vec *const vector_from);

void *insert_ae_vec(ae_vec *const vector, size_t i);

void insert_vector_ae_vec(ae_vec *const vector_to, ae_vec *const vector_from, size_t i_to, size_t i_from, size_t n);

void delete_ae_vec(ae_vec *const vector, size_t i, void *const par);

void delete_vector_ae_vec(ae_vec *const vector, size_t i, size_t n, ae_vec *par);

void pop_ae_vec(ae_vec *const vector, void *const par);

void pop_vector_ae_vec(ae_vec *const vector, size_t n, ae_vec *par);

ae_vec duplicate_ae_vec(ae_vec *const d_vector, const ae_vec *const vector);

void invert_ae_vec(ae_vec *const vector);

size_t find_ae_vec(const ae_vec *const vector, void *const par);

void sort_ae_vec(ae_vec *const vector, int (*comparator)(const void *, const void *));

void swap_ae_vec(ae_vec *const vector, size_t i, size_t j);

void optimize_ae_vec(ae_vec *const vector);

#endif // __AETHER_VECTOR__
