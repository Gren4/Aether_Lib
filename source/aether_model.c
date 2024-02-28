#include "aether_model.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

ae_model open_ae_model(const char *m_filename, const char *t_filename)
{
    ae_model new_model = {
        .verts = create_ae_vector(sizeof(ae_vec3_f), 0),
        .faces = create_ae_vector(sizeof(ae_face) * 3, 0),
        .uvs = create_ae_vector(sizeof(ae_vec2_f), 0),
        .normals = create_ae_vector(sizeof(ae_vec3_f), 0),
        .texture = {.data = NULL}};
    FILE *in;
    if ((in = fopen(m_filename, "rb")) == NULL)
    {
        return new_model;
    }

    if (t_filename != NULL)
    {
        read_file_ae_tga(&new_model.texture, t_filename);
    }

    int32_t type = 0;
    char a[2] = {0};
    while ((a[0] = fgetc(in)) != EOF)
    {
        if (a[0] == '\n')
            continue;
        a[1] = fgetc(in);
        type = a[0] << 8 | a[1];
        switch (type)
        {
        case 30240: // v
        {
            ae_vec3_f v;
            fscanf(in, "%lf %lf %lf\n", &v.x, &v.y, &v.z);
            append_ae_vector(&new_model.verts, &v);
        }
        break;

        case 30324: // vt
        {
            ae_vec2_f uv;
            double temp;
            fscanf(in, "  %lf %lf %lf\n", &uv.u, &uv.v, &temp);
            append_ae_vector(&new_model.uvs, &uv);
        }
        break;

        case 30318: //vn
        {
            ae_vec3_f vn;
            fscanf(in, "  %lf %lf %lf\n", &vn.x, &vn.y, &vn.z);
            append_ae_vector(&new_model.normals, &vn);
        }
        break;

        case 26144: // f
        {
            ae_face faces[3];
            fscanf(in, "%lld/%lld/%lld %lld/%lld/%lld %lld/%lld/%lld\n", &faces[0].v_i, &faces[0].uv_i, &faces[0].norm_i, &faces[1].v_i, &faces[1].uv_i, &faces[1].norm_i, &faces[2].v_i, &faces[2].uv_i, &faces[2].norm_i);
            for (size_t i = 0; i < 3; i++)
            {
                faces[i].v_i -= 1;
                faces[i].uv_i -= 1;
                faces[i].norm_i -= 1;
            }
            append_ae_vector(&new_model.faces, &faces);
        }
        break;

        default:
            while ((a[0] = fgetc(in)) != '\n')
                ;
            break;
        }
    }
    optimize_ae_vector(&new_model.verts);
    optimize_ae_vector(&new_model.faces);
    optimize_ae_vector(&new_model.normals);
    optimize_ae_vector(&new_model.uvs);

    return new_model;
}

void close_ae_model(ae_model *model)
{
    free_ae_vector(&model->verts);
    free_ae_vector(&model->faces);
    free_ae_vector(&model->normals);
    free_ae_vector(&model->uvs);

    return;
}

size_t n_verts_ae_model(ae_model *model)
{
    return model->verts.data.quant;
}

size_t n_faces_ae_model(ae_model *model)
{
    return model->faces.data.quant;
}

void vert_ae_model(ae_model *model, size_t i, ae_vec3_f *vert)
{
    get_ae_vector(&model->verts, i, vert);
    return;
}

void face_ae_model(ae_model *model, size_t i, ae_face *face)
{
    get_ae_vector(&model->faces, i, face);
    return;
}

void uv_ae_model(ae_model *model, size_t i, ae_vec2_f *uv)
{
    get_ae_vector(&model->uvs, i, uv);
    return;
}

void norm_ae_model(ae_model *model, size_t i, ae_vec3_f *norm)
{
    get_ae_vector(&model->normals, i, norm);
    return;
}
