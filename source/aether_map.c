#include "aether_map.h"

size_t hash_function(const char *key)
{
    /* djb2 */
    size_t hash = 5381;
    size_t c;

    while ((c = *key++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}

ae_map create_ae_map(size_t data_size, size_t (*func)(const char *))
{
    ae_map new_map = {
        .data = init_ae_base(),
        .data_size = data_size,
        .max_size = AETHER_MAP_ADD_SIZE,
        .occupancy = 0};

    if (func == NULL)
        new_map.hash_func = &hash_function;
    else
        new_map.hash_func = func;

    prepare_ae_map(&new_map, func);

    return new_map;
}

void prepare_ae_map(ae_map *map, size_t (*func)(const char *))
{

    if (map == NULL)
        return;

    if (create_ae_base(&map->data, sizeof(ae_base), AETHER_MAP_ADD_SIZE) != 0)
        return;

    size_t i = 0;
    for (i = 0; i < AETHER_MAP_ADD_SIZE; i++)
    {
        ae_base bucket;
        if (create_ae_base(&bucket, sizeof(ae_key_val), AETHER_MAP_ADD_SIZE) != 0)
            return;

        if (append_ae_base(&map->data, sizeof(ae_base), AETHER_BUCKET_ADD_SIZE, &bucket) != 0)
            return;
    }

    return;
}

uint8_t free_ae_map(ae_map *map)
{
    if (map == NULL)
        return 0;
    if (map->data.memory != NULL)
    {
        size_t i, j;

        for (i = 0; i < map->data.quant; i++)
        {
            ae_base *bucket;
            if (get_pointer_ae_base(&map->data, sizeof(ae_base), i, (void **)(&bucket)) != 0)
                return 1;
            for (j = 0; j < bucket->quant; j++)
            {
                ae_key_val kv;
                if (get_ae_base(bucket, sizeof(ae_key_val), j, &kv) != 0)
                    return 2;
                if (kv.key != NULL)
                {
#ifdef DEBUG_AE
                    printf("Freed memory at address %p\n", (void *)kv.key);
#endif
                    free(kv.key);
                }
                if (kv.value != NULL)
                {
#ifdef DEBUG_AE
                    printf("Freed memory at address %p\n", kv.value);
#endif
                    free(kv.value);
                }
            }
            free_ae_base(bucket);
        }
        free_ae_base(&map->data);
    }

    return 0;
}

uint8_t create_ae_key_val(ae_key_val *kv, const char *key, void *par, size_t data_size)
{

    if (kv == NULL)
        return 1;

    size_t str_l = strlen(key) + 1;

    kv->key = calloc(str_l, sizeof(uint8_t));

    if (kv->key == NULL)
        return 2;

#ifdef DEBUG_AE
    printf("Allocated %p\n", (void *)kv->key);
#endif

    kv->value = malloc(data_size);

    if (kv->value == NULL)
    {
#ifdef DEBUG_AE
        printf("Freed memory at address %p\n", (void *)kv->key);
#endif
        free(kv->key);
        return 3;
    }

#ifdef DEBUG_AE
    printf("Allocated %p\n", kv->value);
#endif

    memmove(kv->key, key, str_l * sizeof(uint8_t));
    memmove(kv->value, par, data_size);

    return 0;
}

uint8_t resize_ae_map(ae_map *map)
{
    size_t i = 0;
    size_t j = 0;

    size_t new_size = 2 * map->max_size;
    size_t old_size = map->max_size;
    for (i = map->max_size; i < new_size; i++)
    {
        ae_base bucket;
        if (create_ae_base(&bucket, sizeof(ae_key_val), AETHER_MAP_ADD_SIZE) != 0)
            return 1;

        if (append_ae_base(&map->data, sizeof(ae_base), AETHER_BUCKET_ADD_SIZE, &bucket) != 0)
            return 2;
    }

    for (i = 0; i < old_size; i++)
    {
        ae_base *bucket = NULL;
        if (get_pointer_ae_base(&map->data, sizeof(ae_base), i, (void **)(&bucket)) != 0)
            return 3;

        j = 0;
        ae_key_val kv;
        while (j < bucket->quant)
        {
            if (get_ae_base(bucket, sizeof(ae_key_val), j, &kv) != 0)
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
                    if (delete_ae_base(bucket, sizeof(ae_key_val), AETHER_BUCKET_ADD_SIZE, j, &kv) != 0)
                        return 5;
                    ae_base *bucket_to = NULL;
                    if (get_pointer_ae_base(&map->data, sizeof(ae_base), index, (void **)(&bucket)) != 0)
                        return 6;
                    if (append_ae_base(bucket_to, sizeof(ae_key_val), AETHER_BUCKET_ADD_SIZE, &kv) != 0)
                        return 7;
                }
            }
        }
    }

    map->max_size = new_size;

    return 0;
}

