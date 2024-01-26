#ifndef __AETHER_VECTOR__
#define __AETHER_VECTOR__

#include "aether_base.h"

typedef struct ae_vector
{
    ae_base data;
    size_t data_size;
} ae_vector;

ae_vector create_ae_vector(size_t data_size, size_t quant);

void prepare_ae_vector(ae_vector *const vector, size_t quant);

uint8_t free_ae_vector(ae_vector *const vector);

uint8_t resize_ae_vector(ae_vector *const vector, size_t new_size);

uint8_t append_ae_vector(ae_vector *const vector, void *par);

uint8_t append_vector_ae_vector(ae_vector *const vector_to, ae_vector *const vector_from, size_t i, size_t n);

uint8_t set_ae_vector(ae_vector *const vector, size_t i, void *par);

uint8_t get_ae_vector(const ae_vector *const vector, size_t i, void *par);

uint8_t get_pointer_ae_vector(const ae_vector *const vector, size_t i, void **par);

uint8_t concat_ae_vector(ae_vector *const vector_to, ae_vector *const vector_from);

uint8_t insert_ae_vector(ae_vector *const vector, size_t i, void *par);

uint8_t insert_vector_ae_vector(ae_vector *const vector_to, ae_vector *const vector_from, size_t i_to, size_t i_from, size_t n);

uint8_t pop_ae_vector(ae_vector *const vector, void *par);

uint8_t pop_vector_ae_vector(ae_vector *const vector, size_t n, ae_vector *const par);

uint8_t delete_ae_vector(ae_vector *const vector, size_t i, void *par);

uint8_t delete_vector_ae_vector(ae_vector *const vector, size_t i, size_t n, ae_vector *const par);

uint8_t duplicate_ae_vector(ae_vector *const vector_out, const ae_vector *const vector_in);

uint8_t invert_ae_vector(ae_vector *const vector);

uint8_t find_ae_vector(const ae_vector *const vector, void *par, size_t *ret);

uint8_t sort_ae_vector(ae_vector *const vector, int (*comparator)(const void *, const void *));

uint8_t swap_ae_vector(ae_vector *const vector, size_t i, size_t j);

#endif // __AETHER_VECTOR__
