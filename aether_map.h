#ifndef __AETHER_MAP__
#define __AETHER_MAP__

#include "aether_vector.h"

#define AETHER_MAP_ADD_SIZE 10

#define AETHER_MAP_SUCCESS 0
#define AETHER_MAP_ERROR 1

typedef struct
{
    ae_vector *vector;
    void *io_buffer;
    size_t occupancy;
    size_t value_size;
} ae_map;
typedef struct
{
    uint8_t *key;
    void *value;
} ae_key_val;

size_t f_hash_function(uint8_t *str);

ae_map *f_create_ae_map(size_t value_size, uint8_t *status);
#define create_ae_map(type) f_create_ae_map(sizeof(type), NULL)
#define create_ae_map_debug(type, status) f_create_ae_map(sizeof(type), status)

void *f_free_ae_map(ae_map *map, uint8_t *status);
#define free_ae_map(map) map = f_free_ae_map(map, NULL)
#define free_ae_map_debug(map, status) map = f_free_ae_map(map, status)

void f_set_ae_map(ae_map *map, uint8_t *str, uint8_t *status);
#define set_ae_map(map, type, key, par) \
    {                                   \
        *(type *)map->io_buffer = par;  \
        f_set_ae_map(map, key, NULL);   \
    }
#define set_ae_map_debug(map, type, key, par, status) \
    {                                                 \
        *(type *)map->io_buffer = par;                \
        f_set_ae_map(map, key, status);               \
    }

size_t f_has_key_ae_map(ae_map *map, uint8_t *str, uint8_t *status);
#define has_key_ae_map(map, key) f_has_key_ae_map(map, key, NULL)
#define has_key_ae_map_debug(map, key, status) f_has_key_ae_map(map, key, status)

void *f_get_ae_map(ae_map *map, uint8_t *str, uint8_t *status);
#define get_ae_map(map, type, key) *(type *)f_get_ae_map(map, key, NULL)
#define get_ae_map_debug(map, type, key, status) *(type *)f_get_ae_map(map, key, status)

void *f_delete_ae_map(ae_map *map, uint8_t *str, uint8_t *status);
#define delete_ae_map(map, type, key) *(type *)f_delete_ae_map(map, key, NULL)
#define delete_ae_map_debug(map, type, key, status) *(type *)f_delete_ae_map(map, key, status)

ae_vector *f_get_keys_ae_map(ae_map *map, uint8_t *status);
#define get_keys_ae_map(map) f_get_keys_ae_map(map, NULL)
#define get_keys_ae_map_debug(map, status) f_get_keys_ae_map(map, status)

#endif /* __AETHER_MAP__ */