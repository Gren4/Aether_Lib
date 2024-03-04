#include "aether_render.h"

#include <time.h>

#include <stdio.h>
#include <string.h>

int main(void)
{
    clock_t tic, toc;
    ae_vector models = create_ae_vector(sizeof(ae_model), 0);
    tic = clock();
    ae_model model;
    
    model = open_ae_model(
        "models\\african_head.obj",
        "models\\african_head_diffuse.tga",
        AE_NM_TANGENT,
        "models\\african_head_nm_tangent.tga",
        "models\\african_head_spec.tga");

    append_ae_vector(&models, &model);

/*
    model = open_ae_model(
        "models\\african_head_eye_outer.obj",
        "models\\african_head_eye_outer_diffuse.tga",
        AE_NM_GLOBAL,
        "models\\african_head_eye_outer_nm.tga",
        "models\\african_head_eye_outer_spec.tga");

    append_ae_vector(&models, &model);
*/
    
    model = open_ae_model(
        "models\\african_head_eye_inner.obj",
        "models\\african_head_eye_inner_diffuse.tga",
        AE_NM_GLOBAL,
        "models\\african_head_eye_inner_nm.tga",
        "models\\african_head_eye_inner_spec.tga");

    append_ae_vector(&models, &model);
    toc = clock();
    printf("Open model execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);

    tic = clock();
    render_model(&models);
    toc = clock();
    printf("Render execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);

    for (size_t m = 0; m < models.data.quant; m++)
    {
        get_ae_vector(&models, m, &model);
        close_ae_model(&model);
    }
    free_ae_vector(&models);

    return 0;
}
