// based on https://github.com/ssloy/tinyrenderer/wiki
#include "aether_render.h"
#include "aether_render_gv.h"
#include <stdlib.h>
#include <math.h>
#include <float.h>

static AE_TGA_C_RGBA(red, 255, 0, 0, 255);
static AE_TGA_C_RGBA(green, 0, 255, 0, 255);

AE_MATRIX_F_CREATE(ViewPort, 4, 4);
AE_MATRIX_F_CREATE(ModelView, 4, 4);
AE_MATRIX_F_CREATE(Projection, 4, 4);
AE_MATRIX_F_CREATE(Proj_ModelView, 4, 4);
AE_MATRIX_F_CREATE(Proj_ModelView_IT, 4, 4);
AE_MATRIX_F_CREATE(r_xyz, 4, 4);
AE_MATRIX_F_CREATE(Z, 4, 4);
AE_MATRIX_F_CREATE(Z_shdw, 4, 4);
AE_MATRIX_F_CREATE(Shadow, 4, 4);

ae_vec3_f light_dir = {.x = 1.0, .y = 1.0, .z = 1.0};
ae_vec3_f l = {.x = 1.0, .y = 1.0, .z = 1.0};
ae_vec3_f eye = {.x = -1.0, .y = 0.1, .z = 5.0};
ae_vec3_f center = {.x = 0.0, .y = 0.0, .z = 0.0};
ae_vec3_f up = {.x = 0.0, .y = 1.0, .z = 0.0};

double eye_center_dif;
double *shadow_buffer;

bool shadows_on = true;

int32_t width = 800;
int32_t height = 800;
int32_t depth = 2000;

void render_model(ae_vec const *models)
{
    ae_tga_i image = create_ae_tga(width, height, RGBA);
    ae_model model;
    // double X_deg = 0.0;
    // double Y_deg = 0.0;
    // double Z_deg = 0.0;
    // AE_R_XYZ_RENDER(r_xyz, X_deg, Y_deg, Z_deg);

    double *zbuffer = (double *)(calloc(width * height, sizeof(double)));
    shadow_buffer = (double *)(calloc(width * height, sizeof(double)));
    for (size_t i = 0; i < width * height; i++)
    {
        zbuffer[i] = shadow_buffer[i] = -DBL_MAX;
    }

    AE_VIEWPORT_RENDER(ViewPort, depth, width / 8, height / 8, width * 3 / 4, height * 3 / 4);

    ae_vec3_f x = {.x = 10.0, .y = 0.0, .z = 0.0};
    ae_vec3_f y = {.x = 0.0, .y = 10.0, .z = 0.0};
    ae_vec3_f o = {.x = 0.0, .y = 0.0, .z = 0.0};

    AE_DEFORM(ViewPort, o);
    AE_DEFORM(ViewPort, x);
    AE_DEFORM(ViewPort, y);

    line_ae_render(o, x, &image, &red);
    line_ae_render(o, y, &image, &green);

    if (shadows_on == true)
    {
        AE_LOOK_AT_RENDER(ModelView, light_dir, center, up);
        AE_PROJECTION_RENDER(Projection, 0.0);
        AE_MATRIX_F_MULT(Proj_ModelView, Projection, ModelView, 4, 4);
        AE_MATRIX_F_INVERSE_TRANSPOSE(Proj_ModelView_IT, Proj_ModelView, 4, 4);
        AE_MATRIX_F_MULT(Z, ViewPort, Proj_ModelView, 4, 4);
        for (size_t m = 0; m < models->data.quant; m++)
        {
            model = *(ae_model*)get_ae_vec(models, m);
            for (size_t i = 0; i < n_faces_ae_model(&model); i++)
            {
                if (ShadowBufferShader.vertex(&model, i))
                    shadow_buffer_ae_render(&image, &model, &ShadowBufferShader, shadow_buffer);
            }
        }
        Z_shdw = Z;
    }
    AE_LOOK_AT_RENDER(ModelView, eye, center, up);
    AE_PROJECTION_RENDER(Projection, eye_center_dif);
    AE_MATRIX_F_MULT(Proj_ModelView, Projection, ModelView, 4, 4);
    AE_MATRIX_F_INVERSE_TRANSPOSE(Proj_ModelView_IT, Proj_ModelView, 4, 4);
    AE_MATRIX_F_MULT(Z, ViewPort, Proj_ModelView, 4, 4);
    AE_M_x_V_F_PROJ_RENDER(l, Proj_ModelView, light_dir, 0.0);
    AE_VEC3_NORMALIZE(l, l, 1);
    if (shadows_on)
        AE_SHADOW_M_RENDER(Shadow);

    for (size_t m = 0; m < models->data.quant; m++)
    {
        model = *(ae_model*)get_ae_vec(models, m);
        for (size_t i = 0; i < n_faces_ae_model(&model); i++)
        {
            if (Shader.vertex(&model, i))
                triangle_ae_render(&image, &model, &Shader, zbuffer);
        }
    }

    free(zbuffer);
    free(shadow_buffer);

    flip_vertically_ae_tga(&image);
    write_file_ae_tga(&image, "output.tga", false);

    return;
}
