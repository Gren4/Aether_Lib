#include "aether_matrix_float.h"
#include "aether_utils.h"
#include "aether_gc.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define AE_MATRIX_ELEM(m, r, c) (m.data[(c) + (r) * m.cols])
#define AE_MATRIX_P_ELEM(m, r, c) (m->data[(c) + (r) * m->cols])

#define AE_FLOAT_PRECISION 1e6

extern inline uint32_t rows_ae_mat_f(const ae_mat_f *const matrix);
extern inline uint32_t cols_ae_mat_f(const ae_mat_f *const matrix);

ae_mat_f create_ae_mat_f(uint32_t rows, uint32_t cols)
{
    assert(rows > 0 && cols > 0);
    ae_mat_f new_matrix = {.data = calloc(rows * cols, sizeof(double)),
                           .rows = rows,
                           .cols = cols};
    assert(new_matrix.data != NULL);
#ifdef DEBUG_AE
    printf("Allocated memory at address %p\n", new_matrix.data);
#endif
    new_matrix.gc_idx = append_ae_gc((void *)new_matrix.data);

    return new_matrix;
}

static inline ae_mat_f create_temp_ae_mat_f(const uint32_t rows, const uint32_t cols)
{
    assert(rows > 0 && cols > 0);
    ae_mat_f new_matrix = {.data = calloc(rows * cols, sizeof(double)),
                           .rows = rows,
                           .cols = cols};
    assert(new_matrix.data != NULL);
#ifdef DEBUG_AE
    printf("Allocated memory at address %p\n", new_matrix.data);
#endif
    new_matrix.gc_idx = -1;

    return new_matrix;
}

ae_mat_f identity_ae_mat_f(const uint32_t dim)
{
    assert(dim > 0);
    ae_mat_f new_matrix = {.data = calloc(dim * dim, sizeof(double)),
                           .rows = dim,
                           .cols = dim};
    assert(new_matrix.data != NULL);
#ifdef DEBUG_AE
    printf("Allocated memory at address %p\n", new_matrix.data);
#endif
    new_matrix.gc_idx = append_ae_gc((void *)new_matrix.data);
    for (uint32_t i = 0; i < dim; i++)
    {
        for (uint32_t j = 0; j < dim; j++)
        {
            AE_MATRIX_ELEM(new_matrix, i, j) = i == j ? 1.0 : 0.0;
        }
    }
    return new_matrix;
}

static inline ae_mat_f identity_temp_ae_mat_f(const uint32_t dim)
{
    assert(dim > 0);
    ae_mat_f new_matrix = {.data = calloc(dim * dim, sizeof(double)),
                           .rows = dim,
                           .cols = dim};
    assert(new_matrix.data != NULL);
#ifdef DEBUG_AE
    printf("Allocated memory at address %p\n", new_matrix.data);
#endif
    new_matrix.gc_idx = -1;
    for (uint32_t i = 0; i < dim; i++)
    {
        for (uint32_t j = 0; j < dim; j++)
        {
            AE_MATRIX_ELEM(new_matrix, i, j) = i == j ? 1.0 : 0.0;
        }
    }
    return new_matrix;
}

void free_ae_mat_f(ae_mat_f *const matrix)
{
    if (matrix->data != NULL)
    {
#ifdef DEBUG_AE
        printf("Freed memory at address %p\n", matrix->data);
#endif
        free(matrix->data);
        remove_ae_gc(matrix->gc_idx);
    }

    matrix->cols = 0;
    matrix->rows = 0;
    return;
}

double *get_ae_mat_f(const ae_mat_f *const matrix, const uint32_t row, const uint32_t col)
{
    assert(col + matrix->cols * row < matrix->cols * matrix->rows);

    return matrix->data + col + row * matrix->cols;
}

ae_mat_f sum_ae_mat_f(ae_mat_f *const m_matrix, const ae_mat_f *const matrix1, const ae_mat_f *const matrix2)
{
    assert(matrix1->cols == matrix2->cols && matrix1->rows == matrix2->rows);

    ae_mat_f new_matrix;
    if (m_matrix == NULL)
        new_matrix = create_ae_mat_f(matrix1->rows, matrix1->cols);
    else
        new_matrix = create_temp_ae_mat_f(matrix1->rows, matrix1->cols);

    for (uint32_t i = 0; i < new_matrix.rows; i++)
        for (uint32_t j = 0; j < new_matrix.cols; j++)
            AE_MATRIX_ELEM(new_matrix, i, j) = AE_MATRIX_P_ELEM(matrix1, i, j) + AE_MATRIX_P_ELEM(matrix2, i, j);

    if (m_matrix != NULL)
    {
        free(m_matrix->data);
        if (m_matrix->gc_idx != -1)
        {
            new_matrix.gc_idx = m_matrix->gc_idx;
            update_ae_gc(m_matrix->gc_idx, new_matrix.data);
        }
    }

    return new_matrix;
}

ae_mat_f sub_ae_mat_f(ae_mat_f *const m_matrix, const ae_mat_f *const matrix1, const ae_mat_f *const matrix2)
{
    assert(matrix1->cols == matrix2->cols && matrix1->rows == matrix2->rows);

    ae_mat_f new_matrix;
    if (m_matrix == NULL)
        new_matrix = create_ae_mat_f(matrix1->rows, matrix1->cols);
    else
        new_matrix = create_temp_ae_mat_f(matrix1->rows, matrix1->cols);

    for (uint32_t i = 0; i < new_matrix.rows; i++)
        for (uint32_t j = 0; j < new_matrix.cols; j++)
            AE_MATRIX_ELEM(new_matrix, i, j) = AE_MATRIX_P_ELEM(matrix1, i, j) - AE_MATRIX_P_ELEM(matrix2, i, j);

    if (m_matrix != NULL)
    {
        free(m_matrix->data);
        if (m_matrix->gc_idx != -1)
        {
            new_matrix.gc_idx = m_matrix->gc_idx;
            update_ae_gc(m_matrix->gc_idx, new_matrix.data);
        }
    }

    return new_matrix;
}

