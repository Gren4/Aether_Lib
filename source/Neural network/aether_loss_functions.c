#include "aether_loss_functions.h"

float MSE_ae_loss(const aether_tensor * const result, const aether_tensor * const reference, aether_tensor * loss)
{
    if (compare_tensors_ae_tensor(loss, result) == false)
        update_ae_tensor(loss, result->batch, result->filters, result->height, result->width);

    if (compare_tensors_ae_tensor(reference, result) == false)
        return 0;

    float result_loss = 0.0f;

    for(uint32_t f = 0; f < loss->filters; f++)
    {
        for (uint32_t h = 0; h < loss->height; h++)
        {
            for (uint32_t w = 0; w < loss->width; w++)
            {
                *get_element_ae_tensor(loss, 0, f, h, w) = *get_element_ae_tensor(result, 0, f, h, w) - *get_element_ae_tensor(reference, 0, f, h, w);
                result_loss += (*get_element_ae_tensor(loss, 0, f, h, w)) * (*get_element_ae_tensor(loss, 0, f, h, w));
            }
        }
    }

    result_loss /= loss->max_quant;

    if (result_loss > 0.0f)
    {
        for (uint32_t f = 0; f < loss->filters; f++)
        {
            for (uint32_t h = 0; h < loss->height; h++)
            {
                for (uint32_t w = 0; w < loss->width; w++)
                {
                    *get_element_ae_tensor(loss, 0, f, h, w) /= result_loss;
                }
            }
        }
    }

    return result_loss;
}
