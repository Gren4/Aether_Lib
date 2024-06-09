#include "aether_convolution.h"

aether_layer init_ae_conv2d(const aether_nn_config *const config, void *layer)
{
    aether_conv2d *conv = (aether_conv2d *)layer;

    aether_layer wrapped_layer = {
        .input = NULL,
        .output = NULL,
        .gradient_in = NULL,
        .gradient_out = NULL,
        .layer = NULL,
        .layer_forward = NULL,
        .layer_backward = NULL,
        .layer_free = NULL};

    conv->input = NULL;
    conv->gradient_in = NULL;

    create_ae_tensor(&conv->output, 0, 0, 0, 0);
    create_ae_tensor(&conv->gradient_out, 0, 0, 0, 0);

    uint32_t bias_offset = conv->out_channels * sizeof(float);
    if (config->is_train == true)
    {
        void *p = calloc(2, bias_offset);
        if (p == NULL)
            return wrapped_layer;
#ifdef DEBUG_AE
        printf("Allocated memory at address %p\n", p);
#endif
        conv->gc_idx = append_ae_gc(p);

        conv->bias = p;
        conv->d_bias = (void *)conv->bias + bias_offset;

        for (uint32_t i = 0; i < conv->out_channels; i++)
        {
            conv->bias[i] = 0.01f;
            conv->d_bias[i] = 0.0f;
        }

        create_random_ae_tensor(&conv->weights, conv->out_channels, conv->in_channels, conv->kernel_size[0], conv->kernel_size[1]);
        create_zero_ae_tensor(&conv->d_weights, conv->out_channels, conv->in_channels, conv->kernel_size[0], conv->kernel_size[1]);

        create_ae_tensor(&conv->af_cache, 0, 0, 0, 0);
    }
    else
    {
        void *p = calloc(1, bias_offset);
        if (p == NULL)
            return wrapped_layer;
#ifdef DEBUG_AE
        printf("Allocated memory at address %p\n", p);
#endif
        conv->gc_idx = append_ae_gc(p);

        conv->bias = p;

        for (uint32_t i = 0; i < conv->out_channels; i++)
        {
            conv->bias[i] = 0.01f;
        }

        create_random_ae_tensor(&conv->weights, conv->out_channels, conv->in_channels, conv->kernel_size[0], conv->kernel_size[1]);
        create_empty_ae_tensor(&conv->d_weights, 0, 0, 0, 0);

        create_empty_ae_tensor(&conv->af_cache, 0, 0, 0, 0);
    }

    wrapped_layer.input = &conv->input,
    wrapped_layer.output = &conv->output,
    wrapped_layer.gradient_in = &conv->gradient_in,
    wrapped_layer.gradient_out = &conv->gradient_out,
    wrapped_layer.layer = layer,
    wrapped_layer.layer_forward = forward_ae_conv2d,
    wrapped_layer.layer_backward = backward_ae_conv2d,
    wrapped_layer.layer_free = free_ae_conv2d;

    return wrapped_layer;
}

void free_ae_conv2d(void *layer)
{
    if (layer == NULL)
        return;

    aether_conv2d *conv = (aether_conv2d *)layer;

    free_ae_tensor(&conv->af_cache);

    free_ae_tensor(&conv->d_weights);

    free_ae_tensor(&conv->weights);

    if (conv->bias != NULL)
    {
#ifdef DEBUG_AE
        printf("Freed memory at address %p\n", conv->bias);
#endif
        free(conv->bias);
    }

    remove_ae_gc(conv->gc_idx);

    free_ae_tensor(&conv->gradient_out);

    free_ae_tensor(&conv->output);

    return;
}

void forward_ae_conv2d(const aether_nn_config *const config, void *layer)
{
    aether_conv2d *conv = (aether_conv2d *)layer;

    const uint32_t height_in = conv->input->height;
    const uint32_t width_in = conv->input->width;

    const uint32_t height_out = ((height_in - conv->dilation[0] * (conv->kernel_size[0] - 1) + 2 * conv->padding[0] - 1) / conv->stride[0]) + 1;
    const uint32_t width_out = ((width_in - conv->dilation[1] * (conv->kernel_size[1] - 1) + 2 * conv->padding[1] - 1) / conv->stride[1]) + 1;

    if (check_dims_ae_tensor(&conv->output, 1, conv->out_channels, height_out, width_out) == false)
        update_ae_tensor(&conv->output, 1, conv->out_channels, height_out, width_out);

    if (config->is_train)
        if (check_dims_ae_tensor(&conv->af_cache, 1, conv->out_channels, height_out, width_out) == false)
            update_ae_tensor(&conv->af_cache, 1, conv->out_channels, height_out, width_out);

    float sum = 0.0f;
    for (uint32_t f = 0; f < conv->out_channels; f++)
    {
        for (uint32_t h = 0; h < height_out; h++)
        {
            for (uint32_t w = 0; w < width_out; w++)
            {
                sum = conv->bias[f];
                for (uint32_t c = 0; c < conv->in_channels; c++)
                {
                    for (uint32_t h_f = 0; h_f < conv->kernel_size[0]; h_f++)
                    {
                        uint32_t h_tmp = h * conv->stride[0] + h_f * conv->dilation[0] - conv->padding[0];
                        if (h_tmp >= 0 && h_tmp < height_in)
                        {
                            for (uint32_t w_f = 0; w_f < conv->kernel_size[1]; w_f++)
                            {
                                uint32_t w_tmp = w * conv->stride[1] + w_f * conv->dilation[1] - conv->padding[1];

                                if (w_tmp >= 0 && w_tmp < width_in)
                                {
                                    sum += (*get_element_ae_tensor(conv->input, 0, c, h_tmp, w_tmp)) * (*get_element_ae_tensor(&conv->weights, f, c, h_f, w_f));
                                }
                            }
                        }
                    }
                }
                if (config->is_train)
                    *get_element_ae_tensor(&conv->af_cache, 0, f, h, w) = sum;
                *get_element_ae_tensor(&conv->output, 0, f, h, w) = conv->activation_function(FORWARD, &sum);
            }
        }
    }

    return;
}