uint8_t set_ae_map(ae_map *map, const char *key, void *par)
{

    if (map == NULL)
        return 1;

    if (map->occupancy >= map->max_size)
        if (resize_ae_map(map) != 0)
            return 2;

    ae_return_key_val res = {0, 0, false};
    if (find_key_ae_map(map, key, &res) != 0)
        return 3;

    ae_base *bucket;
    if (get_pointer_ae_base(&map->data, sizeof(ae_base), res.index, (void **)(&bucket)) != 0)
        return 4;

    ae_key_val kv;

    if (res.result == true)
    {
        if (get_ae_base(bucket, sizeof(ae_key_val), res.bucket_index, &kv) != 0)
            return 5;

        memmove(kv.value, par, map->data_size);
    }
    else
    {
        if (create_ae_key_val(&kv, key, par, map->data_size) != 0)
            return 6;
        if (append_ae_base(bucket, sizeof(ae_key_val), AETHER_BUCKET_ADD_SIZE, &kv) != 0)
            return 7;
        map->occupancy++;
    }
    return 0;
}

uint8_t find_key_ae_map(ae_map *map, const char *key, ae_return_key_val *res)
{
    res->result = false;

    if (map == NULL)
        return 1;
    if (map->data.memory == NULL)
        return 2;

    res->index = map->hash_func(key) % map->max_size;

    ae_base *bucket;
    if (get_pointer_ae_base(&map->data, sizeof(ae_base), res->index, (void **)(&bucket)) != 0)
        return 3;

    ae_key_val kv;

    res->bucket_index = 0;

    for (res->bucket_index = 0; res->bucket_index < bucket->quant; res->bucket_index++)
    {
        if (get_ae_base(bucket, sizeof(ae_key_val), res->bucket_index, &kv) != 0)
            return 4;

        if ((strlen(kv.key) == strlen(key)) && memcmp(key, kv.key, strlen(key) * sizeof(uint8_t)) == 0)
        {
            res->result = true;
            return 0;
        }
    }

    return 0;
}

bool has_key_ae_map(ae_map *map, const char *key)
{
    ae_return_key_val ret_val = {0, 0, false};

    if (find_key_ae_map(map, key, &ret_val) != 0)
        return false;

    return ret_val.result;
}

uint8_t get_ae_map(ae_map *map, const char *key, void *par)
{

    if (map == NULL)
        return 1;

    ae_return_key_val res = {0, 0, false};
    if (find_key_ae_map(map, key, &res) != 0)
        return 2;

    if (res.result == true)
    {
        ae_base *bucket;
        if (get_pointer_ae_base(&map->data, sizeof(ae_base), res.index, (void **)(&bucket)) != 0)
            return 3;

        ae_key_val kv;
        if (get_ae_base(bucket, sizeof(ae_key_val), res.bucket_index, &kv) != 0)
            return 4;

        memmove(par, kv.value, map->data_size);

        return 0;
    }
    else
    {
        return 5;
    }
}

uint8_t delete_ae_map(ae_map *map, const char *key, void *par)
{

    if (map == NULL)
        return 1;

    ae_return_key_val res = {0, 0, false};
    if (find_key_ae_map(map, key, &res) != 0)
        return 2;

    if (res.result == true)
    {
        ae_base *bucket;
        if (get_pointer_ae_base(&map->data, sizeof(ae_base), res.index, (void **)(&bucket)) != 0)
            return 3;

        ae_key_val kv;
        if (delete_ae_base(bucket, sizeof(ae_key_val), AETHER_BUCKET_ADD_SIZE, res.bucket_index, &kv) != 0)
            return 4;
        if (par != NULL)
            memmove(par, kv.value, map->data_size);

#ifdef DEBUG_AE
        printf("Freed memory at address %p\n", (void *)kv.key);
#endif
        free(kv.key);
#ifdef DEBUG_AE
        printf("Freed memory at address %p\n", kv.value);
#endif
        free(kv.value);
        map->occupancy--;
    }

    return 0;
}

ae_vector get_keys_ae_map(ae_map *map)
{
    ae_vector vector = create_ae_vector(sizeof(uint8_t *), 0);

    if (map == NULL)
        return vector;

    if (map->data.memory == NULL)
        return vector;

    size_t i, j;
    for (i = 0; i < map->data.quant; i++)
    {
        ae_base *bucket;
        if (get_pointer_ae_base(&map->data, sizeof(ae_base), i, (void **)(&bucket)) != 0)
            return vector;
        ae_key_val kv;
        for (j = 0; j < bucket->quant; j++)
        {
            if (get_ae_base(bucket, sizeof(ae_key_val), j, &kv) != 0)
                return vector;
            if (kv.key != NULL)
            {
                if (append_ae_vector(&vector, &kv.key) != 0)
                    return vector;
            }
        }
    }

    return vector;
}
