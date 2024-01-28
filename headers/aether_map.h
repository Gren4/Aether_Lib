#ifndef __AETHER_MAP__
#define __AETHER_MAP__

#include "aether_vector.h"

typedef struct
{
    ae_base data;
    size_t occupancy;
    size_t element_size;
    size_t data_size;
    size_t (*hash_func)(const char *, size_t key_l);
} ae_map;

typedef struct
{
    size_t index;
    size_t bucket_index;
    bool result;
} ae_return_key_val;

ae_map create_ae_map(size_t data_size, size_t quant, size_t (*func)(const char *, size_t));

void prepare_ae_map(ae_map *map, size_t (*func)(const char *));

uint8_t free_ae_map(ae_map *map);

uint8_t resize_ae_map(ae_map *map);

uint8_t set_ae_map(ae_map *map, const char *key, size_t key_l, void *par);

bool has_key_ae_map(ae_map *map, const char *key, size_t key_l);

uint8_t get_ae_map(ae_map *map, const char *key, size_t key_l, void *par);

uint8_t delete_ae_map(ae_map *map, const char *key, size_t key_l, void *par);

ae_vector get_keys_ae_map(ae_map *map);

#endif // __AETHER_MAP__
