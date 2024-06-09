#include "aether_vector.h"
#include <assert.h>
#include <string.h>

extern inline int32_t gc_idx_ae_vec(ae_vec *const vector);

extern inline size_t quant_ae_vec(ae_vec *const vector);

extern inline size_t max_quant_ae_vec(ae_vec *const vector);

extern inline size_t data_size_ae_vec(ae_vec *const vector);

void create_gc_ae_vec(ae_vec *const gc, size_t data_size)
{
    gc->data_size = data_size;

    create_gc_ae_base(&gc->data, &gc->data_size);

    return;
}

ae_vec create_ae_vec(size_t data_size, size_t quant)
{
    ae_vec new_vector = {
        .data = create_ae_base(&data_size, quant),
        .data_size = data_size};

    return new_vector;
}

void free_ae_vec(ae_vec *const vector)
{
    assert(vector != NULL);
    free_ae_base(&vector->data);
    vector->data_size = 0;

    return;
}

void resize_ae_vec(ae_vec *const vector, size_t new_size)
{
    resize_ae_base(&vector->data, &vector->data_size, new_size);

    return;
}

void *append_ae_vec(ae_vec *const vector)
{
    return append_ae_base(&vector->data, &vector->data_size);
}

void append_vector_ae_vec(ae_vec *const vector_to, ae_vec *const vector_from, size_t i, size_t n)
{
    append_base_ae_base(&vector_to->data, &vector_from->data, &vector_to->data_size, i, n);

    return;
}

void set_vector_ae_vec(ae_vec *const vector_to, ae_vec *const vector_from, size_t i_to, size_t i_from, size_t n)
{
    assert(vector_to->data_size == vector_from->data_size);

    set_base_ae_base(&vector_to->data, &vector_from->data, &vector_to->data_size, i_to, i_from, n);

    return;
}

void *get_ae_vec(const ae_vec *const vector, size_t i)
{
    return get_ae_base(&vector->data, &vector->data_size, i);
}

ae_vec concat_ae_vec(ae_vec *c_vector, ae_vec *const vector_to, ae_vec *const vector_from)
{
    assert(vector_to->data_size == vector_from->data_size);
    if (c_vector != NULL)
        assert(vector_to->data_size == c_vector->data_size);

    ae_vec new_vector = {.data = concat_ae_base(&c_vector->data, &vector_to->data, &vector_from->data, &vector_to->data_size),
                            .data_size = vector_to->data_size};

    return new_vector;
}

void *insert_ae_vec(ae_vec *const vector, size_t i)
{
    return insert_ae_base(&vector->data, &vector->data_size, i);
}

void insert_vector_ae_vec(ae_vec *const vector_to, ae_vec *const vector_from, size_t i_to, size_t i_from, size_t n)
{
    assert(vector_to->data_size == vector_from->data_size);

    insert_base_ae_base(&vector_to->data, &vector_from->data, &vector_to->data_size, i_to, i_from, n);

    return;
}

void delete_ae_vec(ae_vec *const vector, size_t i, void *const par)
{
    delete_ae_base(&vector->data, &vector->data_size, i, par);

    return;
}

void delete_vector_ae_vec(ae_vec *const vector, size_t i, size_t n, ae_vec *par)
{
    assert(vector != par);
    if (par != NULL)
        par->data_size = vector->data_size;
    delete_base_ae_base(&vector->data, &vector->data_size, i, n, &par->data);

    return;
}

void pop_ae_vec(ae_vec *const vector, void *const par)
{
    pop_ae_base(&vector->data, &vector->data_size, par);

    return;
}

void pop_vector_ae_vec(ae_vec *const vector, size_t n, ae_vec *par)
{
    assert(vector != par);
    if (par != NULL)
        par->data_size = vector->data_size;
    pop_base_ae_base(&vector->data, &vector->data_size, n, &par->data);

    return;
}

ae_vec duplicate_ae_vec(ae_vec *const d_vector, const ae_vec *const vector)
{
    ae_vec new_vector = {.data = duplicate_ae_base(&d_vector->data, &vector->data, &vector->data_size),
                            .data_size = vector->data_size};

    return new_vector;
}

void invert_ae_vec(ae_vec *const vector)
{
    invert_ae_base(&vector->data, &vector->data_size);

    return;
}

size_t find_ae_vec(const ae_vec *const vector, void *const par)
{
    return find_ae_base(&vector->data, &vector->data_size, par);
}

void sort_ae_vec(ae_vec *const vector, int (*comparator)(const void *, const void *))
{
    sort_ae_base(&vector->data, &vector->data_size, comparator);

    return;
}

void swap_ae_vec(ae_vec *const vector, size_t i, size_t j)
{
    swap_ae_base(&vector->data, &vector->data_size, i, j);

    return;
}

void optimize_ae_vec(ae_vec *const vector)
{
    optimize_ae_base(&vector->data, &vector->data_size);

    return;
}
