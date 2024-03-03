// based on https://github.com/ssloy/tinyrenderer/wiki
#include "aether_render.h"
#include "aether_render_gv.h"
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

ae_vec3_f light_dir = {.x = 1.0, .y = 1.0, .z = 1.0};
ae_vec3_f l = {.x = 1.0, .y = 1.0, .z = 1.0};
ae_vec3_f eye = {.x = 0.0, .y = 1.0, .z = 3.0};
ae_vec3_f center = {.x = 0.0, .y = 0.0, .z = 0.0};
ae_vec3_f up = {.x = 0.0, .y = 1.0, .z = 0.0};

void render_model(ae_model const *model, ae_tga_i *image)
{
    double X_deg = 0.0;
    double Y_deg = 0.0;
    double Z_deg = 0.0;

    double *zbuffer = (double *)(calloc(image->width * image->height, sizeof(double)));
    for (size_t i = 0; i < image->width * image->height; i++)
    {
        zbuffer[i] = -DBL_MAX;
    }

    AE_LOOK_AT_RENDER(ModelView, eye, center, up);

    AE_VIEWPORT_RENDER(ViewPort, 255, image->width / 3, image->height / 4, image->width / 3, image->height / 2);

    AE_PROJECTION_RENDER(Projection, eye, center);
    AE_R_XYZ_RENDER(r_xyz, X_deg, Y_deg, Z_deg);

    AE_MATRIX_F_MULT(Proj_ModelView, Projection, ModelView, 4, 4);
    AE_MATRIX_F_INVERSE_TRANSPOSE(Proj_ModelView_IT, Proj_ModelView, 4, 4);

    AE_M_x_V_F_PROJ_RENDER(l, Proj_ModelView, light_dir, 0.0);
    AE_VEC3_NORMALIZE(l, l, 1);

    ae_vec3_f x = {.x = 10.0, .y = 0.0, .z = 0.0};
    ae_vec3_f y = {.x = 0.0, .y = 10.0, .z = 0.0};
    ae_vec3_f o = {.x = 0.0, .y = 0.0, .z = 0.0};

    AE_DEFORM(ViewPort, o);
    AE_DEFORM(ViewPort, x);
    AE_DEFORM(ViewPort, y);

    line_ae_render(o, x, image, &red);
    line_ae_render(o, y, image, &green);

    ae_face face[3];
    ae_render_data render_data;

    for (size_t i = 0; i < n_faces_ae_model(model); i++)
    {
        face_ae_model(model, i, face);
        for (size_t j = 0; j < 3; j++)
        {
            vert_ae_model(model, face[j].v_i, &render_data.orig_p[j]);
            AE_M_x_V_F_EMBED_RENDER(render_data.orig_p[j], Proj_ModelView, render_data.orig_p[j]);
            AE_M_x_V_I_EMBED_RENDER(render_data.p[j], ViewPort, render_data.orig_p[j]);
            uv_ae_model(model, face[j].uv_i, &render_data.uvs[j]);
            normal_ae_model(model, face[j].norm_i, &render_data.normals[j]);
            AE_M_x_V_F_PROJ_RENDER(render_data.normals[j], Proj_ModelView_IT, render_data.normals[j], 0.0);
        }

        triangle_ae_render(image, model, &render_data, zbuffer);
    }

    free(zbuffer);

    return;
}