ae_mat_f mult_by_sclr_ae_mat_f(ae_mat_f *const m_matrix, const ae_mat_f *const matrix, const double scalar)
{
    ae_mat_f new_matrix;
    if (m_matrix == NULL)
        new_matrix = create_ae_mat_f(matrix->rows, matrix->cols);
    else
        new_matrix = create_temp_ae_mat_f(matrix->rows, matrix->cols);

    for (uint32_t i = 0; i < new_matrix.rows; i++)
        for (uint32_t j = 0; j < new_matrix.cols; j++)
            AE_MATRIX_ELEM(new_matrix, i, j) = AE_MATRIX_P_ELEM(matrix, i, j) * scalar;

    if (m_matrix != NULL)
    {
        free(m_matrix->data);
        if (m_matrix->gc_idx != -1)
        {
            new_matrix.gc_idx = m_matrix->gc_idx;
            update_ae_gc(m_matrix->gc_idx, new_matrix.data);
        }
    }

    return new_matrix;
}

static inline ae_mat_f resize_temp_ae_mat_f(const ae_mat_f *const matrix, const uint32_t rows, const uint32_t cols)
{
    ae_mat_f new_matrix = create_temp_ae_mat_f(rows, cols);

    for (uint32_t i = 0; i < matrix->rows; i++)
    {
        for (uint32_t j = 0; j < matrix->cols; j++)
            AE_MATRIX_ELEM(new_matrix, i, j) = AE_MATRIX_P_ELEM(matrix, i, j);
        for (uint32_t j = matrix->cols; j < new_matrix.cols; j++)
            AE_MATRIX_ELEM(new_matrix, i, j) = 0.0;
    }

    for (uint32_t i = matrix->rows; i < new_matrix.rows; i++)
        for (uint32_t j = 0; j < new_matrix.cols; j++)
            AE_MATRIX_ELEM(new_matrix, i, j) = 0.0;

    return new_matrix;
}

static inline ae_mat_f mult_transpose_ae_mat_f(ae_mat_f *const m_matrix, const ae_mat_f *const matrix1, const ae_mat_f *const matrix2)
{
    ae_mat_f new_matrix;
    if (m_matrix == NULL)
        new_matrix = create_ae_mat_f(matrix1->rows, matrix2->cols);
    else
        new_matrix = create_temp_ae_mat_f(matrix1->rows, matrix2->cols);

    double *matrix_cache = calloc(matrix1->cols, sizeof(double));

    for (uint32_t j = 0; j < new_matrix.cols; j++)
    {
        for (uint32_t k = 0; k < matrix1->cols; k++)
            matrix_cache[k] = AE_MATRIX_P_ELEM(matrix2, k, j);
        for (uint32_t i = 0; i < new_matrix.rows; i++)
        {
            AE_MATRIX_ELEM(new_matrix, i, j) = AE_MATRIX_P_ELEM(matrix1, i, 0) * matrix_cache[0];
            for (uint32_t k = 1; k < matrix1->cols; k++)
                AE_MATRIX_ELEM(new_matrix, i, j) += AE_MATRIX_P_ELEM(matrix1, i, k) * matrix_cache[k];
        }
    }

    free(matrix_cache);

    if (m_matrix != NULL)
    {
        free(m_matrix->data);
        if (m_matrix->gc_idx != -1)
        {
            new_matrix.gc_idx = m_matrix->gc_idx;
            update_ae_gc(m_matrix->gc_idx, new_matrix.data);
        }
    }

    return new_matrix;
}

static inline ae_mat_f mult_standart_ae_mat_f(ae_mat_f *const m_matrix, const ae_mat_f *const matrix1, const ae_mat_f *const matrix2)
{
    ae_mat_f new_matrix;
    if (m_matrix == NULL)
        new_matrix = create_ae_mat_f(matrix1->rows, matrix2->cols);
    else
        new_matrix = create_temp_ae_mat_f(matrix1->rows, matrix2->cols);

    for (uint32_t i = 0; i < new_matrix.rows; i++)
        for (uint32_t j = 0; j < new_matrix.cols; j++)
        {
            AE_MATRIX_ELEM(new_matrix, i, j) = AE_MATRIX_P_ELEM(matrix1, i, 0) * AE_MATRIX_P_ELEM(matrix2, 0, j);
            for (uint32_t k = 1; k < matrix1->cols; k++)
                AE_MATRIX_ELEM(new_matrix, i, j) += AE_MATRIX_P_ELEM(matrix1, i, k) * AE_MATRIX_P_ELEM(matrix2, k, j);
        }

    if (m_matrix != NULL)
    {
        free(m_matrix->data);
        if (m_matrix->gc_idx != -1)
        {
            new_matrix.gc_idx = m_matrix->gc_idx;
            update_ae_gc(m_matrix->gc_idx, new_matrix.data);
        }
    }

    return new_matrix;
}

