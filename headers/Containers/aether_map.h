#ifndef __AETHER_MAP__
#define __AETHER_MAP__

#include "aether_base.h"
#include <stdbool.h>

#define INIT_AE_MAP                                                                                \
    {                                                                                              \
        .data = INIT_AE_BASE, .occupancy = 0, .element_size = 0, .data_size = 0, .hash_func = NULL \
    }

typedef struct
{
    ae_base data;
    size_t occupancy;
    size_t element_size;
    size_t data_size;
    size_t (*hash_func)(const char *, size_t key_l);
} ae_map;

ae_map create_ae_map(size_t data_size, size_t (*func)(const char *, size_t));

void free_ae_map(ae_map *const map);

void *set_ae_map(ae_map *const map, const char *key, size_t key_l);

void *get_ae_map(const ae_map *const map, const char *key, size_t key_l);

void delete_ae_map(ae_map *const map, const char *key, size_t key_l, void *const par);

bool has_key_ae_map(const ae_map *const map, const char *key, size_t key_l);

#endif // __AETHER_MAP__
