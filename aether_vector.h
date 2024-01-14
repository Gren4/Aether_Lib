#ifndef __AETHER_VECTOR__
#define __AETHER_VECTOR__

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define AETHER_VECTOR_ADD_SIZE 5

#define AETHER_VECTOR_SUCCESS 0
#define AETHER_VECTOR_ERROR 1

typedef struct ae_vector
{
    void *data;
    void *io_buffer;
    size_t quantity;
    size_t data_size;
} ae_vector;

ae_vector *f_create_ae_vector(size_t data_size, uint8_t *status);
#define create_ae_vector(type) f_create_ae_vector(sizeof(type), NULL)
#define create_ae_vector_debug(type, status) f_create_ae_vector(sizeof(type), status)

void *f_free_ae_vector(ae_vector *vector, uint8_t *status);
#define free_ae_vector(vector) vector = f_free_ae_vector(vector, NULL)
#define free_ae_vector_debug(vector, status) vector = f_free_ae_vector(vector, status)

void f_resize_ae_vector(ae_vector *vector, size_t new_size, uint8_t *status);
#define resize_ae_vector(vector, new_size) f_resize_ae_vector(vector, new_size, NULL)
#define resize_ae_vector_debug(vector, new_size, status) f_resize_ae_vector(vector, new_size, status)

#define size_ae_vector(vector) vector->quantity

void f_append_ae_vector(ae_vector *vector, uint8_t *status);
#define append_ae_vector(vector, type, par) \
    {                                       \
        *(type *)vector->io_buffer = par;   \
        f_append_ae_vector(vector, NULL);   \
    }
#define append_ae_vector_debug(vector, type, par, status) \
    {                                                     \
        *(type *)vector->io_buffer = par;                 \
        f_append_ae_vector(vector, status);               \
    }

void f_set_ae_vector(ae_vector *vector, size_t i, uint8_t *status);
#define set_ae_vector(vector, i, type, par) \
    {                                       \
        *(type *)vector->io_buffer = par;   \
        f_set_ae_vector(vector, i, NULL);   \
    }
#define set_ae_vector_debug(vector, i, type, par, status) \
    {                                                     \
        *(type *)vector->io_buffer = par;                 \
        f_set_ae_vector(vector, i, status);               \
    }

void *f_get_ae_vector(ae_vector *vector, size_t i, uint8_t *status);
#define get_ae_vector(vector, i, type) *(type *)f_get_ae_vector(vector, i, NULL)
#define get_ae_vector_debug(vector, i, type, status) *(type *)f_get_ae_vector(vector, i, status)

void f_concat_ae_vector(ae_vector *vector_to, ae_vector *vector_from, uint8_t *status);
#define concat_ae_vector(vector_to, vector_from) f_concat_ae_vector(vector_to, vector_from, NULL)
#define concat_ae_vector_debug(vector_to, vector_from, status) f_concat_ae_vector(vector_to, vector_from, status)

void f_insert_ae_vector(ae_vector *vector, size_t i, uint8_t *status);
#define insert_ae_vector(vector, i, type, par) \
    {                                          \
        *(type *)vector->io_buffer = par;      \
        f_insert_ae_vector(vector, i, NULL);   \
    }
#define insert_ae_vector_debug(vector, i, par, type, status) \
    {                                                        \
        *(type *)vector->io_buffer = par;                    \
        f_insert_ae_vector(vector, i, status);               \
    }

void *f_delete_ae_vector(ae_vector *vector, size_t i, uint8_t *status);
#define delete_ae_vector(vector, i, type) *(type *)f_delete_ae_vector(vector, i, NULL)
#define delete_ae_vector_debug(vector, i, type, status) *(type *)f_delete_ae_vector(vector, i, status)

ae_vector *f_duplicate_ae_vector(ae_vector *vector, uint8_t *status);
#define duplicate_ae_vector(vector) f_duplicate_ae_vector(vector, NULL)
#define duplicate_ae_vector_debug(vector, status) f_duplicate_ae_vector(vector, status)

void f_invert_ae_vector(ae_vector *vector, uint8_t *status);
#define invert_ae_vector(vector) f_invert_ae_vector(vector, NULL)
#define invert_ae_vector_debug(vector, status) f_invert_ae_vector(vector, status)

#endif /* __AETHER_VECTOR__ */