#include "aether_model.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

ae_model open_ae_model(const char *m_filename, const char *t_filename)
{
    ae_model new_model = {
        .verts = create_ae_vector(sizeof(ae_vec3_f), 0),
        .faces = create_ae_vector(sizeof(ae_vector), 0),
        .uvs = create_ae_vector(sizeof(ae_vec2_f), 0),
        .texture = {.data = NULL}
    };
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
    while((a[0] = fgetc(in)) != EOF)
    {
        if (a[0] == '\n')
            continue;
        a[1] = fgetc(in);
        type = a[0] << 8 | a[1];
        switch (type)
        {
        case 30240: //v
        {
            ae_vec3_f v;
            fscanf(in, "%lf %lf %lf\n", &v.x, &v.y, &v.z);
            append_ae_vector(&new_model.verts, &v);
        }
        break;

        case 30324: //vt
        {
            ae_vec2_f uv;
            fscanf(in, "  %lf %lf", &uv.u, &uv.v);
            append_ae_vector(&new_model.uvs, &uv);
            while ((a[0] = fgetc(in)) != '\n');
        }
        break;

        case 26144: //f
        {
            ae_vector faces = create_ae_vector(sizeof(ae_face), 0);
            ae_face face;
            char num[65];
            char c;
            int i_c = 0;
            int32_t read_n = 0;
            while (read_n != -1)
            {
                c = fgetc(in);
                if (read_n < 2)
                {
                    if (c != '/')
                    {
                        num[i_c] = c;
                        i_c++;
                    }
                    else
                    {
                        num[i_c] = '\n';
                        switch(read_n)
                        {
                            case 0:
                                face.v_i = atoi(num) - 1;
                            break;

                            case 1:
                                face.uv_i = atoi(num) - 1;
                            break;

                            default:
                            break;
                        }


                        i_c = 0;
                        read_n += 1;
                    }
                }
                else
                {
                    if (c == ' ')
                    {
                        append_ae_vector(&faces, &face);
                        read_n = 0;
                    }
                    else if (c == '\n')
                    {
                        append_ae_vector(&faces, &face);
                        read_n = -1;
                    }
                }
            }
            optimize_ae_vector(&faces);
            append_ae_vector(&new_model.faces, &faces);
        }
        break;

        default:
            while ((a[0] = fgetc(in)) != '\n');
            break;
        }

    }
    optimize_ae_vector(&new_model.verts);
    optimize_ae_vector(&new_model.faces);
    optimize_ae_vector(&new_model.uvs);

    return new_model;
}

void close_ae_model(ae_model *model)
{
    free_ae_vector(&model->verts);
    for (size_t i = 0; i < model->faces.data.quant; i++)
    {
        ae_vector face;
        get_ae_vector(&model->faces, i, &face);
        free_ae_vector(&face);
    }
    free_ae_vector(&model->faces);

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

ae_vec3_f vert_ae_model(ae_model *model, size_t i)
{
    ae_vec3_f vert;
    get_ae_vector(&model->verts, i, &vert);
    return vert;
}

ae_vector face_ae_model(ae_model *model, size_t idx)
{
    ae_vector face;
    get_ae_vector(&model->faces, idx, &face);
    return face;
}

ae_vec2_f uv_ae_model(ae_model *model, size_t i)
{
    ae_vec2_f uv;
    get_ae_vector(&model->uvs, i, &uv);
    return uv;
}
