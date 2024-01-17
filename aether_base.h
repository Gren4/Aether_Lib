#ifndef __AETHER_BASE__
#define __AETHER_BASE__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// #define DEBUG_AE

typedef struct ae_base
{
    void *memory;
    size_t quant;
} ae_base;

ae_base f_init_ae_base(void);

uint8_t f_create_ae_base(ae_base *base, size_t data_size, size_t storage_size);

uint8_t f_free_ae_base(ae_base *base);

uint8_t f_resize_ae_base(ae_base *base, size_t data_size, size_t storage_size, size_t new_size);

uint8_t f_append_ae_base(ae_base *base, size_t data_size, size_t storage_size, void *par);

uint8_t f_set_ae_base(ae_base *base, size_t data_size, size_t i, void *par);

uint8_t f_get_ae_base(ae_base *base, size_t data_size, size_t i, void *par);

uint8_t f_get_pointer_ae_base(ae_base *base, size_t data_size, size_t i, void **par);

uint8_t f_concat_ae_base(ae_base *base_to, ae_base *base_from, size_t data_size, size_t storage_size);

uint8_t f_insert_ae_base(ae_base *base, size_t data_size, size_t storage_size, size_t i, void *par);

uint8_t f_delete_ae_base(ae_base *base, size_t data_size, size_t storage_size, size_t i, void *par);

uint8_t f_duplicate_ae_base(ae_base *base_out, ae_base *base_in, size_t data_size, size_t storage_size);

uint8_t f_invert_ae_base(ae_base *base, size_t data_size);

uint8_t f_find_ae_base(ae_base *base, size_t data_size, void *par, size_t *ret);

#endif /* __AETHER_BASE__ */