#ifndef __AETHER_BASE__
#define __AETHER_BASE__

#include <stdint.h>
#include <stdlib.h>

typedef enum
{
    AE_BASE_INCR,
    AE_BASE_DECR,
    AE_BASE_RECOUNT,
    AE_RESIZE
} AE_BASE_MEM;

typedef struct ae_base
{
    void *memory;
    size_t quant;
    size_t max_quant;
} ae_base;

uint8_t check_realloc_ae_base(ae_base *const base, const size_t *const data_size, size_t new_size, AE_BASE_MEM type);

ae_base init_ae_base(void);

uint8_t create_ae_base(ae_base *const base, const size_t *const data_size, size_t new_size);

uint8_t create_max_size_ae_base(ae_base *const base, const size_t *const data_size, size_t new_size);

uint8_t free_ae_base(ae_base *const base);

uint8_t resize_ae_base(ae_base *const base, const size_t *const data_size, size_t new_size);

size_t current_max_size_ae_base(ae_base *const base);

uint8_t append_return_ae_base(ae_base *const base, const size_t *const data_size, void **par);

uint8_t append_ae_base(ae_base *const base, const size_t *const data_size, void *par);

uint8_t append_base_ae_base(ae_base *const base_to, ae_base *const base_from, const size_t *const data_size, size_t i, size_t n);

uint8_t set_ae_base(ae_base *const base, const size_t *const data_size, size_t i, void *par);

uint8_t set_base_ae_base(ae_base *const base_to, ae_base *const base_from, const size_t *const data_size, size_t i_to, size_t i_from, size_t n);

uint8_t get_ae_base(const ae_base *const base, const size_t *const data_size, size_t i, void *par);

uint8_t get_pointer_ae_base(const ae_base *const base, const size_t *const data_size, size_t i, void **par);

uint8_t concat_ae_base(ae_base *const base_to, ae_base *const base_from, const size_t *const data_size);

uint8_t insert_ae_base(ae_base *const base, const size_t *const data_size, size_t i, void *par);

uint8_t insert_base_ae_base(ae_base *const base_to, ae_base *const base_from, const size_t *const data_size, size_t i_to, size_t i_from, size_t n);

uint8_t delete_ae_base(ae_base *const base, const size_t *const data_size, size_t i, void *par);

uint8_t delete_base_ae_base(ae_base *const base, const size_t *const data_size, size_t i, size_t n, ae_base *const par);

uint8_t pop_ae_base(ae_base *const base, const size_t *const data_size, void *par);

uint8_t pop_base_ae_base(ae_base *const base, const size_t *const data_size, size_t n, ae_base *const par);

uint8_t duplicate_ae_base(ae_base *const base_out, const ae_base *const base_in, const size_t *const data_size);

uint8_t invert_ae_base(ae_base *const base, const size_t *const data_size);

uint8_t find_ae_base(const ae_base *const base, const size_t *const data_size, void *par, size_t *ret);

uint8_t sort_ae_base(ae_base *const base, const size_t *const data_size, int (*comparator)(const void *, const void *));

uint8_t swap_ae_base(ae_base *const base, const size_t *const data_size, size_t i, size_t j);

#endif // __AETHER_BASE__
