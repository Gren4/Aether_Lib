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

ae_map *f_create_ae_map(size_t value_size, uint8_t *status)
{
    if (status != NULL)
        *status = AETHER_MAP_ERROR;

    ae_map *new_map = malloc(sizeof(ae_map));
    if (new_map == NULL)
        return NULL;

    new_map->io_buffer = malloc(value_size);
    if (new_map->io_buffer == NULL)
    {
        free(new_map);
        return NULL;
    }

    uint8_t temp_status;
    new_map->vector = create_ae_vector_debug(ae_key_val, &temp_status);
    resize_ae_vector_debug(new_map->vector, AETHER_MAP_ADD_SIZE, &temp_status);
    if (temp_status == AETHER_MAP_ERROR)
    {
        free_ae_vector(new_map->vector);
        free(new_map);
        return NULL;
    }

    new_map->value_size = value_size;
    new_map->occupancy = 0;

    if (status != NULL)
        *status = AETHER_MAP_SUCCESS;
    return new_map;
}

void *f_free_ae_map(ae_map *map, uint8_t *status)
{
    if (status != NULL)
        *status = AETHER_MAP_ERROR;

    if (map != NULL)
    {
        if (map->vector != NULL)
        {
            if (map->vector->data != NULL)
            {
                size_t i = 0;

                for (i = 0; i < size_ae_vector(map->vector); i++)
                {
                    ae_key_val *par = &get_ae_vector(map->vector, i, ae_key_val);

                    if (par->key != NULL)
                        free(par->key);

                    if (par->value != NULL)
                        free(par->value);
                }
            }

            free_ae_vector(map->vector);
        }

        if (map->io_buffer != NULL)
            free(map->io_buffer);

        free(map);
    }

    if (status != NULL)
        *status = AETHER_MAP_SUCCESS;
    return NULL;
}

static void f_create_ae_key_val(ae_key_val *par, uint8_t *str, size_t *value_size, uint8_t *status)
{
    if (status != NULL)
        *status = AETHER_MAP_ERROR;

    size_t str_l = strlen(str) + 1;
    par->key = calloc(str_l, sizeof(uint8_t));
    par->value = malloc(*value_size);

    if (par->key == NULL || par->value == NULL)
        return;

    if (status != NULL)
        *status = AETHER_MAP_SUCCESS;
    memmove(par->key, str, str_l * sizeof(uint8_t));
    return;
}

static void f_resize_ae_map(ae_map *map, uint8_t *status)
{
    if (status != NULL)
        *status = AETHER_MAP_ERROR;

    size_t new_size = 2 * map->vector->quantity;
    uint8_t temp_status;
    ae_vector *resized_vector = create_ae_vector_debug(ae_key_val, &temp_status);
    resize_ae_vector_debug(resized_vector, new_size, &temp_status);

    if (temp_status == AETHER_VECTOR_ERROR)
        return;

    size_t old_occupancy = map->occupancy;
    ae_vector *old_vector = map->vector;
    map->occupancy = 0;
    map->vector = resized_vector;
    size_t i = 0;

    for (i = 0; i < size_ae_vector(old_vector); i++)
    {
        ae_key_val *par = &get_ae_vector(old_vector, i, ae_key_val);
        if (par->key != NULL)
        {
            memmove(map->io_buffer, par->value, map->value_size);
            f_set_ae_map(map, par->key, &temp_status);
            if (temp_status == AETHER_MAP_ERROR)
            {
                free_ae_vector(resized_vector);
                map->occupancy = old_occupancy;
                map->vector = old_vector;
                return;
            }
        }
    }

    free_ae_vector(old_vector);

    if (status != NULL)
        *status = AETHER_MAP_SUCCESS;
    return;
}

