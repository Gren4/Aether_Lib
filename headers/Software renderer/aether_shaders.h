// based on https://github.com/ssloy/tinyrenderer/wiki
#ifndef __AETHER_SHADERS_H__
#define __AETHER_SHADERS_H__

#include "aether_tga.h"
#include "aether_geometry.h"
#include "aether_model.h"

typedef struct ae_shader
{
    double coeff_p[3];
    ae_vec3_f orig_p[3];
    ae_vec3_f p[3];
    ae_vec2_f uvs[3];
    ae_vec3_f normals[3];
    const bool (*vertex)(ae_model const *model, const int32_t face_i);
    const void (*fragment)(ae_model const *model, ae_vec3_f const *bar, ae_tga_c *color);
} ae_shader;

extern ae_shader ShadowBufferShader;
extern ae_shader Shader;

#endif //__AETHER_SHADERS_H__