static void dop_strassen_mult_by_mat_ae_mat_f(ae_mat_f *const m_matrix, const ae_mat_f *const matrix1, const ae_mat_f *const matrix2)
{
    if (matrix1->rows == 2)
    {
        AE_MATRIX_P_ELEM(m_matrix, 0, 0) = AE_MATRIX_P_ELEM(matrix1, 0, 0) * AE_MATRIX_P_ELEM(matrix2, 0, 0) + AE_MATRIX_P_ELEM(matrix1, 0, 1) * AE_MATRIX_P_ELEM(matrix2, 1, 0);
        AE_MATRIX_P_ELEM(m_matrix, 0, 1) = AE_MATRIX_P_ELEM(matrix1, 0, 0) * AE_MATRIX_P_ELEM(matrix2, 0, 1) + AE_MATRIX_P_ELEM(matrix1, 0, 1) * AE_MATRIX_P_ELEM(matrix2, 1, 1);
        AE_MATRIX_P_ELEM(m_matrix, 1, 0) = AE_MATRIX_P_ELEM(matrix1, 1, 0) * AE_MATRIX_P_ELEM(matrix2, 0, 0) + AE_MATRIX_P_ELEM(matrix1, 1, 1) * AE_MATRIX_P_ELEM(matrix2, 1, 0);
        AE_MATRIX_P_ELEM(m_matrix, 1, 1) = AE_MATRIX_P_ELEM(matrix1, 1, 0) * AE_MATRIX_P_ELEM(matrix2, 0, 1) + AE_MATRIX_P_ELEM(matrix1, 1, 1) * AE_MATRIX_P_ELEM(matrix2, 1, 1);
    }
    else
    {
        uint32_t n = matrix1->rows / 2;
        uint32_t offset = n * n * sizeof(double);

        void *data = malloc(21 * offset);

        ae_mat_f a = {.data = data, .rows = n, .cols = n};
        ae_mat_f d = {.data = data + offset, .rows = n, .cols = n};
        ae_mat_f e = {.data = data + 2 * offset, .rows = n, .cols = n};
        ae_mat_f h = {.data = data + 3 * offset, .rows = n, .cols = n};

        ae_mat_f s1 = {.data = data + 4 * offset, .rows = n, .cols = n};
        ae_mat_f s2 = {.data = data + 5 * offset, .rows = n, .cols = n};
        ae_mat_f s3 = {.data = data + 6 * offset, .rows = n, .cols = n};
        ae_mat_f s4 = {.data = data + 7 * offset, .rows = n, .cols = n};
        ae_mat_f s5 = {.data = data + 8 * offset, .rows = n, .cols = n};
        ae_mat_f s6 = {.data = data + 9 * offset, .rows = n, .cols = n};
        ae_mat_f s7 = {.data = data + 10 * offset, .rows = n, .cols = n};

        ae_mat_f t1 = {.data = data + 11 * offset, .rows = n, .cols = n};
        ae_mat_f t2 = {.data = data + 12 * offset, .rows = n, .cols = n};
        ae_mat_f t3 = {.data = data + 13 * offset, .rows = n, .cols = n};
        ae_mat_f t4 = {.data = data + 14 * offset, .rows = n, .cols = n};
        ae_mat_f t5 = {.data = data + 15 * offset, .rows = n, .cols = n};
        ae_mat_f t6 = {.data = data + 16 * offset, .rows = n, .cols = n};
        ae_mat_f t7 = {.data = data + 17 * offset, .rows = n, .cols = n};
        ae_mat_f t8 = {.data = data + 18 * offset, .rows = n, .cols = n};
        ae_mat_f t9 = {.data = data + 19 * offset, .rows = n, .cols = n};
        ae_mat_f t10 = {.data = data + 20 * offset, .rows = n, .cols = n};

        for (uint32_t i = 0; i < n; i++)
        {
            for (uint32_t j = 0; j < n; j++)
            {
                AE_MATRIX_ELEM(a, i, j) = AE_MATRIX_P_ELEM(matrix1, i, j);
                AE_MATRIX_ELEM(d, i, j) = AE_MATRIX_P_ELEM(matrix1, i + n, j + n);
                AE_MATRIX_ELEM(e, i, j) = AE_MATRIX_P_ELEM(matrix2, i, j);
                AE_MATRIX_ELEM(h, i, j) = AE_MATRIX_P_ELEM(matrix2, i + n, j + n);

                AE_MATRIX_ELEM(t1, i, j) = AE_MATRIX_P_ELEM(matrix2, i, j + n) - AE_MATRIX_P_ELEM(matrix2, i + n, j + n);
                AE_MATRIX_ELEM(t2, i, j) = AE_MATRIX_P_ELEM(matrix1, i, j) + AE_MATRIX_P_ELEM(matrix1, i, j + n);
                AE_MATRIX_ELEM(t3, i, j) = AE_MATRIX_P_ELEM(matrix1, i + n, j) + AE_MATRIX_P_ELEM(matrix1, i + n, j + n);
                AE_MATRIX_ELEM(t4, i, j) = AE_MATRIX_P_ELEM(matrix2, i + n, j) - AE_MATRIX_P_ELEM(matrix2, i, j);
                AE_MATRIX_ELEM(t5, i, j) = AE_MATRIX_P_ELEM(matrix1, i, j) + AE_MATRIX_P_ELEM(matrix1, i + n, j + n);
                AE_MATRIX_ELEM(t6, i, j) = AE_MATRIX_P_ELEM(matrix2, i, j) + AE_MATRIX_P_ELEM(matrix2, i + n, j + n);
                AE_MATRIX_ELEM(t7, i, j) = AE_MATRIX_P_ELEM(matrix1, i, j + n) - AE_MATRIX_P_ELEM(matrix1, i + n, j + n);
                AE_MATRIX_ELEM(t8, i, j) = AE_MATRIX_P_ELEM(matrix2, i + n, j) + AE_MATRIX_P_ELEM(matrix2, i + n, j + n);
                AE_MATRIX_ELEM(t9, i, j) = AE_MATRIX_P_ELEM(matrix1, i, j) - AE_MATRIX_P_ELEM(matrix1, i + n, j);
                AE_MATRIX_ELEM(t10, i, j) = AE_MATRIX_P_ELEM(matrix2, i, j) + AE_MATRIX_P_ELEM(matrix2, i, j + n);
            }
        }

        dop_strassen_mult_by_mat_ae_mat_f(&s1, &a, &t1);
        dop_strassen_mult_by_mat_ae_mat_f(&s2, &t2, &h);
        dop_strassen_mult_by_mat_ae_mat_f(&s3, &t3, &e);
        dop_strassen_mult_by_mat_ae_mat_f(&s4, &d, &t4);
        dop_strassen_mult_by_mat_ae_mat_f(&s5, &t5, &t6);
        dop_strassen_mult_by_mat_ae_mat_f(&s6, &t7, &t8);
        dop_strassen_mult_by_mat_ae_mat_f(&s7, &t9, &t10);

        for (uint32_t i = 0; i < n; i++)
        {
            for (uint32_t j = 0; j < n; j++)
            {
                AE_MATRIX_P_ELEM(m_matrix, i, j) = AE_MATRIX_ELEM(s5, i, j) + AE_MATRIX_ELEM(s6, i, j) + AE_MATRIX_ELEM(s4, i, j) - AE_MATRIX_ELEM(s2, i, j);
                AE_MATRIX_P_ELEM(m_matrix, i, j + n) = AE_MATRIX_ELEM(s1, i, j) + AE_MATRIX_ELEM(s2, i, j);
                AE_MATRIX_P_ELEM(m_matrix, i + n, j) = AE_MATRIX_ELEM(s3, i, j) + AE_MATRIX_ELEM(s4, i, j);
                AE_MATRIX_P_ELEM(m_matrix, i + n, j + n) = AE_MATRIX_ELEM(s1, i, j) - AE_MATRIX_ELEM(s7, i, j) - AE_MATRIX_ELEM(s3, i, j) + AE_MATRIX_ELEM(s5, i, j);
            }
        }

        free(data);
    }

    return;
}

