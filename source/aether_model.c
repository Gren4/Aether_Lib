// based on https://github.com/ssloy/tinyrenderer/wiki
#include "aether_model.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define VERTICES_FLAG 30240
#define FACES_FLAG 26144
#define UVS_FLAG 30324
#define NORMALS_FLAG 30318

ae_model open_ae_model(const char *m_filename, const char *t_filename, const ae_nm_type nm_type, const char *nm_filename, const char *sm_filename)
{
    ae_model new_model = {
        .verts = create_ae_vector(sizeof(ae_vec3_f), 0),
        .faces = create_ae_vector(sizeof(ae_face) * 3, 0),
        .uvs = create_ae_vector(sizeof(ae_vec2_f), 0),
        .normals = create_ae_vector(sizeof(ae_vec3_f), 0),
        .texture = {.data = NULL},
        .nm_type = AE_NM_NONE,
        .normal_map = {.data = NULL},
        .specular_map = {.data = NULL}};

    FILE *in;
    if ((in = fopen(m_filename, "rb")) == NULL)
    {
        return new_model;
    }

    if (t_filename != NULL)
    {
        if (read_file_ae_tga(&new_model.texture, t_filename))
            flip_vertically_ae_tga(&new_model.texture);
    }

    if (nm_filename != NULL)
    {
        if (read_file_ae_tga(&new_model.normal_map, nm_filename))
        {
            new_model.nm_type = nm_type;
            flip_vertically_ae_tga(&new_model.normal_map);
        }
    }

    if (sm_filename != NULL)
    {
        if (read_file_ae_tga(&new_model.specular_map, sm_filename))
            flip_vertically_ae_tga(&new_model.specular_map);
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
        case VERTICES_FLAG:
        {
            ae_vec3_f v;
            fscanf(in, "%lf %lf %lf\n", &v.x, &v.y, &v.z);
            *(ae_vec3_f*)append_ae_vector(&new_model.verts) = v;
        }
        break;

        case UVS_FLAG:
        {
            ae_vec2_f uv;
            double temp;
            fscanf(in, "  %lf %lf %lf\n", &uv.u, &uv.v, &temp);
            *(ae_vec2_f*)append_ae_vector(&new_model.uvs) = uv;
        }
        break;

        case NORMALS_FLAG:
        {
            ae_vec3_f vn;
            fscanf(in, "  %lf %lf %lf\n", &vn.x, &vn.y, &vn.z);
            *(ae_vec3_f*)append_ae_vector(&new_model.normals) = vn;
        }
        break;

        case FACES_FLAG:
        {
            ae_face faces[3];
            fscanf(in, "%lld/%lld/%lld %lld/%lld/%lld %lld/%lld/%lld\n", &faces[0].v_i, &faces[0].uv_i, &faces[0].norm_i, &faces[1].v_i, &faces[1].uv_i, &faces[1].norm_i, &faces[2].v_i, &faces[2].uv_i, &faces[2].norm_i);
            for (size_t i = 0; i < 3; i++)
            {
                faces[i].v_i -= 1;
                faces[i].uv_i -= 1;
                faces[i].norm_i -= 1;
            }
            memcpy(append_ae_vector(&new_model.faces), faces, sizeof(ae_face) * 3);
        }
        break;

        default:
            while (true)
            {
                a[0] = fgetc(in);
                if (a[0] == '\n' || a[0] == EOF)
                    break;
            }
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

size_t n_verts_ae_model(ae_model const *model)
{
    return model->verts.data.quant;
}

size_t n_faces_ae_model(ae_model const *model)
{
    return model->faces.data.quant;
}

ae_vec3_f vert_ae_model(ae_model const *model, const size_t i)
{
    return *(ae_vec3_f*)get_ae_vector(&model->verts, i);
}

void* face_ae_model(ae_model const *model, const size_t i)
{
    return get_ae_vector(&model->faces, i);
}

ae_vec2_f uv_ae_model(ae_model const *model, const size_t i)
{
    return *(ae_vec2_f*)get_ae_vector(&model->uvs, i);
}

ae_vec3_f normal_ae_model(ae_model const *model, const size_t i)
{
    return *(ae_vec3_f*)get_ae_vector(&model->normals, i);
}

ae_tga_c texture_ae_model(ae_model const *model, ae_vec2_f const *uv)
{
    AE_TGA_C_RGBA(new_color, 255, 255, 255, 255);
    if (model->texture.data != NULL)
        new_color = get_ae_tga(&model->texture, uv->u * model->texture.width, uv->v * model->texture.height);
    return new_color;
}

ae_vec3_f normal_map_ae_model(ae_model const *model, ae_vec2_f const *uv)
{
    ae_vec3_f normal_map = {.x = 0.0, .y = 0.0, .z = 0.0};

    if (model->normal_map.data != NULL)
    {
        AE_TGA_C_RGBA(new_color, 0, 0, 0, 0);
        new_color = get_ae_tga(&model->normal_map, uv->u * model->normal_map.width, uv->v * model->normal_map.height);
        normal_map.x = ((double)new_color.r / 255.0) * 2.0 - 1.0;
        normal_map.y = ((double)new_color.g / 255.0) * 2.0 - 1.0;
        normal_map.z = ((double)new_color.b / 255.0) * 2.0 - 1.0;
    }

    return normal_map;
}

double specular_map_ae_model(ae_model const *model, ae_vec2_f const *uv)
{
    double coeff = 0.0;

    if (model->specular_map.data != NULL)
    {
        AE_TGA_C_RGBA(new_color, 0, 0, 0, 0);
        new_color = get_ae_tga(&model->specular_map, uv->u * model->specular_map.width, uv->v * model->specular_map.height);
        coeff = new_color.raw[0];
    }

    return coeff;
}
