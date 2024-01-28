#ifndef __AETHER_MAP__
#define __AETHER_MAP__

#include "aether_vector.h"

typedef struct
{
    ae_base data;
    size_t occupancy;
    size_t max_size;
    size_t data_size;
    size_t (*hash_func)(const char *, size_t);
} ae_map;

typedef struct
{
    char *key;
    void *value;
} ae_key_val;

typedef struct
{
    size_t index;
    size_t bucket_index;
    bool result;
} ae_return_key_val;

size_t hash_function(const char *key, size_t len);

ae_map create_ae_map(size_t data_size, size_t (*func)(const char *, size_t len));

void prepare_ae_map(ae_map *map, size_t (*func)(const char *, size_t len));

uint8_t free_ae_map(ae_map *map);

uint8_t create_ae_key_val(ae_key_val *kv, const char *key, void *par, size_t data_size);

uint8_t resize_ae_map(ae_map *map);

uint8_t set_ae_map(ae_map *map, const char *key, void *par);

uint8_t find_key_ae_map(ae_map *map, const char *key, ae_return_key_val *res);

bool has_key_ae_map(ae_map *map, const char *key);

uint8_t get_ae_map(ae_map *map, const char *key, void *par);

uint8_t delete_ae_map(ae_map *map, const char *key, void *par);

ae_vector get_keys_ae_map(ae_map *map);

#endif // __AETHER_MAP__