static inline ae_mat_f strassen_mult_by_mat_ae_mat_f(ae_mat_f *const m_matrix, const ae_mat_f *const matrix1, const ae_mat_f *const matrix2)
{
    ae_mat_f new_matrix;
    if (m_matrix == NULL)
        new_matrix = create_ae_mat_f(matrix1->rows, matrix1->cols);
    else
        new_matrix = create_temp_ae_mat_f(matrix1->rows, matrix1->cols);

    if (matrix1->rows == 2)
    {
        AE_MATRIX_ELEM(new_matrix, 0, 0) = AE_MATRIX_P_ELEM(matrix1, 0, 0) * AE_MATRIX_P_ELEM(matrix2, 0, 0) + AE_MATRIX_P_ELEM(matrix1, 0, 1) * AE_MATRIX_P_ELEM(matrix2, 1, 0);
        AE_MATRIX_ELEM(new_matrix, 0, 1) = AE_MATRIX_P_ELEM(matrix1, 0, 0) * AE_MATRIX_P_ELEM(matrix2, 0, 1) + AE_MATRIX_P_ELEM(matrix1, 0, 1) * AE_MATRIX_P_ELEM(matrix2, 1, 1);
        AE_MATRIX_ELEM(new_matrix, 1, 0) = AE_MATRIX_P_ELEM(matrix1, 1, 0) * AE_MATRIX_P_ELEM(matrix2, 0, 0) + AE_MATRIX_P_ELEM(matrix1, 1, 1) * AE_MATRIX_P_ELEM(matrix2, 1, 0);
        AE_MATRIX_ELEM(new_matrix, 1, 1) = AE_MATRIX_P_ELEM(matrix1, 1, 0) * AE_MATRIX_P_ELEM(matrix2, 0, 1) + AE_MATRIX_P_ELEM(matrix1, 1, 1) * AE_MATRIX_P_ELEM(matrix2, 1, 1);
    }
    else
    {
        uint32_t n = matrix1->rows / 2;
        uint32_t offset = n * n * sizeof(double);

        void *data = malloc(21 * offset);

        ae_mat_f a = {.data = data, .rows = n, .cols = n};
        ae_mat_f d = {.data = data + offset, .rows = n, .cols = n};
        ae_mat_f e = {.data = data + 2 * offset, .rows = n, .cols = n};
        ae_mat_f h = {.data = data + 3 * offset, .rows = n, .cols = n};

        ae_mat_f s1 = {.data = data + 4 * offset, .rows = n, .cols = n};
        ae_mat_f s2 = {.data = data + 5 * offset, .rows = n, .cols = n};
        ae_mat_f s3 = {.data = data + 6 * offset, .rows = n, .cols = n};
        ae_mat_f s4 = {.data = data + 7 * offset, .rows = n, .cols = n};
        ae_mat_f s5 = {.data = data + 8 * offset, .rows = n, .cols = n};
        ae_mat_f s6 = {.data = data + 9 * offset, .rows = n, .cols = n};
        ae_mat_f s7 = {.data = data + 10 * offset, .rows = n, .cols = n};

        ae_mat_f t1 = {.data = data + 11 * offset, .rows = n, .cols = n};
        ae_mat_f t2 = {.data = data + 12 * offset, .rows = n, .cols = n};
        ae_mat_f t3 = {.data = data + 13 * offset, .rows = n, .cols = n};
        ae_mat_f t4 = {.data = data + 14 * offset, .rows = n, .cols = n};
        ae_mat_f t5 = {.data = data + 15 * offset, .rows = n, .cols = n};
        ae_mat_f t6 = {.data = data + 16 * offset, .rows = n, .cols = n};
        ae_mat_f t7 = {.data = data + 17 * offset, .rows = n, .cols = n};
        ae_mat_f t8 = {.data = data + 18 * offset, .rows = n, .cols = n};
        ae_mat_f t9 = {.data = data + 19 * offset, .rows = n, .cols = n};
        ae_mat_f t10 = {.data = data + 20 * offset, .rows = n, .cols = n};

        for (uint32_t i = 0; i < n; i++)
        {
            for (uint32_t j = 0; j < n; j++)
            {
                AE_MATRIX_ELEM(a, i, j) = AE_MATRIX_P_ELEM(matrix1, i, j);
                AE_MATRIX_ELEM(d, i, j) = AE_MATRIX_P_ELEM(matrix1, i + n, j + n);
                AE_MATRIX_ELEM(e, i, j) = AE_MATRIX_P_ELEM(matrix2, i, j);
                AE_MATRIX_ELEM(h, i, j) = AE_MATRIX_P_ELEM(matrix2, i + n, j + n);

                AE_MATRIX_ELEM(t1, i, j) = AE_MATRIX_P_ELEM(matrix2, i, j + n) - AE_MATRIX_P_ELEM(matrix2, i + n, j + n);
                AE_MATRIX_ELEM(t2, i, j) = AE_MATRIX_P_ELEM(matrix1, i, j) + AE_MATRIX_P_ELEM(matrix1, i, j + n);
                AE_MATRIX_ELEM(t3, i, j) = AE_MATRIX_P_ELEM(matrix1, i + n, j) + AE_MATRIX_P_ELEM(matrix1, i + n, j + n);
                AE_MATRIX_ELEM(t4, i, j) = AE_MATRIX_P_ELEM(matrix2, i + n, j) - AE_MATRIX_P_ELEM(matrix2, i, j);
                AE_MATRIX_ELEM(t5, i, j) = AE_MATRIX_P_ELEM(matrix1, i, j) + AE_MATRIX_P_ELEM(matrix1, i + n, j + n);
                AE_MATRIX_ELEM(t6, i, j) = AE_MATRIX_P_ELEM(matrix2, i, j) + AE_MATRIX_P_ELEM(matrix2, i + n, j + n);
                AE_MATRIX_ELEM(t7, i, j) = AE_MATRIX_P_ELEM(matrix1, i, j + n) - AE_MATRIX_P_ELEM(matrix1, i + n, j + n);
                AE_MATRIX_ELEM(t8, i, j) = AE_MATRIX_P_ELEM(matrix2, i + n, j) + AE_MATRIX_P_ELEM(matrix2, i + n, j + n);
                AE_MATRIX_ELEM(t9, i, j) = AE_MATRIX_P_ELEM(matrix1, i, j) - AE_MATRIX_P_ELEM(matrix1, i + n, j);
                AE_MATRIX_ELEM(t10, i, j) = AE_MATRIX_P_ELEM(matrix2, i, j) + AE_MATRIX_P_ELEM(matrix2, i, j + n);
            }
        }

        dop_strassen_mult_by_mat_ae_mat_f(&s1, &a, &t1);
        dop_strassen_mult_by_mat_ae_mat_f(&s2, &t2, &h);
        dop_strassen_mult_by_mat_ae_mat_f(&s3, &t3, &e);
        dop_strassen_mult_by_mat_ae_mat_f(&s4, &d, &t4);
        dop_strassen_mult_by_mat_ae_mat_f(&s5, &t5, &t6);
        dop_strassen_mult_by_mat_ae_mat_f(&s6, &t7, &t8);
        dop_strassen_mult_by_mat_ae_mat_f(&s7, &t9, &t10);

        for (uint32_t i = 0; i < n; i++)
        {
            for (uint32_t j = 0; j < n; j++)
            {
                AE_MATRIX_ELEM(new_matrix, i, j) = AE_MATRIX_ELEM(s5, i, j) + AE_MATRIX_ELEM(s6, i, j) + AE_MATRIX_ELEM(s4, i, j) - AE_MATRIX_ELEM(s2, i, j);
                AE_MATRIX_ELEM(new_matrix, i, j + n) = AE_MATRIX_ELEM(s1, i, j) + AE_MATRIX_ELEM(s2, i, j);
                AE_MATRIX_ELEM(new_matrix, i + n, j) = AE_MATRIX_ELEM(s3, i, j) + AE_MATRIX_ELEM(s4, i, j);
                AE_MATRIX_ELEM(new_matrix, i + n, j + n) = AE_MATRIX_ELEM(s1, i, j) - AE_MATRIX_ELEM(s7, i, j) - AE_MATRIX_ELEM(s3, i, j) + AE_MATRIX_ELEM(s5, i, j);
            }
        }

        free(data);
    }

    if (m_matrix != NULL)
    {
        free(m_matrix->data);
        if (m_matrix->gc_idx != -1)
        {
            new_matrix.gc_idx = m_matrix->gc_idx;
            update_ae_gc(m_matrix->gc_idx, new_matrix.data);
        }
    }

    return new_matrix;
}

