#include "aether_vector.h"

ae_vector init_ae_vector(void)
{
    ae_vector new_vector = {
        .data = init_ae_base(),
        .data_size = 0};

    return new_vector;
}

uint8_t create_ae_vector(ae_vector *vector, size_t data_size)
{
    if (vector == NULL)
        return 1;

    if (create_ae_base(&vector->data, data_size, AETHER_VECTOR_ADD_SIZE) != 0)
        return 2;

    vector->data_size = data_size;

    return 0;
}

uint8_t free_ae_vector(ae_vector *vector)
{
    free_ae_base(&vector->data);
    vector->data.memory = NULL;
    vector->data_size = 0;

    return 0;
}

uint8_t resize_ae_vector(ae_vector *vector, size_t new_size)
{
    if (vector == NULL)
        return 1;
    if (resize_ae_base(&vector->data, vector->data_size, AETHER_VECTOR_ADD_SIZE, new_size) != 0)
        return 2;

    return 0;
}

uint8_t append_ae_vector(ae_vector *vector, void *par)
{
    if (vector == NULL)
        return 1;
    if (append_ae_base(&vector->data, vector->data_size, AETHER_VECTOR_ADD_SIZE, par) != 0)
        return 2;

    return 0;
}

uint8_t set_ae_vector(ae_vector *vector, size_t i, void *par)
{
    if (vector == NULL)
        return 1;
    if (set_ae_base(&vector->data, vector->data_size, i, par) != 0)
        return 2;

    return 0;
}

uint8_t get_ae_vector(ae_vector *vector, size_t i, void *par)
{
    if (vector == NULL)
        return 1;
    if (get_ae_base(&vector->data, vector->data_size, i, par) != 0)
        return 2;

    return 0;
}

uint8_t get_pointer_ae_vector(ae_vector *vector, size_t i, void **par)
{
    if (vector == NULL)
        return 1;
    if (get_pointer_ae_base(&vector->data, vector->data_size, i, par) != 0)
        return 2;

    return 0;
}

uint8_t concat_ae_vector(ae_vector *vector_to, ae_vector *vector_from)
{
    if (vector_to == NULL)
        return 1;
    if (vector_from == NULL)
        return 2;
    if (vector_to->data_size != vector_from->data_size)
        return 3;
    if (concat_ae_base(&vector_to->data, &vector_from->data, vector_to->data_size, AETHER_VECTOR_ADD_SIZE) != 0)
        return 4;

    return 0;
}

uint8_t insert_ae_vector(ae_vector *vector, size_t i, void *par)
{
    if (vector == NULL)
        return 1;
    if (insert_ae_base(&vector->data, vector->data_size, AETHER_VECTOR_ADD_SIZE, i, par) != 0)
        return 2;

    return 0;
}

uint8_t delete_ae_vector(ae_vector *vector, size_t i, void *par)
{
    if (vector == NULL)
        return 1;
    if (delete_ae_base(&vector->data, vector->data_size, AETHER_VECTOR_ADD_SIZE, i, par) != 0)
        return 2;

    return 0;
}

uint8_t duplicate_ae_vector(ae_vector *vector_out, ae_vector *vector_in)
{
    if (vector_out == NULL)
        return 1;
    if (vector_in == NULL)
        return 2;

    if (duplicate_ae_base(&vector_out->data, &vector_in->data, vector_in->data_size, AETHER_VECTOR_ADD_SIZE) != 0)
        return 3;

    vector_out->data_size = vector_in->data_size;

    return 0;
}

uint8_t invert_ae_vector(ae_vector *vector)
{
    if (vector == NULL)
        return 1;
    if (invert_ae_base(&vector->data, vector->data_size) != 0)
        return 2;

    return 0;
}

uint8_t find_ae_vector(ae_vector *vector, void *par, size_t *ret)
{
    if (vector == NULL)
        return 1;
    if (find_ae_base(&vector->data, vector->data_size, par, ret) != 0)
        return 2;

    return 0;
}

uint8_t sort_ae_vector(ae_vector *vector, int (*comparator)(const void *, const void *))
{
    if (vector == NULL)
        return 1;
    
    if (sort_ae_base(&vector->data, vector->data_size, comparator) != 0)
        return 2;

    return 0;
}