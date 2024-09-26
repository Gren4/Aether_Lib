#include "aether_convolution_transposed.h"

aether_layer init_ae_conv_tr2d(const aether_nn_config *const config, void *layer)
{
    aether_conv_tr2d *conv_tr = (aether_conv_tr2d *)layer;

    aether_layer wrapped_layer = {
        .input = NULL,
        .output = NULL,
        .gradient_in = NULL,
        .gradient_out = NULL,
        .layer = NULL,
        .layer_forward = NULL,
        .layer_backward = NULL,
        .layer_free = NULL};

    conv_tr->input = NULL;
    conv_tr->gradient_in = NULL;

    create_ae_tensor(&conv_tr->output, 0, 0, 0, 0);
    create_ae_tensor(&conv_tr->gradient_out, 0, 0, 0, 0);

    uint32_t bias_offset = conv_tr->out_channels * sizeof(float);
    if (config->is_train == true)
    {
        void *p = calloc(2, bias_offset);
        if (p == NULL)
            return wrapped_layer;
#ifdef DEBUG_AE
        printf("Allocated memory at address %p\n", p);
#endif
        conv_tr->gc_idx = append_ae_gc(p);

        conv_tr->bias = p;
        conv_tr->d_bias = (void *)conv_tr->bias + bias_offset;

        for (uint32_t i = 0; i < conv_tr->out_channels; i++)
        {
            conv_tr->bias[i] = 0.01f;
            conv_tr->d_bias[i] = 0.0f;
        }

        create_random_ae_tensor(&conv_tr->weights, conv_tr->out_channels, conv_tr->in_channels, conv_tr->kernel_size[0], conv_tr->kernel_size[1]);
        create_zero_ae_tensor(&conv_tr->d_weights, conv_tr->out_channels, conv_tr->in_channels, conv_tr->kernel_size[0], conv_tr->kernel_size[1]);

        create_ae_tensor(&conv_tr->af_cache, 0, 0, 0, 0);
    }
    else
    {
        void *p = calloc(1, bias_offset);
        if (p == NULL)
            return wrapped_layer;
#ifdef DEBUG_AE
        printf("Allocated memory at address %p\n", p);
#endif
        conv_tr->gc_idx = append_ae_gc(p);

        conv_tr->bias = p;

        for (uint32_t i = 0; i < conv_tr->out_channels; i++)
        {
            conv_tr->bias[i] = 0.01f;
        }

        create_random_ae_tensor(&conv_tr->weights, conv_tr->out_channels, conv_tr->in_channels, conv_tr->kernel_size[0], conv_tr->kernel_size[1]);
        create_empty_ae_tensor(&conv_tr->d_weights, 0, 0, 0, 0);

        create_empty_ae_tensor(&conv_tr->af_cache, 0, 0, 0, 0);
    }

    wrapped_layer.input = &conv_tr->input,
    wrapped_layer.output = &conv_tr->output,
    wrapped_layer.gradient_in = &conv_tr->gradient_in,
    wrapped_layer.gradient_out = &conv_tr->gradient_out,
    wrapped_layer.layer = layer,
    wrapped_layer.layer_forward = forward_ae_conv_tr2d,
    wrapped_layer.layer_backward = backward_ae_conv_tr2d,
    wrapped_layer.layer_free = free_ae_conv_tr2d;

    return wrapped_layer;
}

void free_ae_conv_tr2d(void *layer)
{
    if (layer == NULL)
        return;

    aether_conv_tr2d *conv_tr = (aether_conv_tr2d *)layer;

    free_ae_tensor(&conv_tr->af_cache);

    free_ae_tensor(&conv_tr->d_weights);

    free_ae_tensor(&conv_tr->weights);

    if (conv_tr->bias != NULL)
    {
#ifdef DEBUG_AE
        printf("Freed memory at address %p\n", conv_tr->bias);
#endif
        free(conv_tr->bias);
    }

    remove_ae_gc(conv_tr->gc_idx);

    free_ae_tensor(&conv_tr->gradient_out);

    free_ae_tensor(&conv_tr->output);

    return;
}

