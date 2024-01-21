#include "aether_vector.h"

ae_vector create_ae_vector(size_t data_size, size_t quant)
{
    if (quant == 0)
        quant = AETHER_VECTOR_ADD_SIZE;
    ae_vector new_vector = {
        .data = init_ae_base(),
        .data_size = data_size};
    prepare_ae_vector(&new_vector, quant);

    return new_vector;
}

void prepare_ae_vector(ae_vector *vector, size_t quant)
{
    if (vector == NULL)
        return;

    if (create_ae_base(&vector->data, vector->data_size, AETHER_VECTOR_ADD_SIZE) != 0)
        return;

    return;
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

uint8_t append_vector_ae_vector(ae_vector *vector_to, ae_vector *vector_from, size_t i, size_t n)
{

    if (vector_to == NULL)
        return 1;
    if (vector_from == NULL)
        return 2;

    if (append_base_ae_base(&vector_to->data, &vector_from->data, vector_to->data_size, AETHER_VECTOR_ADD_SIZE, i, n) != 0)
        return 3;

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

uint8_t insert_vector_ae_vector(ae_vector *vector_to, ae_vector *vector_from, size_t i_to, size_t i_from, size_t n)
{

    if (vector_to == NULL)
        return 1;
    if (vector_from == NULL)
        return 2;
    if (vector_to->data_size != vector_from->data_size)
        return 3;

    if (insert_base_ae_base(&vector_to->data, &vector_from->data, vector_to->data_size, AETHER_VECTOR_ADD_SIZE, i_to, i_from, n) != 0)
        return 4;

    return 0;
}

uint8_t pop_ae_vector(ae_vector *vector, void *par)
{

    if (vector == NULL)
        return 1;

    if (delete_ae_base(&vector->data, vector->data_size, AETHER_VECTOR_ADD_SIZE, vector->data.quant - 1, par) != 0)
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

uint8_t swap_ae_vector(ae_vector *vector, size_t i, size_t j)
{
    if (vector == NULL)
        return 1;

    if (swap_ae_base(&vector->data, vector->data_size, i, j) != 0)
        return 2;

    return 0;
}