ae_mat_f mult_by_mat_ae_mat_f(ae_mat_f *const m_matrix, const ae_mat_f *const matrix1, const ae_mat_f *const matrix2)
{
    assert(matrix1->cols == matrix2->rows);

    if (matrix1->cols < 64)
        return mult_standart_ae_mat_f(m_matrix, matrix1, matrix2);
    else if (matrix1->cols < 1024)
        return mult_standart_ae_mat_f(m_matrix, matrix1, matrix2);
    else
    {
        uint32_t dim;
        {
            uint32_t mc1 = find_next_power_of_2(matrix1->cols);
            uint32_t mr1 = find_next_power_of_2(matrix1->rows);
            uint32_t mc2 = find_next_power_of_2(matrix2->cols);
            uint32_t mr2 = find_next_power_of_2(matrix2->rows);
            uint32_t max_m1 = max_ae(mc1, mr1);
            uint32_t max_m2 = max_ae(mc2, mr2);
            dim = max_ae(max_m1, max_m2);
        }

        uint8_t u1 = matrix1->cols == dim && matrix1->rows == dim;
        uint8_t u2 = matrix2->cols == dim && matrix2->rows == dim;
        if (u1 == 0 && u2 == 0)
        {
            ae_mat_f new_matrix;
            if (m_matrix == NULL)
                new_matrix = create_ae_mat_f(matrix1->rows, matrix2->cols);
            else
                new_matrix = create_temp_ae_mat_f(matrix1->rows, matrix2->cols);
            ae_mat_f new_matrix1 = resize_temp_ae_mat_f(matrix1, dim, dim);
            ae_mat_f new_matrix2 = resize_temp_ae_mat_f(matrix2, dim, dim);

            ae_mat_f result = create_temp_ae_mat_f(dim, dim);

            result = strassen_mult_by_mat_ae_mat_f(&result, &new_matrix1, &new_matrix2);

            for (uint32_t i = 0; i < new_matrix.rows; i++)
                for (uint32_t j = 0; j < new_matrix.cols; j++)
                    AE_MATRIX_ELEM(new_matrix, i, j) = AE_MATRIX_ELEM(result, i, j);

            free(result.data);
            free(new_matrix1.data);
            free(new_matrix2.data);

            if (m_matrix != NULL)
            {
                free(m_matrix->data);
                if (m_matrix->gc_idx != -1)
                {
                    new_matrix.gc_idx = m_matrix->gc_idx;
                    update_ae_gc(m_matrix->gc_idx, new_matrix.data);
                }
            }

            return new_matrix;
        }
        else if (u1 == 0)
        {
            ae_mat_f new_matrix;
            if (m_matrix == NULL)
                new_matrix = create_ae_mat_f(matrix1->rows, matrix2->cols);
            else
                new_matrix = create_temp_ae_mat_f(matrix1->rows, matrix2->cols);
            ae_mat_f new_matrix1 = resize_temp_ae_mat_f(matrix1, dim, dim);

            ae_mat_f result = create_temp_ae_mat_f(dim, dim);

            result = strassen_mult_by_mat_ae_mat_f(&result, &new_matrix1, matrix2);

            for (uint32_t i = 0; i < new_matrix.rows; i++)
                for (uint32_t j = 0; j < new_matrix.cols; j++)
                    AE_MATRIX_ELEM(new_matrix, i, j) = AE_MATRIX_ELEM(result, i, j);

            free(result.data);
            free(new_matrix1.data);

            if (m_matrix != NULL)
            {
                free(m_matrix->data);
                if (m_matrix->gc_idx != -1)
                {
                    new_matrix.gc_idx = m_matrix->gc_idx;
                    update_ae_gc(m_matrix->gc_idx, new_matrix.data);
                }
            }

            return new_matrix;
        }
        else if (u2 == 0)
        {
            ae_mat_f new_matrix;
            if (m_matrix == NULL)
                new_matrix = create_ae_mat_f(matrix1->rows, matrix2->cols);
            else
                new_matrix = create_temp_ae_mat_f(matrix1->rows, matrix2->cols);
            ae_mat_f new_matrix2 = resize_temp_ae_mat_f(matrix2, dim, dim);

            ae_mat_f result = create_temp_ae_mat_f(dim, dim);

            result = strassen_mult_by_mat_ae_mat_f(&result, matrix1, &new_matrix2);

            for (uint32_t i = 0; i < new_matrix.rows; i++)
                for (uint32_t j = 0; j < new_matrix.cols; j++)
                    AE_MATRIX_ELEM(new_matrix, i, j) = AE_MATRIX_ELEM(result, i, j);

            free(result.data);
            free(new_matrix2.data);

            if (m_matrix != NULL)
            {
                free(m_matrix->data);
                if (m_matrix->gc_idx != -1)
                {
                    new_matrix.gc_idx = m_matrix->gc_idx;
                    update_ae_gc(m_matrix->gc_idx, new_matrix.data);
                }
            }

            return new_matrix;
        }
        else
        {
            return strassen_mult_by_mat_ae_mat_f(m_matrix, matrix1, matrix2);
        }
    }
}