void forward_ae_conv_tr2d(const aether_nn_config *const config, void *layer)
{
    aether_conv_tr2d *conv_tr = (aether_conv_tr2d *)layer;

    assert(conv_tr->input->filters == conv_tr->in_channels);


    const uint32_t height_in = conv_tr->input->height;
    const uint32_t width_in = conv_tr->input->width;

    const uint32_t height_out = (height_in - 1) * conv_tr->stride[0] + conv_tr->dilation[0] * (conv_tr->kernel_size[0] - 1) - 2 * conv_tr->padding[0] + conv_tr->output_padding[0] + 1;
    const uint32_t width_out = (width_in - 1) * conv_tr->stride[1] + conv_tr->dilation[1] * (conv_tr->kernel_size[1] - 1) - 2 * conv_tr->padding[1] + conv_tr->output_padding[1] + 1;

    if (check_dims_ae_tensor(&conv_tr->output, 1, conv_tr->out_channels, height_out, width_out) == false)
        update_ae_tensor(&conv_tr->output, 1, conv_tr->out_channels, height_out, width_out);

    if (config->is_train)
        if (check_dims_ae_tensor(&conv_tr->af_cache, 1, conv_tr->out_channels, height_out, width_out) == false)
            update_ae_tensor(&conv_tr->af_cache, 1, conv_tr->out_channels, height_out, width_out);

    float sum = 0.0f;
    for (uint32_t f = 0; f < conv_tr->out_channels; f++)
    {
        for (uint32_t h = 0; h < height_out; h++)
        {
            for (uint32_t w = 0; w < width_out; w++)
            {
                sum = conv_tr->bias[f];
                for (uint32_t c = 0; c < conv_tr->in_channels; c++)
                {
                    for (uint32_t h_f = 0; h_f < conv_tr->kernel_size[0]; h_f++)
                    {
                        uint32_t h_tmp = h + h_f * conv_tr->dilation[0] + conv_tr->padding[0] - conv_tr->kernel_size[0] + 1;
                        if (h_tmp % conv_tr->stride[0] == 0)
                        {
                            h_tmp /= conv_tr->stride[0];
                            for (uint32_t w_f = 0; w_f < conv_tr->kernel_size[1]; w_f++)
                            {
                                uint32_t w_tmp = w + w_f * conv_tr->dilation[1] + conv_tr->padding[1] - conv_tr->kernel_size[1] + 1;
                                if (w_tmp % conv_tr->stride[1] == 0)
                                {
                                    w_tmp /= conv_tr->stride[1];
                                    if (h_tmp >= 0 && h_tmp < height_in && w_tmp >= 0 && w_tmp < width_in)
                                    {
                                        sum += (*get_element_ae_tensor(conv_tr->input, 0, c, h_tmp, w_tmp)) *
                                               (*get_element_ae_tensor(&conv_tr->weights, f, c, conv_tr->kernel_size[0] - 1 - h_f, conv_tr->kernel_size[1] - 1 - w_f));
                                    }
                                }
                            }
                        }
                    }
                }
                if (config->is_train)
                    *get_element_ae_tensor(&conv_tr->af_cache, 0, f, h, w) = sum;
                *get_element_ae_tensor(&conv_tr->output, 0, f, h, w) = conv_tr->activation_function(FORWARD, &sum);
            }
        }
    }

    return;
}

