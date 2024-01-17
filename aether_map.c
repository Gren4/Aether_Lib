#include "aether_map.h"

size_t f_hash_function(uint8_t *str)
{
    /* djb2 */
    size_t hash = 5381;

    while (*str)
    {
        hash = ((hash << 5) + hash) + *str;
        *str++;
    }

    return hash;
}

ae_map f_init_ae_map(void)
{
    ae_map new_map = {
        .data = f_init_ae_base,
        .data_size = 0,
        .hash_func = &f_hash_function,
        .max_size = 0,
        .occupancy = 0};

    return new_map;
}

uint8_t f_create_ae_map(ae_map *map, size_t data_size, size_t (*func)(uint8_t *))
{
    if (map == NULL)
        return 1;

    if (f_create_ae_base(&map->data, sizeof(ae_base), AETHER_MAP_ADD_SIZE) != 0)
        return 2;

    size_t i = 0;
    for (i = 0; i < AETHER_MAP_ADD_SIZE; i++)
    {
        ae_base bucket;
        if (f_create_ae_base(&bucket, sizeof(ae_key_val), AETHER_MAP_ADD_SIZE) != 0)
            return 3;

        if (f_append_ae_base(&map->data, sizeof(ae_base), AETHER_BUCKET_ADD_SIZE, &bucket) != 0)
            return 4;
    }

    map->data_size = data_size;
    map->occupancy = 0;
    map->max_size = AETHER_MAP_ADD_SIZE;
    if (func == NULL)
        map->hash_func = &f_hash_function;
    else
        map->hash_func = func;

    return 0;
}

uint8_t f_free_ae_map(ae_map *map)
{
    if (map == NULL)
        return 0;
    if (map->data.memory != NULL)
    {
        size_t i, j;

        for (i = 0; i < map->data.quant; i++)
        {
            ae_base *bucket;
            if (f_get_pointer_ae_base(&map->data, sizeof(ae_base), i, (void **)(&bucket)) != 0)
                return 1;
            for (j = 0; j < bucket->quant; j++)
            {
                ae_key_val kv;
                if (f_get_ae_base(bucket, sizeof(ae_key_val), j, &kv) != 0)
                    return 2;
                if (kv.key != NULL)
                {
#ifdef DEBUG_AE
                    printf("Freed %x\n", kv.key);
#endif
                    free(kv.key);
                }
                if (kv.value != NULL)
                {
#ifdef DEBUG_AE
                    printf("Freed %x\n", kv.value);
#endif
                    free(kv.value);
                }
            }
            f_free_ae_base(bucket);
        }
        f_free_ae_base(&map->data);
    }

    return 0;
}

uint8_t f_create_ae_key_val(ae_key_val *kv, uint8_t *key, void *par, size_t data_size)
{
    if (kv == NULL)
        return 1;
    size_t str_l = strlen(key) + 1;

    kv->key = calloc(str_l, sizeof(uint8_t));
#ifdef DEBUG_AE
    printf("Allocated %x\n", kv->key);
#endif
    if (kv->key == NULL)
        return 2;

    kv->value = malloc(data_size);
#ifdef DEBUG_AE
    printf("Allocated %x\n", kv->value);
#endif
    if (kv->value == NULL)
    {
#ifdef DEBUG_AE
        printf("Freed %x\n", kv->key);
#endif
        free(kv->key);
        return 3;
    }

    memmove(kv->key, key, str_l * sizeof(uint8_t));
    memmove(kv->value, par, data_size);

    return 0;
}

uint8_t f_resize_ae_map(ae_map *map)
{
    size_t i = 0;
    size_t j = 0;

    size_t new_size = 2 * map->max_size;
    size_t old_size = map->max_size;
    for (i = map->max_size; i < new_size; i++)
    {
        ae_base bucket;
        if (f_create_ae_base(&bucket, sizeof(ae_key_val), AETHER_MAP_ADD_SIZE) != 0)
            return 1;

        if (f_append_ae_base(&map->data, sizeof(ae_base), AETHER_BUCKET_ADD_SIZE, &bucket) != 0)
            return 2;
    }

    for (i = 0; i < old_size; i++)
    {
        ae_base *bucket;
        if (f_get_pointer_ae_base(&map->data, sizeof(ae_base), i, (void **)(&bucket)) != 0)
            return 3;

        j = 0;
        ae_key_val kv;
        while (j < bucket->quant)
        {
            if (f_get_ae_base(bucket, sizeof(ae_key_val), j, &kv) != 0)
                return 4;
            if (kv.key != NULL)
            {
                size_t index = map->hash_func(kv.key) % new_size;
                if (index == i)
                {
                    j++;
                }
                else
                {
                    if (f_delete_ae_base(bucket, sizeof(ae_key_val), AETHER_BUCKET_ADD_SIZE, j, &kv) != 0)
                        return 5;
                    ae_base *bucket_to;
                    if (f_get_pointer_ae_base(&map->data, sizeof(ae_base), index, (void **)(&bucket)) != 0)
                        return 6;
                    if (f_append_ae_base(bucket_to, sizeof(ae_key_val), AETHER_BUCKET_ADD_SIZE, &kv) != 0)
                        return 7;
                }
            }
        }
    }

    map->max_size = new_size;

    return 0;
}

