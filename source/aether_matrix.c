#include "aether_matrix.h"
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

static inline ae_mat_f create_temporary_ae_mat_f(const uint32_t rows, const uint32_t cols)
{
    assert(rows > 0 && cols > 0);
    ae_mat_f new_matrix = {.data = calloc(rows * cols, sizeof(double)),
                              .rows = rows,
                              .cols = cols};
    assert(new_matrix.data != NULL);
#ifdef DEBUG_AE
    printf("Allocated memory at address %p\n", new_matrix.data);
#endif

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

static inline ae_mat_f identity_temporary_ae_mat_f(const uint32_t dim)
{
    assert(dim > 0);
    ae_mat_f new_matrix = {.data = calloc(dim * dim, sizeof(double)),
                              .rows = dim,
                              .cols = dim};
    assert(new_matrix.data != NULL);
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
        new_matrix = create_temporary_ae_mat_f(matrix1->rows, matrix1->cols);

    for (uint32_t i = 0; i < new_matrix.rows; i++)
        for (uint32_t j = 0; j < new_matrix.cols; j++)
            AE_MATRIX_ELEM(new_matrix, i, j) = AE_MATRIX_P_ELEM(matrix1, i, j) + AE_MATRIX_P_ELEM(matrix2, i, j);

    if (m_matrix != NULL)
    {
        free(m_matrix->data);
        new_matrix.gc_idx = m_matrix->gc_idx;
        update_ae_gc(m_matrix->gc_idx, new_matrix.data);
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
        new_matrix = create_temporary_ae_mat_f(matrix1->rows, matrix1->cols);

    for (uint32_t i = 0; i < new_matrix.rows; i++)
        for (uint32_t j = 0; j < new_matrix.cols; j++)
            AE_MATRIX_ELEM(new_matrix, i, j) = AE_MATRIX_P_ELEM(matrix1, i, j) - AE_MATRIX_P_ELEM(matrix2, i, j);

    if (m_matrix != NULL)
    {
        free(m_matrix->data);
        new_matrix.gc_idx = m_matrix->gc_idx;
        update_ae_gc(m_matrix->gc_idx, new_matrix.data);
    }

    return new_matrix;
}

ae_mat_f mult_by_sclr_ae_mat_f(ae_mat_f *const m_matrix, const ae_mat_f *const matrix, const double scalar)
{
    ae_mat_f new_matrix;
    if (m_matrix == NULL)
        new_matrix = create_ae_mat_f(matrix->rows, matrix->cols);
    else
        new_matrix = create_temporary_ae_mat_f(matrix->rows, matrix->cols);

    for (uint32_t i = 0; i < new_matrix.rows; i++)
        for (uint32_t j = 0; j < new_matrix.cols; j++)
            AE_MATRIX_ELEM(new_matrix, i, j) = AE_MATRIX_P_ELEM(matrix, i, j) * scalar;

    if (m_matrix != NULL)
    {
        free(m_matrix->data);
        new_matrix.gc_idx = m_matrix->gc_idx;
        update_ae_gc(m_matrix->gc_idx, new_matrix.data);
    }

    return new_matrix;
}

ae_mat_f mult_by_mat_ae_mat_f(ae_mat_f *const m_matrix, const ae_mat_f *const matrix1, const ae_mat_f *const matrix2)
{
    assert(matrix1->cols == matrix2->rows);

    ae_mat_f new_matrix;
    if (m_matrix == NULL)
        new_matrix = create_ae_mat_f(matrix1->rows, matrix2->cols);
    else
        new_matrix = create_temporary_ae_mat_f(matrix1->rows, matrix2->cols);

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
        new_matrix.gc_idx = m_matrix->gc_idx;
        update_ae_gc(m_matrix->gc_idx, new_matrix.data);
    }

    return new_matrix;
}

ae_mat_f transpose_ae_mat_f(ae_mat_f *const m_matrix, const ae_mat_f *const matrix)
{
    ae_mat_f new_matrix;
    if (m_matrix == NULL)
        new_matrix = create_ae_mat_f(matrix->cols, matrix->rows);
    else
        new_matrix = create_temporary_ae_mat_f(matrix->cols, matrix->rows);

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
        new_matrix.gc_idx = m_matrix->gc_idx;
        update_ae_gc(m_matrix->gc_idx, new_matrix.data);
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
        new_matrix = create_temporary_ae_mat_f(matrix->rows, matrix->cols);

    ae_mat_f temp = create_temporary_ae_mat_f(matrix->rows, matrix->cols * 2);

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
        new_matrix.gc_idx = m_matrix->gc_idx;
        update_ae_gc(m_matrix->gc_idx, new_matrix.data);
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
        new_matrix = create_temporary_ae_mat_f(matrix->rows, matrix->cols);

    ae_mat_f temp = create_temporary_ae_mat_f(matrix->rows, matrix->cols * 2);

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
        new_matrix.gc_idx = m_matrix->gc_idx;
        update_ae_gc(m_matrix->gc_idx, new_matrix.data);
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
        ae_mat_f new_matrix = create_temporary_ae_mat_f(matrix->rows, matrix->cols);

        memcpy(new_matrix.data,matrix->data, matrix->rows*matrix->cols*sizeof(double));

        for (uint32_t i = 0; i < matrix->cols; i++)
        {
            if ((int64_t)(AE_MATRIX_ELEM(new_matrix, i, i) * AE_FLOAT_PRECISION) == 0)
            {
                uint32_t k = 1;
                for (; k < matrix->cols; k++)
                {
                    int32_t new_i = (k+i) % matrix->cols;
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
        new_matrix = create_temporary_ae_mat_f(matrix->rows, matrix->cols);

    ae_mat_f temp = create_temporary_ae_mat_f(matrix->rows - 1, matrix->cols - 1);

    int32_t x,y;
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
                    AE_MATRIX_ELEM(temp,x,y) = AE_MATRIX_P_ELEM(matrix,i_t,j_t);
                    y++;
                }
                x++;
            }
            AE_MATRIX_ELEM(new_matrix, i, j) = ((i + j) % 2 == 0 ? 1:-1) * det_ae_mat_f(&temp);
        }
    }

    free(temp.data);

    if (m_matrix != NULL)
    {
        free(m_matrix->data);
        new_matrix.gc_idx = m_matrix->gc_idx;
        update_ae_gc(m_matrix->gc_idx, new_matrix.data);
    }

    return new_matrix;
}
