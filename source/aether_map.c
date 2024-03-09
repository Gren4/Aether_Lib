#include "aether_map.h"
#include <string.h>
#include <math.h>

typedef enum
{
    EMPTY_AE,
    DELETED_AE,
    USED_AE
} AE_MAP_ELEMNT;

#define AETHER_MAP_ADD_SIZE 4

static const size_t uint8_t_size = sizeof(uint8_t);
static const size_t uint32_t_size = sizeof(uint32_t);

#define STATUS_AE(kv) *(uint8_t *)(kv)
#define P_STATUS_AE(kv) (kv)
#define KEY_AE(kv) *(uint32_t *)(kv + uint8_t_size)
#define P_KEY_AE(kv) (kv + uint8_t_size)
#define P_DATA_AE(kv) (kv + uint8_t_size + uint32_t_size)

static size_t hash_function(const char *key, size_t len)
{
    const size_t m = 0x5bd1e995;
    const size_t seed = 0;
    const int r = 24;

    size_t h = seed ^ len;

    const unsigned char *data = (const unsigned char *)key;
    size_t k = 0;

    while (len >= 4)
    {
        k = data[0];
        k |= data[1] << 8;
        k |= data[2] << 16;
        k |= data[3] << 24;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    switch (len)
    {
    case 3:
        h ^= data[2] << 16;
    case 2:
        h ^= data[1] << 8;
    case 1:
        h ^= data[0];
        h *= m;
    };

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}

static uint32_t key_function(const char *key, size_t len)
{
    const uint32_t m = 0x5bd1e995;
    const uint32_t seed = 0;
    const int r = 24;

    uint32_t h = seed ^ len;

    const unsigned char *data = (const unsigned char *)key;
    uint32_t k = 0;

    while (len >= 4)
    {
        k = data[0];
        k |= data[1] << 8;
        k |= data[2] << 16;
        k |= data[3] << 24;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    switch (len)
    {
    case 3:
        h ^= data[2] << 16;
    case 2:
        h ^= data[1] << 8;
    case 1:
        h ^= data[0];
        h *= m;
    };

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}

ae_map create_ae_map(size_t data_size, size_t (*func)(const char *, size_t))
{
    ae_map new_map = {
        .occupancy = 0,
        .data_size = data_size,
        .element_size = uint8_t_size + uint32_t_size + data_size};

    if (func == NULL)
        new_map.hash_func = &hash_function;
    else
        new_map.hash_func = func;

    new_map.data = create_max_size_ae_base(&new_map.element_size, AETHER_MAP_ADD_SIZE);

    return new_map;
}

void free_ae_map(ae_map *const map)
{
    free_ae_base(&map->data);

    return;
}

static void resize_ae_map(ae_map *const map)
{
    ae_base new_base = create_max_size_ae_base(&map->element_size, 4 * map->data.max_quant);

    for (size_t i = 0; i < map->occupancy; i++)
    {
        void *kv = get_ae_base(&map->data, &map->element_size, i);
        if (STATUS_AE(kv) != USED_AE)
            continue;
        uint32_t h_key = KEY_AE(kv);
        void *par = P_DATA_AE(kv);
        size_t index = map->hash_func(P_KEY_AE(kv), uint32_t_size) % new_base.max_quant;
        size_t offset = 1;
        while (true)
        {
            kv = get_ae_base(&new_base, &map->element_size, index);
            if (STATUS_AE(kv) != USED_AE)
            {
                memset(P_STATUS_AE(kv), USED_AE, uint8_t_size);
                memcpy(P_KEY_AE(kv), &h_key, uint32_t_size);
                memcpy(P_DATA_AE(kv), par, map->data_size);
                break;
            }
            else
            {
                index = (index + offset) % new_base.max_quant;
                offset++;
            }
        }
    }
    free_ae_base(&map->data);
    map->data = new_base;

    return;
}

void *set_ae_map(ae_map *map, const char *key, size_t key_l)
{
    if (map->occupancy >= map->data.max_quant)
        resize_ae_map(map);

    uint32_t h_key = key_function(key, key_l);
    size_t index = map->hash_func((const char *)&h_key, uint32_t_size) % map->data.max_quant;
    size_t i = index;
    size_t offset = 1;
    while (true)
    {
        void *kv = get_ae_base(&map->data, &map->element_size, i);
        if (STATUS_AE(kv) == USED_AE && KEY_AE(kv) == h_key)
        {
            return P_DATA_AE(kv);
        }
        else
        {
            if (STATUS_AE(kv) != USED_AE)
            {
                memset(P_STATUS_AE(kv), USED_AE, uint8_t_size);
                memcpy(P_KEY_AE(kv), &h_key, uint32_t_size);
                map->occupancy++;
                return P_DATA_AE(kv);
            }
            else
            {
                i = (i + offset) % map->data.max_quant;
                offset++;
                assert(i != index);
            }
        }
    }

    return NULL;
}

void *get_ae_map(const ae_map *const map, const char *key, size_t key_l)
{
    uint32_t h_key = key_function(key, key_l);
    size_t index = map->hash_func((const char *)&h_key, uint32_t_size) % map->data.max_quant;
    size_t i = index;
    size_t offset = 1;
    while (true)
    {
        void *kv = get_ae_base(&map->data, &map->element_size, i);
        assert(STATUS_AE(kv) != EMPTY_AE);
        if (STATUS_AE(kv) == USED_AE)
            if (KEY_AE(kv) == h_key)
                return P_DATA_AE(kv);
        i = (i + offset) % map->data.max_quant;
        offset++;
        assert(i != index);
    }
}

void delete_ae_map(ae_map *const map, const char *key, size_t key_l, void *const par)
{
    uint32_t h_key = key_function(key, key_l);
    size_t index = map->hash_func((const char *)&h_key, uint32_t_size) % map->data.max_quant;
    size_t i = index;
    size_t offset = 1;
    while (true)
    {
        void *kv = get_ae_base(&map->data, &map->element_size, i);
        assert(STATUS_AE(kv) != EMPTY_AE);
        if (STATUS_AE(kv) == USED_AE)
            if (KEY_AE(kv) == h_key)
            {
                memset(P_STATUS_AE(kv), DELETED_AE, map->data_size);
                memcpy(par, P_DATA_AE(kv), map->data_size);
                map->occupancy--;
                return;
            }
        i = (i + offset) % map->data.max_quant;
        offset++;
        assert(i != index);
    }

    return;
}

bool has_key_ae_map(const ae_map *const map, const char *key, size_t key_l)
{
    uint32_t h_key = key_function(key, key_l);
    size_t index = map->hash_func((const char *)&h_key, uint32_t_size) % map->data.max_quant;
    size_t i = index;
    size_t offset = 1;
    while (true)
    {
        void *kv = get_ae_base(&map->data, &map->element_size, i);
        if (STATUS_AE(kv) == USED_AE)
        {
            if (KEY_AE(kv) == h_key)
                return true;
        }
        else if (STATUS_AE(kv) == EMPTY_AE)
            return false;
        i = (i + offset) % map->data.max_quant;
        offset++;
        if (i == index)
            return false;
    }
}
