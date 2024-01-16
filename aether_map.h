#ifndef __AETHER_MAP__
#define __AETHER_MAP__

#include "aether_vector.h"

#define AETHER_MAP_ADD_SIZE 4
#define AETHER_BUCKET_ADD_SIZE 4

typedef struct
{
    ae_base data;
    size_t occupancy;
    size_t max_size;
    size_t data_size;
    size_t (*hash_func)(uint8_t *);
} ae_map;

typedef struct
{
    uint8_t *key;
    void *value;
} ae_key_val;

typedef struct
{
    size_t index;
    size_t bucket_index;
    bool result;
} ae_return_key_val;

size_t f_hash_function(uint8_t *key);

ae_map f_init_ae_map(void);

uint8_t f_create_ae_map(ae_map *map, size_t data_size, size_t (*func)(uint8_t *));

uint8_t f_free_ae_map(ae_map *map);

uint8_t f_create_ae_key_val(ae_key_val *kv, uint8_t *key, void *par, size_t data_size);

uint8_t f_resize_ae_map(ae_map *map);

uint8_t f_set_ae_map(ae_map *map, uint8_t *key, void *par);

uint8_t f_find_key_ae_map(ae_map *map, uint8_t *key, ae_return_key_val *res);

bool f_has_key_ae_map(ae_map *map, uint8_t *key);

uint8_t f_get_ae_map(ae_map *map, uint8_t *key, void *par);

uint8_t f_delete_ae_map(ae_map *map, uint8_t *key, void *par);

ae_vector f_get_keys_ae_map(ae_map *map);

#endif /* __AETHER_MAP__ */