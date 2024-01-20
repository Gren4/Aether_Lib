#ifndef __AETHER_BASE__
#define __AETHER_BASE__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct ae_base
{
    void *memory;
    size_t quant;
} ae_base;

ae_base init_ae_base(void);

uint8_t create_ae_base(ae_base *base, size_t data_size, size_t storage_size);

uint8_t free_ae_base(ae_base *base);

uint8_t resize_ae_base(ae_base *base, size_t data_size, size_t storage_size, size_t new_size);

uint8_t append_ae_base(ae_base *base, size_t data_size, size_t storage_size, void *par);

uint8_t append_base_ae_base(ae_base *base_to, ae_base *base_from, size_t data_size, size_t storage_size, size_t i, size_t n);

uint8_t set_ae_base(ae_base *base, size_t data_size, size_t i, void *par);

uint8_t get_ae_base(ae_base *base, size_t data_size, size_t i, void *par);

uint8_t get_pointer_ae_base(ae_base *base, size_t data_size, size_t i, void **par);

uint8_t concat_ae_base(ae_base *base_to, ae_base *base_from, size_t data_size, size_t storage_size);

uint8_t insert_ae_base(ae_base *base, size_t data_size, size_t storage_size, size_t i, void *par);

uint8_t insert_base_ae_base(ae_base *base_to, ae_base *base_from, size_t data_size, size_t storage_size, size_t i_to, size_t i_from, size_t n);

uint8_t delete_ae_base(ae_base *base, size_t data_size, size_t storage_size, size_t i, void *par);

uint8_t duplicate_ae_base(ae_base *base_out, ae_base *base_in, size_t data_size, size_t storage_size);

uint8_t invert_ae_base(ae_base *base, size_t data_size);

uint8_t find_ae_base(ae_base *base, size_t data_size, void *par, size_t *ret);

uint8_t sort_ae_base(ae_base *base, size_t data_size, int (*comparator)(const void *, const void *));

#endif /* __AETHER_BASE__ */
