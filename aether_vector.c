#include "aether_vector.h"

ae_vector *f_create_ae_vector(size_t data_size, uint8_t *status)
{
    if (status != NULL)
        *status = AETHER_VECTOR_ERROR;

    ae_vector *vector = malloc(sizeof(ae_vector));
    if (vector == NULL)
        return NULL;

    vector->data = calloc(AETHER_VECTOR_ADD_SIZE, data_size);
    if (vector->data == NULL)
        return NULL;

    vector->io_buffer = malloc(data_size);
    if (vector->io_buffer == NULL)
    {
        free(vector->data);
        return NULL;
    }

    vector->data_size = data_size;
    vector->quantity = 0;

    if (status != NULL)
        *status = AETHER_VECTOR_SUCCESS;
    return vector;
}

void *f_free_ae_vector(ae_vector *vector, uint8_t *status)
{
    if (status != NULL)
        *status = AETHER_VECTOR_ERROR;

    if (vector != NULL)
    {
        if (vector->data != NULL)
            free(vector->data);

        if (vector->io_buffer != NULL)
            free(vector->io_buffer);

        free(vector);
    }

    if (status != NULL)
        *status = AETHER_VECTOR_SUCCESS;
    return NULL;
}

void f_resize_ae_vector(ae_vector *vector, size_t new_size, uint8_t *status)
{
    if (status != NULL)
        *status = AETHER_VECTOR_ERROR;

    if (vector == NULL)
        return;

    if (vector->data == NULL)
        return;
    
    if (new_size < 0)
        return;

    if (new_size == vector->quantity)
    {
        if (status != NULL)
            *status = AETHER_VECTOR_SUCCESS;
        return;
    }

    void *p = realloc(vector->data, (((new_size / AETHER_VECTOR_ADD_SIZE) + 1) * AETHER_VECTOR_ADD_SIZE) * vector->data_size);

    if (p == NULL)
        return;
    vector->data = p;

    if (new_size > vector->quantity)
        memset(vector->data + vector->quantity * vector->data_size, 0, (new_size - vector->quantity) * vector->data_size);

    vector->quantity = new_size;

    if (status != NULL)
        *status = AETHER_VECTOR_SUCCESS;
    return;
}

void f_append_ae_vector(ae_vector *vector, uint8_t *status)
{
    if (status != NULL)
        *status = AETHER_VECTOR_ERROR;

    if (vector == NULL)
        return;

    if (vector->data == NULL)
        return;
    
    if ((vector->quantity + 1) % AETHER_VECTOR_ADD_SIZE == 0)
    {
        void *p = realloc(vector->data, ((vector->quantity + 1) + AETHER_VECTOR_ADD_SIZE) * vector->data_size);
        if (p == NULL)
            return;

        vector->data = p;
    }

    memmove(vector->data + vector->quantity * vector->data_size, vector->io_buffer, vector->data_size);

    vector->quantity++;

    if (status != NULL)
        *status = AETHER_VECTOR_SUCCESS;
    return;
}

void f_set_ae_vector(ae_vector *vector, size_t i, uint8_t *status)
{
    if (status != NULL)
        *status = AETHER_VECTOR_ERROR;

    if (vector == NULL)
        return;

    if (vector->data == NULL)
        return;
    
    if (i < 0 || i >= vector->quantity)
        return;

    memmove(vector->data + i * vector->data_size, vector->io_buffer, vector->data_size);

    if (status != NULL)
        *status = AETHER_VECTOR_SUCCESS;
    return;
}

void *f_get_ae_vector(ae_vector *vector, size_t i, uint8_t *status)
{
    if (status != NULL)
        *status = AETHER_VECTOR_ERROR;

    if (vector == NULL)
        return NULL;

    if (vector->data == NULL)
        return NULL;
    
    if (i < 0 || i >= vector->quantity)
        return NULL;
    
    if (status != NULL)
        *status = AETHER_VECTOR_SUCCESS;
    return vector->data + i * vector->data_size;
    
}