uint8_t f_set_ae_map(ae_map *map, uint8_t *key, void *par)
{
    if (map == NULL)
        return 1;

    if (map->occupancy >= map->max_size)
        if (f_resize_ae_map(map) != 0)
            return 2;

    ae_return_key_val res = {0, 0, false};
    if (f_find_key_ae_map(map, key, &res) != 0)
        return 3;

    ae_base *bucket;
    if (f_get_pointer_ae_base(&map->data, sizeof(ae_base), res.index, (void **)(&bucket)) != 0)
        return 4;

    ae_key_val kv;

    if (res.result == true)
    {
        if (f_get_ae_base(bucket, sizeof(ae_key_val), res.bucket_index, &kv) != 0)
            return 5;

        memmove(kv.value, par, map->data_size);
    }
    else
    {
        if (f_create_ae_key_val(&kv, key, par, map->data_size) != 0)
            return 6;
        if (f_append_ae_base(bucket, sizeof(ae_key_val), AETHER_BUCKET_ADD_SIZE, &kv) != 0)
            return 7;
        map->occupancy++;
    }
    return 0;
}

uint8_t f_find_key_ae_map(ae_map *map, uint8_t *key, ae_return_key_val *res)
{
    res->result = false;

    if (map == NULL)
        return 1;
    if (map->data.memory == NULL)
        return 2;

    res->index = map->hash_func(key) % map->max_size;

    ae_base *bucket;
    if (f_get_pointer_ae_base(&map->data, sizeof(ae_base), res->index, (void **)(&bucket)) != 0)
        return 3;

    ae_key_val kv;

    res->bucket_index = 0;

    for (res->bucket_index = 0; res->bucket_index < bucket->quant; res->bucket_index++)
    {
        if (f_get_ae_base(bucket, sizeof(ae_key_val), res->bucket_index, &kv) != 0)
            return 4;

        if ((strlen(kv.key) == strlen(key)) && memcmp(key, kv.key, strlen(key) * sizeof(uint8_t)) == 0)
        {
            res->result = true;
            return 0;
        }
    }

    return 0;
}

bool f_has_key_ae_map(ae_map *map, uint8_t *key)
{
    ae_return_key_val ret_val = {0, 0, false};

    if (f_find_key_ae_map(map, key, &ret_val) != 0)
        return false;

    return ret_val.result;
}

uint8_t f_get_ae_map(ae_map *map, uint8_t *key, void *par)
{
    if (map == NULL)
        return 1;

    ae_return_key_val res = {0, 0, false};
    if (f_find_key_ae_map(map, key, &res) != 0)
        return 2;

    if (res.result == true)
    {
        ae_base *bucket;
        if (f_get_pointer_ae_base(&map->data, sizeof(ae_base), res.index, (void **)(&bucket)) != 0)
            return 3;

        ae_key_val kv;
        if (f_get_ae_base(bucket, sizeof(ae_key_val), res.bucket_index, &kv) != 0)
            return 4;

        memmove(par, kv.value, map->data_size);

        return 0;
    }
    else
    {
        return 5;
    }
}

uint8_t f_delete_ae_map(ae_map *map, uint8_t *key, void *par)
{
    if (map == NULL)
        return 1;

    ae_return_key_val res = {0, 0, false};
    if (f_find_key_ae_map(map, key, &res) != 0)
        return 2;

    if (res.result == true)
    {
        ae_base *bucket;
        if (f_get_pointer_ae_base(&map->data, sizeof(ae_base), res.index, (void **)(&bucket)) != 0)
            return 3;

        ae_key_val kv;
        if (f_delete_ae_base(bucket, sizeof(ae_key_val), AETHER_BUCKET_ADD_SIZE, res.bucket_index, &kv) != 0)
            return 4;
        if (par != NULL)
            memmove(par, kv.value, map->data_size);

#ifdef DEBUG_AE
        printf("Freed %x\n", kv.key);
#endif
        free(kv.key);
#ifdef DEBUG_AE
        printf("Freed %x\n", kv.value);
#endif
        free(kv.value);
        map->occupancy--;
    }

    return 0;
}

ae_vector f_get_keys_ae_map(ae_map *map)
{
    ae_vector vector;

    if (f_create_ae_vector(&vector, sizeof(uint8_t *)) != 0)
        return f_init_ae_vector();

    if (map == NULL)
        return f_init_ae_vector();

    if (map->data.memory == NULL)
        return f_init_ae_vector();

    size_t i, j;
    for (i = 0; i < map->data.quant; i++)
    {
        ae_base *bucket;
        if (f_get_pointer_ae_base(&map->data, sizeof(ae_base), i, (void **)(&bucket)) != 0)
            return f_init_ae_vector();
        ae_key_val kv;
        for (j = 0; j < bucket->quant; j++)
        {
            if (f_get_ae_base(bucket, sizeof(ae_key_val), j, &kv) != 0)
                return f_init_ae_vector();
            if (kv.key != NULL)
            {
                if (f_append_ae_vector(&vector, &kv.key) != 0)
                    return f_init_ae_vector();
            }
        }
    }

    return vector;
}