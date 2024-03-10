#ifndef __AETHER_BASE__
#define __AETHER_BASE__

#include "aether_gc.h"

#define INIT_AE_BASE                                            \
    {                                                           \
        .memory = NULL, .gc_idx = 0, .quant = 0, .max_quant = 0 \
    }

typedef struct ae_base
{
    void *memory;
    int32_t gc_idx;
    size_t quant;
    size_t max_quant;
} ae_base;

#include <string.h>
#include <math.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#ifdef DEBUG_AE
#include <stdio.h>
#endif

#define AETHER_BASE_ADD_SIZE 2

typedef enum
{
    AE_BASE_INCR,
    AE_BASE_DECR,
    AE_BASE_RECOUNT,
    AE_RESIZE
} AE_BASE_MEM;

inline size_t find_next_power_of_2(size_t n)
{
    if (n == 0)
        return 1;
    else if (n == 1)
        return 2;
    else
    {
        n--;

        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;
        n |= n >> 32;

        return ++n;
    }
}

inline void check_realloc_ae_base(ae_base *const base, const size_t *const data_size, size_t new_size, AE_BASE_MEM type)
{
    uint8_t check = 0;

    switch (type)
    {
    case AE_RESIZE:
        check = 1;
        base->max_quant = find_next_power_of_2(new_size);
        break;
    case AE_BASE_RECOUNT:
        check = new_size > base->max_quant;
        if (check)
            base->max_quant = find_next_power_of_2(new_size);
        break;
    case AE_BASE_INCR:
        check = new_size > base->max_quant;
        if (check)
            base->max_quant = base->max_quant << 1;
        break;

    case AE_BASE_DECR:
        check = new_size < base->max_quant / 8;
        if (check)
            base->max_quant = base->max_quant >> 1;
        break;

    default:
        break;
    }

    if (check)
    {
        void *p = realloc(base->memory, base->max_quant * (*data_size));

        assert(p != NULL);

#ifdef DEBUG_AE
        printf("Reallocated memory from address %p to %p\n", base->memory, p);
#endif

        base->memory = p;

        update_ae_gc(base->gc_idx, base->memory);
    }

    return;
}

inline void create_gc_ae_base(ae_base *const base, const size_t *const data_size)
{
    base->max_quant = AETHER_BASE_ADD_SIZE;
    base->memory = calloc(base->max_quant, *data_size);

    if (base->memory == NULL)
        return;

#ifdef DEBUG_AE
    printf("Allocated memory at address %p\n", base->memory);
#endif

    base->quant = 0;

    base->gc_idx = append_ae_gc(base->memory);

    return;
}

inline ae_base create_ae_base(const size_t *const data_size, size_t new_size)
{
    ae_base new_base;

    if (new_size == 0)
        new_base.max_quant = AETHER_BASE_ADD_SIZE;
    else
        new_base.max_quant = find_next_power_of_2(new_size);
    new_base.memory = calloc(new_base.max_quant, *data_size);

    assert(new_base.memory != NULL);

#ifdef DEBUG_AE
    printf("Allocated memory at address %p\n", new_base.memory);
#endif

    new_base.quant = 0;

    new_base.gc_idx = append_ae_gc(new_base.memory);

    return new_base;
}

inline ae_base create_max_size_ae_base(const size_t *const data_size, size_t new_size)
{
    ae_base new_base;

    if (new_size == 0)
        new_base.max_quant = AETHER_BASE_ADD_SIZE;
    else
        new_base.max_quant = find_next_power_of_2(new_size);
    new_base.memory = calloc(new_base.max_quant, *data_size);

    assert(new_base.memory != NULL);

#ifdef DEBUG_AE
    printf("Allocated memory at address %p\n", new_base.memory);
#endif

    new_base.quant = new_base.max_quant;

    new_base.gc_idx = append_ae_gc(new_base.memory);

    return new_base;
}

inline void free_ae_base(ae_base *const base)
{
    assert(base != NULL);
#ifdef DEBUG_AE
    printf("Freed memory at address %p\n", base->memory);
#endif
    free(base->memory);
    base->quant = 0;
    base->max_quant = 0;
    remove_ae_gc(base->gc_idx);

    return;
}

inline void resize_ae_base(ae_base *const base, const size_t *const data_size, size_t new_size)
{
    if (new_size == base->quant)
        return;

    check_realloc_ae_base(base, data_size, new_size, AE_RESIZE);

    if (new_size > base->quant)
        memset(base->memory + base->quant * (*data_size), 0, (new_size - base->quant) * (*data_size));

    base->quant = new_size;

    return;
}

inline int32_t gc_idx_ae_base(ae_base *const base)
{
    return base->gc_idx;
}

inline size_t quant_ae_base(ae_base *const base)
{
    return base->quant;
}

inline size_t max_quant_ae_base(ae_base *const base)
{
    return base->max_quant;
}