void f_set_ae_map(ae_map *map, uint8_t *str, uint8_t *status)
{
    if (status != NULL)
        *status = AETHER_MAP_ERROR;

    if (map == NULL)
        return;

    if (map->occupancy >= map->vector->quantity)
        return;

    size_t index = f_hash_function(str) % size_ae_vector(map->vector);

    while (1)
    {
        uint8_t temp_status;
        ae_key_val *par = &get_ae_vector_debug(map->vector, index, ae_key_val, &temp_status);

        if (temp_status == AETHER_VECTOR_ERROR)
            return;

        if (par->key == NULL)
        {
            f_create_ae_key_val(par, str, &map->value_size, status);

            memmove(par->value, map->io_buffer, map->value_size);

            map->occupancy++;

            if (map->occupancy >= map->vector->quantity)
            {
                uint8_t temp_status;
                f_resize_ae_map(map, &temp_status);

                if (temp_status == AETHER_MAP_ERROR)
                    return;
            }

            if (status != NULL)
                *status = AETHER_MAP_SUCCESS;
            return;
        }
        else
        {
            if ((strlen(par->key) == strlen(str)) && memcmp(str, par->key, strlen(str) * sizeof(uint8_t)) == 0)
            {
                memmove(par->value, map->io_buffer, map->value_size);

                if (status != NULL)
                    *status = AETHER_MAP_SUCCESS;
                return;
            }
            else
            {
                index = (index + 1) % size_ae_vector(map->vector);
            }
        }
    }
}

size_t f_find_key_ae_map(ae_map *map, uint8_t *str, uint8_t *status)
{
    if (status != NULL)
        *status = AETHER_MAP_ERROR;

    if (map == NULL)
        return map->vector->quantity;

    size_t orig_index = f_hash_function(str) % size_ae_vector(map->vector);
    size_t index = orig_index;

    uint8_t temp_status;
    while (1)
    {
        ae_key_val *par = &get_ae_vector_debug(map->vector, index, ae_key_val, &temp_status);

        if (temp_status == AETHER_VECTOR_ERROR)
            return map->vector->quantity;

        if (par->key == NULL)
            return map->vector->quantity;

        size_t element_index = f_hash_function(par->key) % size_ae_vector(map->vector);
        if (element_index != orig_index)
            return map->vector->quantity;

        if ((strlen(par->key) == strlen(str)) && memcmp(str, par->key, strlen(str) * sizeof(uint8_t)) == 0)
        {
            if (status != NULL)
                *status = AETHER_MAP_SUCCESS;
            return index;
        }
        else
        {
            index = (index + 1) % size_ae_vector(map->vector);
        }
    }
}

void *f_get_ae_map(ae_map *map, uint8_t *str, uint8_t *status)
{
    if (status != NULL)
        *status = AETHER_MAP_ERROR;

    if (map == NULL)
        return NULL;

    uint8_t temp_status;
    size_t index = f_find_key_ae_map(map, str, &temp_status);

    if (temp_status == AETHER_MAP_ERROR)
        return NULL;

    if (index == map->vector->quantity)
        return NULL;

    ae_key_val *par = &get_ae_vector_debug(map->vector, index, ae_key_val, &temp_status);

    if (temp_status == AETHER_VECTOR_ERROR)
        return NULL;

    return par->value;
}

void *f_delete_ae_map(ae_map *map, uint8_t *str, uint8_t *status)
{
    if (status != NULL)
        *status = AETHER_MAP_ERROR;

    if (map == NULL)
        return NULL;

    uint8_t temp_status;
    size_t index = f_find_key_ae_map(map, str, &temp_status);

    if (temp_status == AETHER_MAP_ERROR)
        return NULL;

    if (index == map->vector->quantity)
        return NULL;

    ae_key_val *par = &get_ae_vector_debug(map->vector, index, ae_key_val, &temp_status);

    if (temp_status == AETHER_VECTOR_ERROR)
        return NULL;

    memmove(map->io_buffer, par->value, map->value_size);

    free(par->key);
    par->key = NULL;

    free(par->value);
    par->value = NULL;

    map->occupancy--;

    return map->io_buffer;
}

ae_vector *f_get_keys_ae_map(ae_map *map, uint8_t *status)
{
    if (status != NULL)
        *status = AETHER_MAP_ERROR;

    if (map == NULL)
        return NULL;

    if (map->vector == NULL)
        return NULL;

    uint8_t temp_status;
    ae_vector *keys_vector = create_ae_vector_debug(uint8_t *, &temp_status);

    if (temp_status == AETHER_VECTOR_ERROR)
        return NULL;

    size_t i = 0;
    for (i = 0; i < size_ae_vector(map->vector); i++)
    {
        ae_key_val *par = &get_ae_vector_debug(map->vector, i, ae_key_val, &temp_status);

        if (temp_status == AETHER_VECTOR_ERROR)
            return NULL;

        if (par->key != NULL)
        {
            append_ae_vector_debug(keys_vector, uint8_t *, par->key, &temp_status);

            if (temp_status == AETHER_VECTOR_ERROR)
                return NULL;
        }
    }

    return keys_vector;
}