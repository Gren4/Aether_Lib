#include "aether_matrix.h"
#include "aether_gc.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

ae_matrix_f create_ae_matrix_f(size_t rows, size_t cols)
{
    assert(rows > 0 && cols > 0);
    ae_matrix_f new_matrix = {.data = calloc(rows * cols, sizeof(double)),
                              .rows = rows,
                              .cols = cols};
    assert(new_matrix.data != NULL);
#ifdef DEBUG_AE
    printf("Allocated memory at address %p\n", new_matrix.data);
#endif
    new_matrix.gc_idx = append_ae_gc((void*)new_matrix.data);

    return new_matrix;
}

void free_ae_matrix_f(ae_matrix_f *matrix)
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

ae_matrix_f identity_ae_matrix_f(size_t dim)
{
    assert(dim > 0);
    ae_matrix_f new_matrix = {.data = calloc(dim * dim, sizeof(double)),
                              .rows = dim,
                              .cols = dim};
    assert(new_matrix.data != NULL);
    for (size_t i = 0; i < dim; i++)
    {
        for (size_t j = 0; j < dim; j++)
        {
            new_matrix.data[j + new_matrix.cols * i] = i == j ? 1.0 : 0.0;
        }
    }
    return new_matrix;
}

double get_ae_matrix_f(ae_matrix_f const *matrix, size_t row, size_t col)
{
    assert(col + matrix->cols * row < matrix->cols * matrix->rows);

    return matrix->data[col + matrix->cols * row];
}

void set_ae_matrix_f(ae_matrix_f *matrix, size_t row, size_t col, const double par)
{
    assert(col + matrix->cols * row < matrix->cols * matrix->rows);

    matrix->data[col + matrix->cols * row] = par;

    return;
}

ae_matrix_f multiply_by_matrix_ae_matrix_f(ae_matrix_f const *matrix1, ae_matrix_f const *matrix2)
{
    assert(matrix1->cols == matrix2->rows);
    ae_matrix_f new_matrix = create_ae_matrix_f(matrix1->rows, matrix2->cols);

    for (size_t i = 0; i < new_matrix.rows; i++)
    {
        for (size_t j = 0; j < new_matrix.cols; j++)
        {
            new_matrix.data[j + new_matrix.cols * i] = 0.0;
            for (size_t k = 0; k < matrix1->cols; k++)
                new_matrix.data[j + new_matrix.cols * i] += matrix1->data[k + matrix1->cols * i] * matrix2->data[j + matrix2->cols * k];
        }
    }

    return new_matrix;
}

ae_matrix_f transpose_ae_matrix_f(ae_matrix_f const *matrix)
{
    ae_matrix_f new_matrix = create_ae_matrix_f(matrix->cols, matrix->rows);

    for (size_t i = 0; i < new_matrix.rows; i++)
    {
        for (size_t j = 0; j < new_matrix.cols; j++)
        {
            new_matrix.data[j + new_matrix.cols * i] = matrix->data[i + matrix->cols * j];
        }
    }

    return new_matrix;
}

ae_matrix_f invert_ae_matrix_f(ae_matrix_f const *matrix)
{
    assert(matrix->rows == matrix->cols);

    ae_matrix_f new_matrix = create_ae_matrix_f(matrix->rows, matrix->cols);
    ae_matrix_f temp = create_ae_matrix_f(matrix->rows, matrix->cols * 2);

    for (size_t i = 0; i < matrix->rows; i++)
        for (size_t j = 0; j < matrix->cols; j++)
        {
            temp.data[j + temp.cols * i] = matrix->data[j + matrix->cols * i];
            temp.data[j + matrix->cols + temp.cols * i] = i == j ? 1.0 : 0.0;
        }

    for (size_t i = 0; i < matrix->rows - 1; i++)
    {
        for (size_t j = temp.cols; j > 0; j--)
            temp.data[j - 1 + temp.cols * i] /= temp.data[i + temp.cols * i];
        for (size_t k = i + 1; k < matrix->rows; k++)
        {
            double coeff = temp.data[i + temp.cols * k];
            for (size_t j = 0; j < temp.cols; j++)
                temp.data[j + temp.cols * k] -= temp.data[j + temp.cols * i] * coeff;
        }
    }

    for (size_t j = temp.cols; j >= matrix->rows; j--)
        temp.data[j - 1 + temp.cols * (matrix->rows - 1)] /= temp.data[matrix->rows - 1 + temp.cols * (matrix->rows - 1)];

    for (size_t i = matrix->rows - 1; i > 0; i--)
        for (size_t k = i; k > 0; k--)
        {
            double coeff = temp.data[i + temp.cols * (k - 1)];
            for (size_t j = 0; j < temp.cols; j++)
                temp.data[j + temp.cols * (k - 1)] -= temp.data[j + temp.cols * i] * coeff;
        }
    for (size_t i = 0; i < matrix->rows; i++)
        for (size_t j = 0; j < matrix->cols; j++)
            new_matrix.data[j + new_matrix.cols * i] = temp.data[j + matrix->cols + temp.cols * i];

    return new_matrix;
}