inline void *append_ae_base(ae_base *const base, const size_t *const data_size)
{
    size_t new_size = base->quant + 1;
    check_realloc_ae_base(base, data_size, new_size, AE_BASE_INCR);
    base->quant++;

    return base->memory + (base->quant - 1) * (*data_size);
}

inline void append_base_ae_base(ae_base *const base_to, ae_base *const base_from, const size_t *const data_size, size_t i, size_t n)
{
    assert(i >= 0 && i < base_from->quant);

    if ((base_from->quant - i) < n)
        n = base_from->quant - i;

    size_t new_size = base_to->quant + n;
    check_realloc_ae_base(base_to, data_size, new_size, AE_BASE_INCR);

    memcpy(base_to->memory + base_to->quant * (*data_size), base_from->memory + i * (*data_size), n * (*data_size));

    base_to->quant += n;

    return;
}

inline void set_base_ae_base(ae_base *const base_to, ae_base *const base_from, const size_t *const data_size, size_t i_to, size_t i_from, size_t n)
{
    assert(i_to >= 0 && i_to < base_to->quant && i_from >= 0 && i_from < base_from->quant);

    if ((base_from->quant - i_from) < n)
        n = base_from->quant - i_from;

    if ((base_to->quant - i_to < n))
        n = base_to->quant - i_to;

    memcpy(base_to->memory + i_to * (*data_size), base_from->memory + i_from * (*data_size), n * (*data_size));

    return;
}

inline void *get_ae_base(const ae_base *const base, const size_t *const data_size, size_t i)
{
    assert(i >= 0 && i < base->quant);

    return base->memory + i * (*data_size);
}

inline ae_base concat_ae_base(ae_base *c_base, ae_base *const base_to, ae_base *const base_from, const size_t *const data_size)
{
    if (c_base == NULL)
    {
        assert(base_to->memory != NULL && base_from->memory != NULL);
        ae_base new_base = create_ae_base(data_size, base_to->quant + base_from->quant);
        memcpy(new_base.memory, base_to->memory, base_to->quant * (*data_size));
        memcpy(new_base.memory + base_to->quant * (*data_size), base_from->memory, base_from->quant * (*data_size));
        new_base.quant = base_to->quant + base_from->quant;

        return new_base;
    }
    else
    {
        assert(c_base->memory != NULL && base_to->memory != NULL && base_from->memory != NULL);
        check_realloc_ae_base(c_base, data_size, base_to->quant + base_from->quant, AE_BASE_RECOUNT);
        if (c_base == base_to)
        {
            memcpy(c_base->memory + base_to->quant * (*data_size), base_from->memory, base_from->quant * (*data_size));
        }
        else if (c_base == base_from)
        {
            memcpy(c_base->memory + base_to->quant * (*data_size), base_from->memory, base_from->quant * (*data_size));
            memcpy(c_base->memory, base_to->memory, base_to->quant * (*data_size));
        }
        else
        {
            memcpy(c_base->memory, base_to->memory, base_to->quant * (*data_size));
            memcpy(c_base->memory + base_to->quant * (*data_size), base_from->memory, base_from->quant * (*data_size));
        }
        c_base->quant = base_to->quant + base_from->quant;

        return *c_base;
    }
}

inline void *insert_ae_base(ae_base *const base, const size_t *const data_size, size_t i)
{
    size_t new_size = base->quant + 1;
    check_realloc_ae_base(base, data_size, new_size, AE_BASE_INCR);

    memcpy(base->memory + (i + 1) * (*data_size), base->memory + i * (*data_size), (base->quant - i) * (*data_size));

    base->quant++;

    return base->memory + i * (*data_size);
}

inline void insert_base_ae_base(ae_base *const base_to, ae_base *const base_from, const size_t *const data_size, size_t i_to, size_t i_from, size_t n)
{
    assert(i_to >= 0 && i_to < base_to->quant && i_from >= 0 && i_from < base_from->quant && base_to->memory != base_from->memory);

    if ((base_from->quant - i_from) < n)
        n = base_from->quant - i_from;

    size_t new_size = base_to->quant + n;
    check_realloc_ae_base(base_to, data_size, new_size, AE_BASE_INCR);

    memcpy(base_to->memory + (i_to + n) * (*data_size), base_to->memory + i_to * (*data_size), (base_to->quant - i_to) * (*data_size));
    memcpy(base_to->memory + i_to * (*data_size), base_from->memory + i_from * (*data_size), n * (*data_size));

    base_to->quant += n;

    return;
}

inline void delete_ae_base(ae_base *const base, const size_t *const data_size, size_t i, void *const par)
{
    assert(i >= 0 && i < base->quant);

    if (par != NULL)
        memcpy(par, base->memory + i * (*data_size), *data_size);
    if (i != base->quant - 1)
        memcpy(base->memory + i * (*data_size), base->memory + (i + 1) * (*data_size), (base->quant - i) * (*data_size));

    size_t new_size = base->quant - 1;
    check_realloc_ae_base(base, data_size, new_size, AE_BASE_DECR);

    base->quant--;

    return;
}

