// based on https://github.com/ssloy/tinyrenderer/wiki
#ifndef __AETHER_MODEL__
#define __AETHER_MODEL__

#include "aether_vector.h"
#include "aether_tga.h"
#include "aether_geometry.h"

typedef struct ae_face
{
    size_t v_i;
    size_t uv_i;
    size_t norm_i;
} ae_face;

typedef struct ae_model
{
    ae_vector verts;
    ae_vector faces;
    ae_vector normals;
    ae_vector uvs;
    ae_tga_i texture;
} ae_model;

ae_model open_ae_model(const char *m_filename, const char *t_filename);
void close_ae_model(ae_model *model);
size_t n_verts_ae_model(ae_model *model);
size_t n_faces_ae_model(ae_model *model);
void vert_ae_model(ae_model *model, size_t i, ae_vec3_f *vert);
void face_ae_model(ae_model *model, size_t i, ae_face *face);
void uv_ae_model(ae_model *model, size_t i, ae_vec2_f *uv);
void normal_ae_model(ae_model *model, size_t i, ae_vec3_f *norm);

#endif // __AETHER_MODEL__
