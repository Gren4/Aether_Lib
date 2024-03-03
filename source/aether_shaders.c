// based on https://github.com/ssloy/tinyrenderer/wiki
#include "aether_shaders.h"
#include "aether_render_gv.h"
#include "aether_utils.h"
#include <math.h>
#include <float.h>

static void ShadowBufferShader_vertex(ae_model const *model, const int32_t face_i);
static void ShadowBufferShader_fragment(ae_model const *model, ae_vec3_f const *bar, ae_tga_c *color);

static void Shader_vertex(ae_model const *model, const int32_t face_i);
static void Shader_fragment(ae_model const *model, ae_vec3_f const *bar, ae_tga_c *color);

ae_shader ShadowBufferShader = {.vertex = &ShadowBufferShader_vertex, .fragment = &ShadowBufferShader_fragment};
ae_shader Shader = {.vertex = &Shader_vertex, .fragment = &Shader_fragment};

static void ShadowBufferShader_vertex(ae_model const *model, const int32_t face_i)
{
    ae_face face[3];
    face_ae_model(model, face_i, face);
    for (size_t j = 0; j < 3; j++)
    {
        vert_ae_model(model, face[j].v_i, &ShadowBufferShader.p[j]);
        AE_M_x_V_I_EMBED_RENDER(ShadowBufferShader.p[j], Z, ShadowBufferShader.p[j]);
    }
    return;
}

static void ShadowBufferShader_fragment(ae_model const *model, ae_vec3_f const *bar, ae_tga_c *color)
{
    double P_z;
    P_z = bar->raw[0] * Shader.p[0].z + bar->raw[1] * Shader.p[1].z + bar->raw[2] * Shader.p[2].z;

    uint8_t coeff = 255 * (P_z / depth);
    AE_TGA_SET_P_RGBA(color, coeff, coeff, coeff, 255);
    return;
}

static void Shader_vertex(ae_model const *model, const int32_t face_i)
{
    ae_face face[3];
    face_ae_model(model, face_i, face);
    for (size_t j = 0; j < 3; j++)
    {
        vert_ae_model(model, face[j].v_i, &Shader.orig_p[j]);
        AE_M_x_V_F_EMBED_RENDER(Shader.orig_p[j], Proj_ModelView, Shader.orig_p[j]);
        AE_M_x_V_I_EMBED_RENDER(Shader.p[j], ViewPort, Shader.orig_p[j]);
        uv_ae_model(model, face[j].uv_i, &Shader.uvs[j]);
        normal_ae_model(model, face[j].norm_i, &Shader.normals[j]);
        AE_M_x_V_F_PROJ_RENDER(Shader.normals[j], Proj_ModelView_IT, Shader.normals[j], 0.0);
    }
    return;
}

