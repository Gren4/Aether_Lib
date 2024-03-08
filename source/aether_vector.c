#define AE_BASE
#include "aether_vector.h"
#include <assert.h>
#include <string.h>

void create_gc_ae_vector(ae_vector *gc, size_t data_size)
{
    gc->data_size = data_size;

    create_gc_ae_base(&gc->data, &gc->data_size);

    return;
}

ae_vector create_ae_vector(size_t data_size, size_t quant)
{
    ae_vector new_vector = {
        .data = create_ae_base(&data_size, quant),
        .data_size = data_size};

    return new_vector;
}

size_t gc_idx_ae_vector(ae_vector *const vector)
{
    return gc_idx_ae_base(&vector->data);
}

size_t quant_ae_vector(ae_vector *const vector)
{
    return quant_ae_base(&vector->data);
}

size_t max_quant_ae_vector(ae_vector *const vector)
{
    return max_quant_ae_base(&vector->data);
}

size_t data_size_ae_vector(ae_vector *const vector)
{
    return vector->data_size;
}

void free_ae_vector(ae_vector *const vector)
{
    free_ae_base(&vector->data);
    vector->data.memory = NULL;
    vector->data_size = 0;

    return;
}

void resize_ae_vector(ae_vector *const vector, size_t new_size)
{
    resize_ae_base(&vector->data, &vector->data_size, new_size);

    return;
}

void *append_ae_vector(ae_vector *const vector)
{
    return append_ae_base(&vector->data, &vector->data_size);
}

void append_vector_ae_vector(ae_vector *const vector_to, ae_vector *const vector_from, size_t i, size_t n)
{
    append_base_ae_base(&vector_to->data, &vector_from->data, &vector_to->data_size, i, n);

    return;
}

void set_vector_ae_vector(ae_vector *const vector_to, ae_vector *const vector_from, size_t i_to, size_t i_from, size_t n)
{
    assert(vector_to->data_size == vector_from->data_size);

    set_base_ae_base(&vector_to->data, &vector_from->data, &vector_to->data_size, i_to, i_from, n);

    return;
}

void *get_ae_vector(const ae_vector *const vector, size_t i)
{
    return get_ae_base(&vector->data, &vector->data_size, i);
}

ae_vector concat_ae_vector(ae_vector *c_vector, ae_vector *const vector_to, ae_vector *const vector_from)
{
    assert(vector_to->data_size == vector_from->data_size);
    if (c_vector != NULL)
        assert(vector_to->data_size == c_vector->data_size);

    ae_vector new_vector = {.data = concat_ae_base(&c_vector->data, &vector_to->data, &vector_from->data, &vector_to->data_size),
                            .data_size = vector_to->data_size};

    return new_vector;
}

void *insert_ae_vector(ae_vector *const vector, size_t i)
{
    return insert_ae_base(&vector->data, &vector->data_size, i);
}

void insert_vector_ae_vector(ae_vector *const vector_to, ae_vector *const vector_from, size_t i_to, size_t i_from, size_t n)
{
    assert(vector_to->data_size == vector_from->data_size);

    insert_base_ae_base(&vector_to->data, &vector_from->data, &vector_to->data_size, i_to, i_from, n);

    return;
}

void delete_ae_vector(ae_vector *const vector, size_t i, void *par)
{
    delete_ae_base(&vector->data, &vector->data_size, i, par);

    return;
}

void delete_vector_ae_vector(ae_vector *const vector, size_t i, size_t n, ae_vector *par)
{
    assert(vector != par);
    if (par != NULL)
        par->data_size = vector->data_size;
    delete_base_ae_base(&vector->data, &vector->data_size, i, n, &par->data);

    return;
}

void pop_ae_vector(ae_vector *const vector, void *par)
{
    pop_ae_base(&vector->data, &vector->data_size, par);

    return;
}

void pop_vector_ae_vector(ae_vector *const vector, size_t n, ae_vector *par)
{
    assert(vector != par);
    if (par != NULL)
        par->data_size = vector->data_size;
    pop_base_ae_base(&vector->data, &vector->data_size, n, &par->data);

    return;
}

ae_vector duplicate_ae_vector(ae_vector *d_vector, const ae_vector *const vector)
{
    ae_vector new_vector = {.data = duplicate_ae_base(&d_vector->data, &vector->data, &vector->data_size),
                            .data_size = vector->data_size};

    return new_vector;
}

void invert_ae_vector(ae_vector *const vector)
{
    invert_ae_base(&vector->data, &vector->data_size);

    return;
}

size_t find_ae_vector(const ae_vector *const vector, void *par)
{
    return find_ae_base(&vector->data, &vector->data_size, par);
}

void sort_ae_vector(ae_vector *const vector, int (*comparator)(const void *, const void *))
{
    sort_ae_base(&vector->data, &vector->data_size, comparator);

    return;
}

void swap_ae_vector(ae_vector *const vector, size_t i, size_t j)
{
    swap_ae_base(&vector->data, &vector->data_size, i, j);

    return;
}

void optimize_ae_vector(ae_vector *const vector)
{
    optimize_ae_base(&vector->data, &vector->data_size);

    return;
}