ae_mat_f transpose_ae_mat_f(ae_mat_f *const m_matrix, const ae_mat_f *const matrix)
{
    ae_mat_f new_matrix;
    if (m_matrix == NULL)
        new_matrix = create_ae_mat_f(matrix->cols, matrix->rows);
    else
        new_matrix = create_temp_ae_mat_f(matrix->cols, matrix->rows);

    for (uint32_t i = 0; i < new_matrix.rows; i++)
    {
        for (uint32_t j = 0; j < new_matrix.cols; j++)
        {
            AE_MATRIX_ELEM(new_matrix, i, j) = AE_MATRIX_ELEM(new_matrix, j, i);
        }
    }

    if (m_matrix != NULL)
    {
        free(m_matrix->data);
        if (m_matrix->gc_idx != -1)
        {
            new_matrix.gc_idx = m_matrix->gc_idx;
            update_ae_gc(m_matrix->gc_idx, new_matrix.data);
        }
    }

    return new_matrix;
}

ae_mat_f invert_ae_mat_f(ae_mat_f *const m_matrix, const ae_mat_f *const matrix)
{
    assert(matrix->rows == matrix->cols);

    ae_mat_f new_matrix;
    if (m_matrix == NULL)
        new_matrix = create_ae_mat_f(matrix->rows, matrix->cols);
    else
        new_matrix = create_temp_ae_mat_f(matrix->rows, matrix->cols);

    ae_mat_f temp = create_temp_ae_mat_f(matrix->rows, matrix->cols * 2);

    for (uint32_t i = 0; i < matrix->rows; i++)
        for (uint32_t j = 0; j < matrix->cols; j++)
        {
            AE_MATRIX_ELEM(temp, i, j) = AE_MATRIX_P_ELEM(matrix, i, j);
            AE_MATRIX_ELEM(temp, i, j + matrix->cols) = i == j ? 1.0 : 0.0;
        }

    for (uint32_t i = 0; i < matrix->rows - 1; i++)
    {
        /* if AE_MATRIX_ELEM(temp, i, i) ~= 1 */
        if (AE_FLOAT_PRECISION != (int64_t)(AE_MATRIX_ELEM(temp, i, i) * AE_FLOAT_PRECISION))
            for (int32_t j = temp.cols - 1; j >= 0; j--)
                AE_MATRIX_ELEM(temp, i, j) /= AE_MATRIX_ELEM(temp, i, i);

        for (uint32_t k = i + 1; k < matrix->rows; k++)
        {
            double coeff = AE_MATRIX_ELEM(temp, k, i);
            for (uint32_t j = 0; j < temp.cols; j++)
                AE_MATRIX_ELEM(temp, k, j) -= AE_MATRIX_ELEM(temp, i, j) * coeff;
        }
    }

    /* if temp.data[matrix->rows - 1 + temp.cols * (matrix->rows - 1)] ~= 1 */
    if (AE_FLOAT_PRECISION != (int64_t)(AE_MATRIX_ELEM(temp, matrix->rows - 1, matrix->rows - 1) * AE_FLOAT_PRECISION))
        for (int32_t j = temp.cols - 1; j >= matrix->rows - 1; j--)
            AE_MATRIX_ELEM(temp, matrix->rows - 1, j) /= AE_MATRIX_ELEM(temp, matrix->rows - 1, matrix->rows - 1);

    for (int32_t i = matrix->rows - 1; i > 0; i--)
        for (int32_t k = i - 1; k >= 0; k--)
        {
            double coeff = AE_MATRIX_ELEM(temp, k, i);
            for (uint32_t j = 0; j < temp.cols; j++)
                AE_MATRIX_ELEM(temp, k, j) -= AE_MATRIX_ELEM(temp, i, j) * coeff;
        }

    for (uint32_t i = 0; i < matrix->rows; i++)
        for (uint32_t j = 0; j < matrix->cols; j++)
            AE_MATRIX_ELEM(new_matrix, i, j) = AE_MATRIX_ELEM(temp, i, j + matrix->cols);

    free(temp.data);

    if (m_matrix != NULL)
    {
        free(m_matrix->data);
        if (m_matrix->gc_idx != -1)
        {
            new_matrix.gc_idx = m_matrix->gc_idx;
            update_ae_gc(m_matrix->gc_idx, new_matrix.data);
        }
    }

    return new_matrix;
}