void f_concat_ae_vector(ae_vector *vector_to, ae_vector *vector_from, uint8_t *status)
{
    if (status != NULL)
        *status = AETHER_VECTOR_ERROR;

    if (vector_to == NULL)
        return;

    if (vector_to->data == NULL)
        return;

    if (vector_from == NULL)
        return;

    if (vector_from->data != NULL)
        return;

    if (vector_to->data_size != vector_from->data_size)
        return;        

    size_t new_size = vector_to->quantity + vector_from->quantity;
    void *p = realloc(vector_to->data, (((new_size / AETHER_VECTOR_ADD_SIZE) + 1) * AETHER_VECTOR_ADD_SIZE) * vector_to->data_size);
    if (p == NULL)
        return;
    
    vector_to->data = p;

    memmove(vector_to->data + vector_to->quantity * vector_to->data_size, vector_from->data, vector_from->quantity * vector_to->data_size);

    vector_to->quantity = new_size;

    if (status != NULL)
        *status = AETHER_VECTOR_SUCCESS;
    return;
}

void f_insert_ae_vector(ae_vector *vector, size_t i, uint8_t *status)
{
    if (status != NULL)
        *status = AETHER_VECTOR_ERROR;

    if (vector == NULL)
        return;

    if (vector->data != NULL)
        return;
        
    if ((vector->quantity + 1) % AETHER_VECTOR_ADD_SIZE == 0)
    {
        void *p = realloc(vector->data, ((vector->quantity + 1) + AETHER_VECTOR_ADD_SIZE) * vector->data_size);
        if (p == NULL)
            return;

        vector->data = p;
    }

    memmove(vector->data + (i + 1) * vector->data_size, vector->data + i * vector->data_size, (vector->quantity - i) * vector->data_size);
    memmove(vector->data + i * vector->data_size, vector->io_buffer, vector->data_size);

    vector->quantity++;

    if (status != NULL)
        *status = AETHER_VECTOR_SUCCESS;
    return;
}

void *f_delete_ae_vector(ae_vector *vector, size_t i, uint8_t *status)
{
    if (status != NULL)
        *status = AETHER_VECTOR_ERROR;

    if (vector == NULL)
        return NULL;

    if (vector->data == NULL)
        return NULL;

    if ((vector->quantity + 1) % AETHER_VECTOR_ADD_SIZE == 0)
    {
        void *p = realloc(vector->data, ((vector->quantity + 1) + AETHER_VECTOR_ADD_SIZE) * vector->data_size);
        if (p == NULL)
            return NULL;
        
        vector->data = p;
    }

    if (vector->io_buffer != NULL)
        memmove(vector->io_buffer, vector->data + i * vector->data_size, vector->data_size);

    memmove(vector->data + i * vector->data_size, vector->data + (i + 1) * vector->data_size, (vector->quantity - i) * vector->data_size);

    vector->quantity--;

    if (status != NULL)
        *status = AETHER_VECTOR_SUCCESS;
    return vector->io_buffer;
}

ae_vector *f_duplicate_ae_vector(ae_vector *vector, uint8_t *status)
{
    if (status != NULL)
        *status = AETHER_VECTOR_ERROR;

    if (vector == NULL)
        return NULL;

    ae_vector *duplicated_vector = f_create_ae_vector(vector->data_size, status);
    f_resize_ae_vector(duplicated_vector, vector->quantity, status);

    if (vector->data != NULL)
        memmove(duplicated_vector->data, vector->data, vector->quantity * vector->data_size);

    if (status != NULL)
        *status = AETHER_VECTOR_SUCCESS;
    return duplicated_vector;
}

void f_invert_ae_vector(ae_vector *vector, uint8_t *status)
{
    if (status != NULL)
        *status = AETHER_VECTOR_ERROR;

    if (vector == NULL)
        return;
    
    if (vector->data == NULL)
        return;
        
    if (vector->quantity > 1)
    {
        size_t iterations = vector->quantity / 2;
        size_t i = 0;
        for (i = 0; i < iterations; i++)
        {
            memmove(vector->io_buffer, vector->data + i * vector->data_size, vector->data_size);
            memmove(vector->data + i * vector->data_size, vector->data + (vector->quantity - 1 - i) * vector->data_size, vector->data_size);
            memmove(vector->data + (vector->quantity - 1 - i) * vector->data_size, vector->io_buffer, vector->data_size);
        }
    }

    if (status != NULL)
        *status = AETHER_VECTOR_SUCCESS;
    return;
}