void backward_ae_conv_tr2d(const aether_nn_config *const config, void *layer)
{
    aether_conv_tr2d *conv_tr = (aether_conv_tr2d *)layer;

    assert(conv_tr->gradient_in->filters == conv_tr->out_channels);

    const uint32_t height_in = conv_tr->input->height;
    const uint32_t width_in = conv_tr->input->width;

    const uint32_t height_out = (height_in - 1) * conv_tr->stride[0] + conv_tr->dilation[0] * (conv_tr->kernel_size[0] - 1) - 2 * conv_tr->padding[0] + conv_tr->output_padding[0] + 1;
    const uint32_t width_out = (width_in - 1) * conv_tr->stride[1] + conv_tr->dilation[1] * (conv_tr->kernel_size[1] - 1) - 2 * conv_tr->padding[1] + conv_tr->output_padding[1] + 1;

    if (check_dims_ae_tensor(&conv_tr->gradient_out, 1, conv_tr->in_channels, height_in, width_in) == false)
        update_ae_tensor(&conv_tr->gradient_out, 1, conv_tr->in_channels, height_in, width_in);

    float sum = 0.0f;
    for (uint32_t f = 0; f < conv_tr->out_channels; f++)
    {
        for (uint32_t h = 0; h < height_out; h++)
        {
            for (uint32_t w = 0; w < width_out; w++)
            {
                (*get_element_ae_tensor(conv_tr->gradient_in, 0, f, h, w)) *= conv_tr->activation_function(BACKWARD, get_element_ae_tensor(&conv_tr->af_cache, 0, f, h, w));
                for (uint32_t c = 0; c < conv_tr->in_channels; c++)
                {
                    for (uint32_t h_f = 0; h_f < conv_tr->kernel_size[0]; h_f++)
                    {
                        uint32_t h_tmp = h - h_f * conv_tr->dilation[0] + conv_tr->padding[0];
                        if (h_tmp % conv_tr->stride[0] == 0)
                        {
                            h_tmp /= conv_tr->stride[0];
                            for (uint32_t w_f = 0; w_f < conv_tr->kernel_size[1]; w_f++)
                            {
                                uint32_t w_tmp = w - w_f * conv_tr->dilation[1] + conv_tr->padding[1];
                                if (w_tmp % conv_tr->stride[1] == 0)
                                {
                                    w_tmp /= conv_tr->stride[1];
                                    if (h_tmp >= 0 && h_tmp < height_in && w_tmp >= 0 && w_tmp < width_in)
                                    {
                                        *get_element_ae_tensor(&conv_tr->d_weights, f, c, h_f, w_f) += (*get_element_ae_tensor(conv_tr->input, 0, c, h_tmp, w_tmp)) * (*get_element_ae_tensor(conv_tr->gradient_in, 0, f, h, w));
                                    }
                                }
                            }
                        }
                    }
                }
                conv_tr->d_bias[f] += (*get_element_ae_tensor(conv_tr->gradient_in, 0, f, h, w));
            }
        }
    }

    for (uint32_t c = 0; c < conv_tr->in_channels; c++)
    {
        for (uint32_t h = 0; h < height_in; h++)
        {
            for (uint32_t w = 0; w < width_in; w++)
            {
                sum = 0.0f;
                for (uint32_t f = 0; f < conv_tr->out_channels; f++)
                {
                    for (uint32_t h_f = 0; h_f < conv_tr->kernel_size[0]; h_f++)
                    {
                        uint32_t h_tmp = h * conv_tr->stride[0] + h_f * conv_tr->dilation[0] - conv_tr->padding[0];
                        for (uint32_t w_f = 0; w_f < conv_tr->kernel_size[1]; w_f++)
                        {
                            uint32_t w_tmp = w * conv_tr->stride[0] + w_f * conv_tr->dilation[1] - conv_tr->padding[1];
                            if (h_tmp >= 0 && h_tmp < height_out && w_tmp >= 0 && w_tmp < width_out)
                            {
                                sum += (*get_element_ae_tensor(&conv_tr->weights, f, c, h_f, w_f)) * (*get_element_ae_tensor(conv_tr->gradient_in, 0, f, h_tmp, w_tmp));
                            }
                        }
                    }
                }
                *get_element_ae_tensor(&conv_tr->gradient_out, 0, c, h, w) = sum;
            }
        }
    }

    for (uint32_t f = 0; f < conv_tr->out_channels; f++)
    {
        for (uint32_t c = 0; c < conv_tr->in_channels; c++)
        {
            for (uint32_t h_f = 0; h_f < conv_tr->kernel_size[0]; h_f++)
            {
                for (uint32_t w_f = 0; w_f < conv_tr->kernel_size[1]; w_f++)
                {
                    *get_element_ae_tensor(&conv_tr->weights, f, c, h_f, w_f) -= config->learning_rate * (*get_element_ae_tensor(&conv_tr->d_weights, f, c, h_f, w_f));
                    *get_element_ae_tensor(&conv_tr->d_weights, f, c, h_f, w_f) = 0.0f;
                }
            }
        }
        conv_tr->bias[f] -= config->learning_rate * conv_tr->d_bias[f];
        conv_tr->d_bias[f] = 0.0f;
    }

    return;
}