ae_mat_f invert_transpose_ae_mat_f(ae_mat_f *const m_matrix, const ae_mat_f *const matrix)
{
    assert(matrix->rows == matrix->cols);

    ae_mat_f new_matrix;
    if (m_matrix == NULL)
        new_matrix = create_ae_mat_f(matrix->rows, matrix->cols);
    else
        new_matrix = create_temp_ae_mat_f(matrix->rows, matrix->cols);

    ae_mat_f temp = create_temp_ae_mat_f(matrix->rows, matrix->cols * 2);

    for (uint32_t i = 0; i < matrix->rows; i++)
        for (uint32_t j = 0; j < matrix->cols; j++)
        {
            AE_MATRIX_ELEM(temp, i, j) = AE_MATRIX_P_ELEM(matrix, i, j);
            AE_MATRIX_ELEM(temp, i, j + matrix->cols) = i == j ? 1.0 : 0.0;
        }

    for (uint32_t i = 0; i < matrix->rows - 1; i++)
    {
        /* if AE_MATRIX_ELEM(temp, i, i) ~= 1 */
        if (AE_FLOAT_PRECISION != (int64_t)(AE_MATRIX_ELEM(temp, i, i) * AE_FLOAT_PRECISION))
            for (int32_t j = temp.cols - 1; j >= 0; j--)
                AE_MATRIX_ELEM(temp, i, j) /= AE_MATRIX_ELEM(temp, i, i);

        for (uint32_t k = i + 1; k < matrix->rows; k++)
        {
            double coeff = AE_MATRIX_ELEM(temp, k, i);
            for (uint32_t j = 0; j < temp.cols; j++)
                AE_MATRIX_ELEM(temp, k, j) -= AE_MATRIX_ELEM(temp, i, j) * coeff;
        }
    }

    /* if temp.data[matrix->rows - 1 + temp.cols * (matrix->rows - 1)] ~= 1 */
    if (AE_FLOAT_PRECISION != (int64_t)(AE_MATRIX_ELEM(temp, matrix->rows - 1, matrix->rows - 1) * AE_FLOAT_PRECISION))
        for (int32_t j = temp.cols - 1; j >= matrix->rows - 1; j--)
            AE_MATRIX_ELEM(temp, matrix->rows - 1, j) /= AE_MATRIX_ELEM(temp, matrix->rows - 1, matrix->rows - 1);

    for (int32_t i = matrix->rows - 1; i > 0; i--)
        for (int32_t k = i - 1; k >= 0; k--)
        {
            double coeff = AE_MATRIX_ELEM(temp, k, i);
            for (uint32_t j = 0; j < temp.cols; j++)
                AE_MATRIX_ELEM(temp, k, j) -= AE_MATRIX_ELEM(temp, i, j) * coeff;
        }

    for (uint32_t i = 0; i < matrix->rows; i++)
        for (uint32_t j = 0; j < matrix->cols; j++)
            AE_MATRIX_ELEM(new_matrix, i, j) = AE_MATRIX_ELEM(temp, j + matrix->cols, i);

    free(temp.data);

    if (m_matrix != NULL)
    {
        free(m_matrix->data);
        if (m_matrix->gc_idx != -1)
        {
            new_matrix.gc_idx = m_matrix->gc_idx;
            update_ae_gc(m_matrix->gc_idx, new_matrix.data);
        }
    }

    return new_matrix;
}