inline void delete_base_ae_base(ae_base *const base, const size_t *const data_size, size_t i, size_t n, ae_base *par)
{
    assert(i >= 0 && i < base->quant);
    if (n > base->quant - i)
        n = base->quant - i;

    if (par != NULL)
    {
        if (par->memory != NULL && par->memory != base->memory)
            check_realloc_ae_base(par, data_size, n, AE_BASE_RECOUNT);
        else
            *par = create_ae_base(data_size, n);
        memcpy(par->memory, base->memory + i * (*data_size), n * (*data_size));
        par->quant = n;
    }
    if (i != base->quant - n)
        memcpy(base->memory + i * (*data_size), base->memory + (i + n) * (*data_size), (base->quant - i - n) * (*data_size));

    size_t new_size = base->quant - n;
    check_realloc_ae_base(base, data_size, new_size, AE_BASE_DECR);

    base->quant -= n;

    return;
}

inline void pop_ae_base(ae_base *const base, const size_t *const data_size, void *const par)
{
    assert(base->quant != 0);

    if (par != NULL)
        memcpy(par, base->memory + (base->quant - 1) * (*data_size), *data_size);

    size_t new_size = base->quant - 1;
    check_realloc_ae_base(base, data_size, new_size, AE_BASE_DECR);

    base->quant--;

    return;
}

inline void pop_base_ae_base(ae_base *const base, const size_t *const data_size, size_t n, ae_base *par)
{
    assert(base->quant != 0);

    if (n > base->quant)
        n = base->quant;

    if (par != NULL)
    {
        if (par->memory != NULL && par->memory != base->memory)
            check_realloc_ae_base(par, data_size, n, AE_BASE_RECOUNT);
        else
            *par = create_ae_base(data_size, n);
        memcpy(par->memory, base->memory + (base->quant - n) * (*data_size), n * (*data_size));
        par->quant = n;
    }

    size_t new_size = base->quant - n;
    check_realloc_ae_base(base, data_size, new_size, AE_BASE_DECR);

    base->quant -= n;

    return;
}

inline ae_base duplicate_ae_base(ae_base *d_base, const ae_base *const base, const size_t *const data_size)
{
    if (d_base == NULL)
    {
        assert(base->memory != NULL);
        ae_base new_base = create_ae_base(data_size, base->quant);
        memcpy(new_base.memory, base->memory, base->quant * (*data_size));
        new_base.quant = base->quant;

        return new_base;
    }
    else
    {
        if (d_base != base)
        {
            assert(d_base->memory != NULL && base->memory != NULL);
            check_realloc_ae_base(d_base, data_size, base->quant, AE_BASE_RECOUNT);
            memcpy(d_base->memory, base->memory, base->quant * (*data_size));
            d_base->quant = base->quant;
        }

        return *d_base;
    }
}

inline void invert_ae_base(ae_base *const base, const size_t *const data_size)
{
    if (base->quant > 1)
    {
        size_t iterations = base->quant / 2;
        size_t i = 0;
        void *par = malloc(*data_size);

        assert(par != NULL);

        for (i = 0; i < iterations; i++)
        {
            memcpy(par, base->memory + i * (*data_size), *data_size);
            memcpy(base->memory + i * (*data_size), base->memory + (base->quant - 1 - i) * (*data_size), *data_size);
            memcpy(base->memory + (base->quant - 1 - i) * (*data_size), par, *data_size);
        }

        free(par);
    }

    return;
}

inline size_t find_ae_base(const ae_base *const base, const size_t *const data_size, void *const par)
{
    for (size_t i = 0; i < base->quant; i++)
    {
        if (memcmp(par, base->memory + i * (*data_size), *data_size) == 0)
        {
            return i;
        }
    }

    return base->quant;
}

inline void sort_ae_base(ae_base *const base, const size_t *const data_size, int (*comparator)(const void *, const void *))
{
    qsort(base->memory, base->quant, *data_size, comparator);

    return;
}

inline void swap_ae_base(ae_base *const base, const size_t *const data_size, size_t i, size_t j)
{
    void *p = malloc(*data_size);
    assert(p != NULL);

    memcpy(p, base->memory + j * (*data_size), *data_size);
    memcpy(base->memory + j * (*data_size), base->memory + i * (*data_size), *data_size);
    memcpy(base->memory + i * (*data_size), p, *data_size);

    free(p);

    return;
}

inline void optimize_ae_base(ae_base *const base, const size_t *const data_size)
{
    assert(base->memory != NULL);

    if (base->quant == 0)
        return;

    void *p = realloc(base->memory, base->quant * (*data_size));

    assert(p != NULL);

#ifdef DEBUG_AE
    printf("Reallocated memory from address %p to %p\n", base->memory, p);
#endif

    base->memory = p;

    base->max_quant = base->quant;

    update_ae_gc(base->gc_idx, base->memory);

    return;
}

#endif // __AETHER_BASE__
