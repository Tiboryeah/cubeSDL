#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mesh.h"
#include "array.h"

mesh_t mesh = {NULL, NULL};



void load_cube_mesh_data(void) {
    FILE* file = fopen("C:/Users/USER/Desktop/Graficos/sdl2-basic/sdl2-basic/cube.obj", "r");
    if (!file) {
        printf("Error al abrir el archivo cube.obj\n");
        return;
    }

    char line[128];
    while (fgets(line, sizeof(line), file)) {
        // Leer vértices
        if (strncmp(line, "v ", 2) == 0) {
            vec3_t vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            array_push(mesh.vertices, vertex);
            printf("Vértice cargado: (%f, %f, %f)\n", vertex.x, vertex.y, vertex.z);  // Debug
        }
        // Leer caras ignorando texturas y normales
        else if (strncmp(line, "f ", 2) == 0) {
            face_t face;
            int vertex_indices[3];
            sscanf(line, "f %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &vertex_indices[0], &vertex_indices[1], &vertex_indices[2]);
            face.a = vertex_indices[0] - 1;  // Convertir a índice de 0
            face.b = vertex_indices[1] - 1;
            face.c = vertex_indices[2] - 1;
            face.color = 0xFFFFFF;  // Blanco por defecto
            array_push(mesh.faces, face);
            printf("Cara cargada: (%d, %d, %d)\n", face.a, face.b, face.c);  // Debug
        }
    }

}

