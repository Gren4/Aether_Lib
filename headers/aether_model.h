#ifndef __AETHER_MODEL__
#define __AETHER_MODEL__

#include "aether_vector.h"
#include "aether_geometry.h"

typedef struct ae_model
{
    ae_vector verts;
    ae_vector faces;
} ae_model;

ae_model open_ae_model(const char *filename);
void close_ae_model(ae_model *model);
size_t n_verts_ae_model(ae_model *model);
size_t n_faces_ae_model(ae_model *model);
ae_vec3_f vert_ae_model(ae_model *model, size_t i);
ae_vector face_ae_model(ae_model *model, size_t idx);


#endif // __AETHER_MODEL__
