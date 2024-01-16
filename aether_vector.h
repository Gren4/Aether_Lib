#ifndef __AETHER_VECTOR__
#define __AETHER_VECTOR__

#include "aether_base.h"

#define AETHER_VECTOR_ADD_SIZE 5

typedef struct ae_vector
{
    ae_base data;
    size_t data_size;
} ae_vector;

ae_vector f_init_ae_vector(void);

uint8_t f_create_ae_vector(ae_vector *vector, size_t data_size);

uint8_t f_free_ae_vector(ae_vector *vector);

uint8_t f_resize_ae_vector(ae_vector *vector, size_t new_size);

uint8_t f_append_ae_vector(ae_vector *vector, void *par);

uint8_t f_set_ae_vector(ae_vector *vector, size_t i, void *par);

uint8_t f_get_ae_vector(ae_vector *vector, size_t i, void *par);

uint8_t f_concat_ae_vector(ae_vector *vector_to, ae_vector *vector_from);

uint8_t f_insert_ae_vector(ae_vector *vector, size_t i, void *par);

uint8_t f_delete_ae_vector(ae_vector *vector, size_t i, void *par);

uint8_t f_duplicate_ae_vector(ae_vector *vector_out, ae_vector *vector_in);

uint8_t f_invert_ae_vector(ae_vector *vector);

uint8_t f_find_ae_vector(ae_vector *vector, void *par, size_t *ret);

#endif /* __AETHER_VECTOR__ */