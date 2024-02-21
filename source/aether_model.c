#include "aether_model.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

ae_model open_ae_model(const char *filename)
{
    ae_model new_model = {
        .verts = create_ae_vector(sizeof(ae_vec3_f), 0),
        .faces = create_ae_vector(sizeof(ae_vector), 0)
    };
    FILE *in;
    if ((in = fopen(filename, "rb")) == NULL)
    {
        return new_model;
    }

    int type = 0;
    char a[2] = {0};
    while((a[0] = fgetc(in)) != EOF)
    {
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

        case 26144: //f
        {
            ae_vector faces = create_ae_vector(sizeof(size_t), 0);
            size_t v_i = 0;
            char num[65];
            char c;
            int i_c = 0;
            bool read_n = true;
            while ((c = fgetc(in)) != '\n')
            {
                if (read_n)
                {
                    if (c != '/')
                    {
                        num[i_c] = c;
                        i_c++;
                    }
                    else
                    {
                        num[i_c] = '\n';
                        v_i = atoi(num) - 1;
                        append_ae_vector(&faces, &v_i);
                        i_c = 0;
                        read_n = false;
                    }
                }
                else
                {
                    if (c == ' ')
                    {
                        read_n = true;
                    }
                }
            }

            append_ae_vector(&new_model.faces, &faces);
        }
        break;

        default:
            break;
        }

    }
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