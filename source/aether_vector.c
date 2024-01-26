#include "aether_vector.h"

ae_vector create_ae_vector(size_t data_size, size_t quant)
{
    ae_vector new_vector = {
        .data = init_ae_base(),
        .data_size = data_size};

    prepare_ae_vector(&new_vector, quant);

    return new_vector;
}

void prepare_ae_vector(ae_vector *const vector, size_t quant)
{
    create_ae_base(&vector->data, &vector->data_size, quant);

    return;
}

uint8_t free_ae_vector(ae_vector *const vector)
{
    free_ae_base(&vector->data);
    vector->data.memory = NULL;
    vector->data_size = 0;

    return 0;
}

uint8_t resize_ae_vector(ae_vector *const vector, size_t new_size)
{
    if (resize_ae_base(&vector->data, &vector->data_size, new_size) != 0)
        return 1;

    return 0;
}

uint8_t append_ae_vector(ae_vector *const vector, void *par)
{
    if (append_ae_base(&vector->data, &vector->data_size, par) != 0)
        return 1;

    return 0;
}

uint8_t append_vector_ae_vector(ae_vector *const vector_to, ae_vector *const vector_from, size_t i, size_t n)
{
    if (append_base_ae_base(&vector_to->data, &vector_from->data, &vector_to->data_size, i, n) != 0)
        return 1;

    return 0;
}

uint8_t set_ae_vector(ae_vector *const vector, size_t i, void *par)
{
    if (set_ae_base(&vector->data, &vector->data_size, i, par) != 0)
        return 1;

    return 0;
}

uint8_t get_ae_vector(const ae_vector *const vector, size_t i, void *par)
{
    if (get_ae_base(&vector->data, &vector->data_size, i, par) != 0)
        return 1;

    return 0;
}

uint8_t get_pointer_ae_vector(const ae_vector *const vector, size_t i, void **par)
{
    if (get_pointer_ae_base(&vector->data, &vector->data_size, i, par) != 0)
        return 1;

    return 0;
}

uint8_t concat_ae_vector(ae_vector *const vector_to, ae_vector *const vector_from)
{
    if (vector_to->data_size != vector_from->data_size)
        return 1;

    if (concat_ae_base(&vector_to->data, &vector_from->data, &vector_to->data_size) != 0)
        return 2;

    return 0;
}

uint8_t insert_ae_vector(ae_vector *const vector, size_t i, void *par)
{
    if (insert_ae_base(&vector->data, &vector->data_size, i, par) != 0)
        return 1;

    return 0;
}

uint8_t insert_vector_ae_vector(ae_vector *const vector_to, ae_vector *const vector_from, size_t i_to, size_t i_from, size_t n)
{
    if (vector_to->data_size != vector_from->data_size)
        return 1;

    if (insert_base_ae_base(&vector_to->data, &vector_from->data, &vector_to->data_size, i_to, i_from, n) != 0)
        return 2;

    return 0;
}

uint8_t delete_ae_vector(ae_vector *const vector, size_t i, void *par)
{
    if (delete_ae_base(&vector->data, &vector->data_size, i, par) != 0)
        return 1;

    return 0;
}

uint8_t delete_vector_ae_vector(ae_vector *const vector, size_t i, size_t n, ae_vector *const par)
{
    if (par != NULL)
        if (vector->data_size != par->data_size)
            return 1;
    if (delete_base_ae_base(&vector->data, &vector->data_size, i, n, &par->data) != 0)
        return 2;

    return 0;
}

uint8_t pop_ae_vector(ae_vector *const vector, void *par)
{
    if (pop_ae_base(&vector->data, &vector->data_size, par) != 0)
        return 1;

    return 0;
}

uint8_t pop_vector_ae_vector(ae_vector *const vector, size_t n, ae_vector *const par)
{
    if (par != NULL)
        if (vector->data_size != par->data_size)
            return 1;
    if (pop_base_ae_base(&vector->data, &vector->data_size, n, &par->data) != 0)
        return 2;

    return 0;
}

uint8_t duplicate_ae_vector(ae_vector *const vector_out, const ae_vector *const vector_in)
{
    if (duplicate_ae_base(&vector_out->data, &vector_in->data, &vector_in->data_size) != 0)
        return 1;

    vector_out->data_size = vector_in->data_size;

    return 0;
}

uint8_t invert_ae_vector(ae_vector *const vector)
{
    if (invert_ae_base(&vector->data, &vector->data_size) != 0)
        return 1;

    return 0;
}

uint8_t find_ae_vector(const ae_vector *const vector, void *par, size_t *ret)
{
    if (find_ae_base(&vector->data, &vector->data_size, par, ret) != 0)
        return 1;

    return 0;
}

uint8_t sort_ae_vector(ae_vector *const vector, int (*comparator)(const void *, const void *))
{
    if (sort_ae_base(&vector->data, &vector->data_size, comparator) != 0)
        return 1;

    return 0;
}

uint8_t swap_ae_vector(ae_vector *const vector, size_t i, size_t j)
{
    if (swap_ae_base(&vector->data, &vector->data_size, i, j) != 0)
        return 1;

    return 0;
}
