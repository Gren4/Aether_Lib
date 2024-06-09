#include "aether_sequence.h"

void init_ae_sequence(aether_sequence *sequence, bool is_train, float learning_rate)
{
    sequence->config.is_train = is_train;
    sequence->config.learning_rate = learning_rate;
    sequence->layers_quant = 0;
    sequence->layers = NULL;
    sequence->input = NULL;
    sequence->output = NULL;
    create_ae_tensor(&sequence->loss, 0, 0, 0, 0);
    sequence->layers = malloc(0);
#ifdef DEBUG_AE
    printf("Allocated memory at address %p\n", sequence->layers);
#endif
    sequence->gc_idx = append_ae_gc(sequence->layers);

    return;
}

void free_ae_sequence(aether_sequence *sequence)
{
    for (uint32_t i = sequence->layers_quant; i > 0; i--)
    {
        sequence->layers[i - 1].layer_free(sequence->layers[i - 1].layer);
    }

    sequence->layers_quant = 0;

#ifdef DEBUG_AE
    printf("Freed memory at address %p\n", sequence->layers);
#endif
    free(sequence->layers);
    remove_ae_gc(sequence->gc_idx);
    free_ae_tensor(&sequence->loss);

    return;
}

void add_layer_ae_sequence(aether_sequence *sequence, aether_layer layer)
{
    sequence->layers_quant += 1;

    void *p = realloc(sequence->layers, sequence->layers_quant * sizeof(aether_layer));
#ifdef DEBUG_AE
        printf("Reallocated memory from address %p to %p\n", sequence->layers, p);
#endif
    sequence->layers = p;

    if (sequence->layers_quant > 1)
    {
        aether_layer *last_layer = &sequence->layers[sequence->layers_quant - 2];

        *layer.gradient_in = &sequence->loss;
        sequence->output = layer.output;

        *last_layer->gradient_in = layer.gradient_out;
        *layer.input = last_layer->output;
    }
    else
    {
        *layer.gradient_in = &sequence->loss;
        sequence->output = layer.output;
        sequence->input = layer.input;
    }

    sequence->layers[sequence->layers_quant - 1] = layer;

    return;
}

void forward_ae_sequence(aether_sequence *sequence, const aether_tensor * const input)
{
    *sequence->input = input;
    for (uint32_t i = 0; i < sequence->layers_quant; i++)
    {
        sequence->layers[i].layer_forward(&sequence->config, sequence->layers[i].layer);
    }

    return;
}

void backward_ae_sequence(aether_sequence *sequence)
{
    for (uint32_t i = sequence->layers_quant; i > 0; i--)
    {
        sequence->layers[i - 1].layer_backward(&sequence->config, sequence->layers[i - 1].layer);
    }

    return;
}
