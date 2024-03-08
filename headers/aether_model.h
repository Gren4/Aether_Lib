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

typedef enum ae_nm_type
{
    AE_NM_NONE,
    AE_NM_GLOBAL,
    AE_NM_TANGENT
} ae_nm_type;

typedef struct ae_model
{
    ae_vector verts;
    ae_vector faces;
    ae_vector normals;
    ae_vector uvs;
    ae_tga_i texture;
    ae_nm_type nm_type;
    ae_tga_i normal_map;
    ae_tga_i specular_map;
} ae_model;

ae_model open_ae_model(const char *m_filename, const char *t_filename, const ae_nm_type nm_type, const char *nm_filename, const char *sm_filename);
void close_ae_model(ae_model *model);
size_t n_verts_ae_model(ae_model const *model);
size_t n_faces_ae_model(ae_model const *model);
ae_vec3_f vert_ae_model(ae_model const *model, const size_t i);
void* face_ae_model(ae_model const *model, const size_t i);
ae_vec2_f uv_ae_model(ae_model const *model, const size_t i);
ae_vec3_f normal_ae_model(ae_model const *model, const size_t i);
ae_tga_c texture_ae_model(ae_model const *model, ae_vec2_f const *uv);
ae_vec3_f normal_map_ae_model(ae_model const *model, ae_vec2_f const *uv);
double specular_map_ae_model(ae_model const *model, ae_vec2_f const *uv);

#endif // __AETHER_MODEL__
