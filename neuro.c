#include "aether_layers.h"
#include "aether_sequence.h"
#include "aether_convolution.h"
#include "aether_convolution_transposed.h"
#include "aether_loss_functions.h"

#define SDL_MAIN_HANDLED
#include "SDL.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 640

int main(void)
_AE_START_MAIN
    aether_sequence seq;
    init_ae_sequence(&seq, true, 0.001f);

    aether_conv2d conv2d_1 = {
        .in_channels = 1,
        .out_channels = 32,
        .kernel_size = {3,3},
        .stride = {2,2},
        .padding = {1,1},
        .dilation = {1,1},
        .activation_function = Sigmoid_ae_af
    };

    aether_conv2d conv2d_2 = {
        .in_channels = 32,
        .out_channels = 64,
        .kernel_size = {3,3},
        .stride = {2,2},
        .padding = {1,1},
        .dilation = {1,1},
        .activation_function = Sigmoid_ae_af
    };

    aether_conv_tr2d conv_tr2d_1 = {
        .in_channels = 64,
        .out_channels = 32,
        .kernel_size = {4,4},
        .stride = {2,2},
        .padding = {1,1},
        .output_padding = {0,0},
        .dilation = {1,1},
        .activation_function = Sigmoid_ae_af
    };

    aether_conv_tr2d conv_tr2d_2 = {
        .in_channels = 32,
        .out_channels = 1,
        .kernel_size = {4,4},
        .stride = {2,2},
        .padding = {1,1},
        .output_padding = {0,0},
        .dilation = {1,1},
        .activation_function = Sigmoid_ae_af
    };
    float in[] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    };
    uint32_t in_s = sqrt(sizeof(in)/sizeof(in[0]));
    aether_tensor input;
    create_from_array_ae_tensor(&input, 1, 1, in_s, in_s, in);
    aether_tensor reference;
    create_from_array_ae_tensor(&reference, 1, 1, in_s, in_s, in);

    add_layer_ae_sequence(&seq, init_ae_conv2d(&seq.config, &conv2d_1));
    add_layer_ae_sequence(&seq, init_ae_conv2d(&seq.config, &conv2d_2));
    add_layer_ae_sequence(&seq, init_ae_conv_tr2d(&seq.config, &conv_tr2d_1));
    add_layer_ae_sequence(&seq, init_ae_conv_tr2d(&seq.config, &conv_tr2d_2));

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        return EXIT_FAILURE;

    SDL_Window *window = SDL_CreateWindow("Neuro",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (window == NULL)
        return EXIT_FAILURE;

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (renderer == NULL)
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_RenderClear(renderer);

    SDL_Rect rect = {0};

    bool quit = false;
    uint32_t i, j;

    uint32_t timer = 0;

    float loss = 1.0f;
    while(!quit)
    {
        static SDL_Event e;

        SDL_WaitEventTimeout(&e, 0);

        switch(e.type)
        {
        case SDL_QUIT:
            quit = true;
            break;
        case SDL_KEYDOWN:
            if (e.key.keysym.sym == SDLK_ESCAPE)
                quit = true;
            break;
        default:
            break;
        }

        if (loss > 1e-21)
        {
            forward_ae_sequence(&seq, &input);

            rect.h = SCREEN_HEIGHT / seq.output->height;
            rect.w = SCREEN_WIDTH / seq.output->width;

            loss = MSE_ae_loss(seq.output, &reference, &seq.loss);
            printf("%.20f \r", loss);
            backward_ae_sequence(&seq);

            if (timer % 10 == 0)
            {
                for (i = 0; i < seq.output->height; i++)
                {
                    for (j = 0; j < seq.output->width; j++)
                    {
                        unsigned int color = (unsigned int)(255 * *get_element_ae_tensor(seq.output, 0, 0, i, j));
                        SDL_SetRenderDrawColor(renderer, color, color, color, 0xFF);
                        rect.x = j * rect.w;
                        rect.y = i * rect.h;
                        SDL_RenderFillRect(renderer, &rect);
                    }
                }

                SDL_RenderPresent(renderer);
            }
            timer += 1;
        }
        else
        {
            for (i = 0; i < seq.output->height; i++)
            {
                for (j = 0; j < seq.output->width; j++)
                {
                    unsigned int color = (unsigned int)(255 * *get_element_ae_tensor(seq.output, 0, 0, i, j));
                    SDL_SetRenderDrawColor(renderer, color, color, color, 0xFF);
                    rect.x = j * rect.w;
                    rect.y = i * rect.h;
                    SDL_RenderFillRect(renderer, &rect);
                }
            }

            SDL_RenderPresent(renderer);

            quit = true;
        }
    }
    printf("\n");
    print_ae_tensor(seq.output);

    free_ae_sequence(&seq);

    SDL_DestroyWindow(window);

    SDL_DestroyRenderer(renderer);

    SDL_Quit();

AE_END_(0)
