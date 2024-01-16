#include "aether_vector.h"

ae_vector f_init_ae_vector(void)
{
    ae_vector new_vector = {
        .data = f_init_ae_base(),
        .data_size = 0 
    };

    return new_vector;
}

uint8_t f_create_ae_vector(ae_vector *vector, size_t data_size)
{
    if (vector == NULL)
        return 1;

    if (f_create_ae_base(&vector->data, data_size, AETHER_VECTOR_ADD_SIZE) != 0)
        return 2;

    vector->data_size = data_size;

    return 0;
}

uint8_t f_free_ae_vector(ae_vector *vector)
{
    f_free_ae_base(&vector->data);
    vector->data.memory = NULL;
    vector->data_size = 0;

    return 0;
}

uint8_t f_resize_ae_vector(ae_vector *vector, size_t new_size)
{
    if (vector == NULL)
        return 1;
    if (f_resize_ae_base(&vector->data, vector->data_size, AETHER_VECTOR_ADD_SIZE, new_size) != 0)
        return 2;

    return 0;
}

uint8_t f_append_ae_vector(ae_vector *vector, void *par)
{
    if (vector == NULL)
        return 1;
    if (f_append_ae_base(&vector->data, vector->data_size, AETHER_VECTOR_ADD_SIZE, par) != 0)
        return 2;

    return 0;
}

uint8_t f_set_ae_vector(ae_vector *vector, size_t i, void *par)
{
    if (vector == NULL)
        return 1;
    if (f_set_ae_base(&vector->data, vector->data_size, i, par) != 0)
        return 2;

    return 0;
}

uint8_t f_get_ae_vector(ae_vector *vector, size_t i, void *par)
{
    if (vector == NULL)
        return 1;
    if (f_get_ae_base(&vector->data, vector->data_size, i, par) != 0)
        return 2;

    return 0;
}

uint8_t f_concat_ae_vector(ae_vector *vector_to, ae_vector *vector_from)
{
    if (vector_to == NULL)
        return 1;
    if (vector_from == NULL)
        return 2;
    if (vector_to->data_size != vector_from->data_size)
        return 3;
    if (f_concat_ae_base(&vector_to->data, &vector_from->data, vector_to->data_size, AETHER_VECTOR_ADD_SIZE) != 0)
        return 4;

    return 0;
}

uint8_t f_insert_ae_vector(ae_vector *vector, size_t i, void *par)
{
    if (vector == NULL)
        return 1;
    if (f_insert_ae_base(&vector->data, vector->data_size, AETHER_VECTOR_ADD_SIZE, i, par) != 0)
        return 2;

    return 0;
}

uint8_t f_delete_ae_vector(ae_vector *vector, size_t i, void *par)
{
    if (vector == NULL)
        return 1;
    if (f_delete_ae_base(&vector->data, vector->data_size, AETHER_VECTOR_ADD_SIZE, i, par) != 0)
        return 2;

    return 0;
}

uint8_t f_duplicate_ae_vector(ae_vector *vector_out, ae_vector *vector_in)
{
    if (vector_out == NULL)
        return 1;
    if (vector_in == NULL)
        return 2;

    if (f_duplicate_ae_base(&vector_out->data, &vector_in->data, vector_in->data_size, AETHER_VECTOR_ADD_SIZE) != 0)
        return 3;
    
    vector_out->data_size = vector_in->data_size;

    return 0;
}

uint8_t f_invert_ae_vector(ae_vector *vector)
{
    if (vector == NULL)
        return 1;
    if (f_invert_ae_base(&vector->data, vector->data_size) != 0)
        return 2;

    return 0;
}

uint8_t f_find_ae_vector(ae_vector *vector, void *par, size_t *ret)
{
    if (vector == NULL)
        return 1;
    if (f_find_ae_base(&vector->data, vector->data_size, par, ret) != 0)
        return 2;

    return 0;
}