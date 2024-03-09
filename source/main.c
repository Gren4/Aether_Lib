#include "aether_render.h"

#include <time.h>

#include <stdio.h>
#include <string.h>

//#define a
#ifdef a
int main(void)
{
    init_ae_gc();
    clock_t tic, toc;
    ae_vec models = create_ae_vec(sizeof(ae_model), 0);
    tic = clock();
    ae_model model;

    model = open_ae_model(
        "models\\african_head.obj",
        "models\\african_head_diffuse.tga",
        AE_NM_TANGENT,
        "models\\african_head_nm_tangent.tga",
        "models\\african_head_spec.tga");

    *(ae_model*)append_ae_vec(&models) = model;


    model = open_ae_model(
        "models\\floor.obj",
        "models\\floor_diffuse.tga",
        AE_NM_TANGENT,
        "models\\floor_nm_tangent.tga",
        NULL);

    *(ae_model*)append_ae_vec(&models) = model;


    model = open_ae_model(
        "models\\african_head_eye_inner.obj",
        "models\\african_head_eye_inner_diffuse.tga",
        AE_NM_GLOBAL,
        "models\\african_head_eye_inner_nm.tga",
        "models\\african_head_eye_inner_spec.tga");

    *(ae_model*)append_ae_vec(&models) = model;

    toc = clock();
    printf("Open model execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);

    tic = clock();
    render_model(&models);
    toc = clock();
    printf("Render execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);
    on_return_ae_gc();
    return 0;
}
#endif