void backward_ae_conv2d(const aether_nn_config *const config, void *layer)
{
    aether_conv2d *conv = (aether_conv2d *)layer;

    const uint32_t height_in = conv->input->height;
    const uint32_t width_in = conv->input->width;

    const uint32_t height_out = ((height_in - conv->dilation[0] * (conv->kernel_size[0] - 1) + 2 * conv->padding[0] - 1) / conv->stride[0]) + 1;
    const uint32_t width_out = ((width_in - conv->dilation[1] * (conv->kernel_size[1] - 1) + 2 * conv->padding[1] - 1) / conv->stride[1]) + 1;

    if (check_dims_ae_tensor(&conv->gradient_out, 1, conv->in_channels, height_in, width_in) == false)
        update_ae_tensor(&conv->gradient_out, 1, conv->in_channels, height_in, width_in);

    float sum = 0.0f;
    for (uint32_t f = 0; f < conv->out_channels; f++)
    {
        for (uint32_t h = 0; h < height_out; h++)
        {
            for (uint32_t w = 0; w < width_out; w++)
            {
                (*get_element_ae_tensor(conv->gradient_in, 0, f, h, w)) *= conv->activation_function(BACKWARD, get_element_ae_tensor(&conv->af_cache, 0, f, h, w));
                for (uint32_t c = 0; c < conv->in_channels; c++)
                {
                    for (uint32_t h_f = 0; h_f < conv->kernel_size[0]; h_f++)
                    {
                        uint32_t h_tmp = h * conv->stride[0] + h_f * conv->dilation[0] - conv->padding[0];
                        for (uint32_t w_f = 0; w_f < conv->kernel_size[1]; w_f++)
                        {
                            uint32_t w_tmp = w * conv->stride[0] + w_f * conv->dilation[1] - conv->padding[1];
                            if (h_tmp >= 0 && h_tmp < height_in && w_tmp >= 0 && w_tmp < width_in)
                            {
                                *get_element_ae_tensor(&conv->d_weights, f, c, h_f, w_f) += (*get_element_ae_tensor(conv->input, 0, c, h_tmp, w_tmp)) * (*get_element_ae_tensor(conv->gradient_in, 0, f, h, w));
                            }
                        }
                    }
                }
                conv->d_bias[f] += (*get_element_ae_tensor(conv->gradient_in, 0, f, h, w));
            }
        }
    }

    for (uint32_t c = 0; c < conv->in_channels; c++)
    {
        for (uint32_t h = 0; h < height_in; h++)
        {
            for (uint32_t w = 0; w < width_in; w++)
            {
                sum = 0.0f;
                for (uint32_t f = 0; f < conv->out_channels; f++)
                {
                    for (uint32_t h_f = 0; h_f < conv->kernel_size[0]; h_f++)
                    {
                        uint32_t h_tmp = h + h_f * conv->dilation[0] + conv->padding[0] - conv->kernel_size[0] + 1;
                        if (h_tmp % conv->stride[0] == 0)
                        {
                            h_tmp /= conv->stride[0];
                            for (uint32_t w_f = 0; w_f < conv->kernel_size[1]; w_f++)
                            {
                                uint32_t w_tmp = w + w_f * conv->dilation[1] + conv->padding[1] - conv->kernel_size[1] + 1;
                                if (w_tmp % conv->stride[1] == 0)
                                {
                                    w_tmp /= conv->stride[1];
                                    if (h_tmp >= 0 && h_tmp < height_out && w_tmp >= 0 && w_tmp < width_out)
                                    {
                                        sum += *get_element_ae_tensor(&conv->weights, f, c, conv->kernel_size[0] - 1 - h_f, conv->kernel_size[1] - 1 - w_f) *
                                               (*get_element_ae_tensor(conv->gradient_in, 0, f, h_tmp, w_tmp));
                                    }
                                }
                            }
                        }
                    }
                }
                *get_element_ae_tensor(&conv->gradient_out, 0, c, h, w) = sum;
            }
        }
    }

    for (uint32_t f = 0; f < conv->out_channels; f++)
    {
        for (uint32_t c = 0; c < conv->in_channels; c++)
        {
            for (uint32_t h_f = 0; h_f < conv->kernel_size[0]; h_f++)
            {
                for (uint32_t w_f = 0; w_f < conv->kernel_size[1]; w_f++)
                {
                    *get_element_ae_tensor(&conv->weights, f, c, h_f, w_f) -= config->learning_rate * (*get_element_ae_tensor(&conv->d_weights, f, c, h_f, w_f));
                    *get_element_ae_tensor(&conv->d_weights, f, c, h_f, w_f) = 0.0f;
                }
            }
        }
        conv->bias[f] -= config->learning_rate * conv->d_bias[f];
        conv->d_bias[f] = 0.0f;
    }

    return;
}