double det_ae_mat_f(const ae_mat_f *const matrix)
{
    assert(matrix->rows == matrix->cols);

    if (matrix->rows == 2)
    {
        return AE_MATRIX_P_ELEM(matrix, 0, 0) * AE_MATRIX_P_ELEM(matrix, 1, 1) - AE_MATRIX_P_ELEM(matrix, 0, 1) * AE_MATRIX_P_ELEM(matrix, 1, 0);
    }
    else
    {
        ae_mat_f new_matrix = create_temp_ae_mat_f(matrix->rows, matrix->cols);

        memcpy(new_matrix.data, matrix->data, matrix->rows * matrix->cols * sizeof(double));

        for (uint32_t i = 0; i < matrix->cols; i++)
        {
            if ((int64_t)(AE_MATRIX_ELEM(new_matrix, i, i) * AE_FLOAT_PRECISION) == 0)
            {
                uint32_t k = 1;
                for (; k < matrix->cols; k++)
                {
                    int32_t new_i = (k + i) % matrix->cols;
                    if ((int64_t)(AE_MATRIX_ELEM(new_matrix, new_i, i) * AE_FLOAT_PRECISION) != 0)
                    {
                        for (uint32_t u = 0; u < matrix->cols; u++)
                            AE_MATRIX_ELEM(new_matrix, i, u) += AE_MATRIX_ELEM(new_matrix, new_i, u);
                        break;
                    }
                }
                if (k == matrix->cols)
                    return 0;
            }
            for (uint32_t j = i + 1; j < matrix->cols; j++)
            {
                double coeff = AE_MATRIX_ELEM(new_matrix, j, i) / AE_MATRIX_ELEM(new_matrix, i, i);
                for (uint32_t k = 0; k < matrix->cols; k++)
                {
                    AE_MATRIX_ELEM(new_matrix, j, k) -= coeff * AE_MATRIX_ELEM(new_matrix, i, k);
                }
            }
        }

        double det = 1.0;
        for (uint32_t i = 0; i < matrix->cols; i++)
        {
            det *= AE_MATRIX_ELEM(new_matrix, i, i);
        }

        free(new_matrix.data);

        return det;
    }
}

ae_mat_f adjugate_ae_mat_f(ae_mat_f *const m_matrix, const ae_mat_f *const matrix)
{
    assert(matrix->rows == matrix->cols);

    ae_mat_f new_matrix;
    if (m_matrix == NULL)
        new_matrix = create_ae_mat_f(matrix->rows, matrix->cols);
    else
        new_matrix = create_temp_ae_mat_f(matrix->rows, matrix->cols);

    ae_mat_f temp = create_temp_ae_mat_f(matrix->rows - 1, matrix->cols - 1);

    int32_t x, y;
    for (uint32_t i = 0; i < matrix->rows; i++)
    {
        for (uint32_t j = 0; j < matrix->cols; j++)
        {
            x = 0;
            for (uint32_t i_t = 0; i_t < matrix->rows; i_t++)
            {
                if (i == i_t)
                    continue;
                y = 0;
                for (uint32_t j_t = 0; j_t < matrix->cols; j_t++)
                {
                    if (j == j_t)
                        continue;
                    AE_MATRIX_ELEM(temp, x, y) = AE_MATRIX_P_ELEM(matrix, i_t, j_t);
                    y++;
                }
                x++;
            }
            AE_MATRIX_ELEM(new_matrix, i, j) = ((i + j) % 2 == 0 ? 1 : -1) * det_ae_mat_f(&temp);
        }
    }

    free(temp.data);

    if (m_matrix != NULL)
    {
        free(m_matrix->data);
        if (m_matrix->gc_idx != -1)
        {
            new_matrix.gc_idx = m_matrix->gc_idx;
            update_ae_gc(m_matrix->gc_idx, new_matrix.data);
        }
    }

    return new_matrix;
}