static void Shader_fragment(ae_model const *model, ae_vec3_f const *bar, ae_tga_c *color)
{
    // Get uv coords
    ae_vec2_f uv = {.u = bar->raw[0] * Shader.uvs[0].u + bar->raw[1] * Shader.uvs[1].u + bar->raw[2] * Shader.uvs[2].u,
                    .v = bar->raw[0] * Shader.uvs[0].v + bar->raw[1] * Shader.uvs[1].v + bar->raw[2] * Shader.uvs[2].v};

    // Calculate normal maps in the tangent space and coeff
    ae_vec3_f bn = {.x = bar->raw[0] * Shader.normals[0].x + bar->raw[1] * Shader.normals[1].x + bar->raw[2] * Shader.normals[2].x,
                    .y = bar->raw[0] * Shader.normals[0].y + bar->raw[1] * Shader.normals[1].y + bar->raw[2] * Shader.normals[2].y,
                    .z = bar->raw[0] * Shader.normals[0].z + bar->raw[1] * Shader.normals[1].z + bar->raw[2] * Shader.normals[2].z};

    AE_VEC3_NORMALIZE(bn, bn, 1);

    AE_MATRIX_F_CREATE(Am, 3, 3);
    AE_MATRIX_F_SET_ROW_FROM_V_DIF(Am, 0, Shader.orig_p[1], Shader.orig_p[0]);
    AE_MATRIX_F_SET_ROW_FROM_V_DIF(Am, 1, Shader.orig_p[2], Shader.orig_p[0]);
    AE_MATRIX_F_SET_ROW_FROM_V(Am, 2, bn);

    AE_MATRIX_F_CREATE(AIm, 3, 3);
    AE_MATRIX_F_INVERSE(AIm, Am, 3, 3);

    ae_vec3_f i_v = {.x = (AE_MATRIX_F_GET(AIm, 0, 0) * (Shader.uvs[1].u - Shader.uvs[0].u) + AE_MATRIX_F_GET(AIm, 0, 1) * (Shader.uvs[2].u - Shader.uvs[0].u)),
                     .y = (AE_MATRIX_F_GET(AIm, 1, 0) * (Shader.uvs[1].u - Shader.uvs[0].u) + AE_MATRIX_F_GET(AIm, 1, 1) * (Shader.uvs[2].u - Shader.uvs[0].u)),
                     .z = (AE_MATRIX_F_GET(AIm, 2, 0) * (Shader.uvs[1].u - Shader.uvs[0].u) + AE_MATRIX_F_GET(AIm, 2, 1) * (Shader.uvs[2].u - Shader.uvs[0].u))};
    AE_VEC3_NORMALIZE(i_v, i_v, 1);

    ae_vec3_f j_v = {.x = (AE_MATRIX_F_GET(AIm, 0, 0) * (Shader.uvs[1].v - Shader.uvs[0].v) + AE_MATRIX_F_GET(AIm, 0, 1) * (Shader.uvs[2].v - Shader.uvs[0].v)),
                     .y = (AE_MATRIX_F_GET(AIm, 1, 0) * (Shader.uvs[1].v - Shader.uvs[0].v) + AE_MATRIX_F_GET(AIm, 1, 1) * (Shader.uvs[2].v - Shader.uvs[0].v)),
                     .z = (AE_MATRIX_F_GET(AIm, 2, 0) * (Shader.uvs[1].v - Shader.uvs[0].v) + AE_MATRIX_F_GET(AIm, 2, 1) * (Shader.uvs[2].v - Shader.uvs[0].v))};
    AE_VEC3_NORMALIZE(j_v, j_v, 1);

    AE_MATRIX_F_CREATE(Bm, 3, 3);
    AE_MATRIX_F_SET_COL_FROM_V(Bm, 0, i_v);
    AE_MATRIX_F_SET_COL_FROM_V(Bm, 1, j_v);
    AE_MATRIX_F_SET_COL_FROM_V(Bm, 2, bn);

    ae_vec3_f t_normal = normal_map_ae_model(model, &uv);
    ae_vec3_f normal;
    AE_MATRIX_3x3_MULT_V(normal, Bm, t_normal);
    AE_VEC3_NORMALIZE(normal, normal, 1);
    double diff = AE_VEC3_DOT(normal, l);

    // Get specular coeff
    ae_vec3_f r = normal;
    AE_VEC3_F_MULT(r, r, diff * 2.0);
    AE_VEC3_DIF(r, r, l);
    AE_VEC3_NORMALIZE(r, r, 1);
    r.z = max_ae(r.z, 0.0);
    double spec = pow(r.z, specular_map_ae_model(model, &uv));

    diff = max_ae(diff, 0.0);

    *color = texture_ae_model(model, &uv);

    // Calc shadow
    ae_vec3_f sb_p = {.x = bar->raw[0] * Shader.p[0].x + bar->raw[1] * Shader.p[1].x + bar->raw[2] * Shader.p[2].x,
                      .y = bar->raw[0] * Shader.p[0].y + bar->raw[1] * Shader.p[1].y + bar->raw[2] * Shader.p[2].y,
                      .z = bar->raw[0] * Shader.p[0].z + bar->raw[1] * Shader.p[1].z + bar->raw[2] * Shader.p[2].z};
    AE_M_x_V_F_EMBED_RENDER(sb_p, Shadow, sb_p);
    int32_t idx = (int32_t)sb_p.raw[0] + (int32_t)sb_p.raw[1] * width;
    double shadow = 0.3 + 0.7 * (shadow_buffer[idx] < sb_p.raw[2] + 43.34);

    // Calc sum of coeffs
    double intensity = (1.0 * diff + 0.3 * spec) * shadow;
    AE_TGA_SET_P_RGBA(
        color,
        min_ae(5 + color->r * intensity, 255),
        min_ae(5 + color->g * intensity, 255),
        min_ae(5 + color->b * intensity, 255),
        255);

    return;
}
