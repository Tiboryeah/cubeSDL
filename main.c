#include <SDL.h>
#include <stdlib.h>
#include "display.h"
#include "mesh.h"
#include "vector.h"

// Banderas de visibilidad
bool render_triangles = true;
bool render_vertices = false;
bool render_edges = true;
bool is_perspective = true;

typedef struct {
    face_t face;
    Uint32 color;
    float avg_depth; // Profundidad promedio del triángulo
} depth_sorted_face_t;

depth_sorted_face_t sorted_faces[12];
Uint32 triangle_colors[] = {
    0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00,
    0xFF00FF, 0x00FFFF, 0xFFA500, 0x800080,
    0x008000, 0x000080, 0x808080, 0xFFFFFF
};

// Inicializar los colores de los triángulos
void initialize_colored_faces() {
    for (int i = 0; i < 12; i++) {
        sorted_faces[i].face = mesh.faces[i];
        sorted_faces[i].color = triangle_colors[i % 12];
    }
}

// Asegura que al menos un elemento esté visible en pantalla
void ensure_visibility() {
    if (!render_triangles && !render_edges && !render_vertices) {
        render_vertices = true;  // Activar triángulos como predeterminado si todo esta apagado
    }
}

// Procesar entradas de teclado
void process_input(bool* is_running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *is_running = false;
        }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_f:
                    render_triangles = !render_triangles;
                    break;
                case SDLK_v:
                    render_vertices = !render_vertices;
                    break;
                case SDLK_l:
                    render_edges = !render_edges;
                    break;
                case SDLK_p:
                    is_perspective = !is_perspective;
                    break;
            }
        }
    }

    // Asegura que al menos una bandera de renderizado esté activa
    ensure_visibility();
}

// Comparador para ordenar triángulos por profundidad en orden descendente
int compare_faces(const void* a, const void* b) {
    depth_sorted_face_t* face_a = (depth_sorted_face_t*)a;
    depth_sorted_face_t* face_b = (depth_sorted_face_t*)b;
    if (face_a->avg_depth < face_b->avg_depth) return 1;
    if (face_a->avg_depth > face_b->avg_depth) return -1;
    return 0;
}

// Función para verificar si un triángulo es visible (orientado hacia la cámara) en perspectiva
bool is_triangle_visible(vec3_t a, vec3_t b, vec3_t c) {
    vec3_t ab = vec3_sub(b, a);
    vec3_t ac = vec3_sub(c, a);
    vec3_t normal = vec3_cross(ab, ac);  // Producto cruzado para obtener la normal del triángulo
    return normal.z < 0;  // Verificar si la normal apunta hacia la cámara
}

// Función para calcular la profundidad promedio y ordenar los triángulos
void render(float angle_x, float angle_y, float angle_z) {
    clear_screen(0x000000);

    int visible_face_count = 0;

    for (int i = 0; i < 12; i++) {
        face_t face = mesh.faces[i];
        vec3_t point_a = vec3_rotate_z(vec3_rotate_y(vec3_rotate_x(mesh.vertices[face.a], angle_x), angle_y), angle_z);
        vec3_t point_b = vec3_rotate_z(vec3_rotate_y(vec3_rotate_x(mesh.vertices[face.b], angle_x), angle_y), angle_z);
        vec3_t point_c = vec3_rotate_z(vec3_rotate_y(vec3_rotate_x(mesh.vertices[face.c], angle_x), angle_y), angle_z);

        // Aplicar backface culling
        if (!is_triangle_visible(point_a, point_b, point_c)) continue;

        float avg_depth = (point_a.z + point_b.z + point_c.z) / 3.0f;
        sorted_faces[visible_face_count++] = (depth_sorted_face_t){ .face = face, .color = triangle_colors[i % 12], .avg_depth = avg_depth };
    }

    // Ordenar los triángulos visibles por la profundidad promedio
    qsort(sorted_faces, visible_face_count, sizeof(depth_sorted_face_t), compare_faces);

    // Dibujar triángulos en orden de profundidad
    for (int i = 0; i < visible_face_count; i++) {
        depth_sorted_face_t sorted_face = sorted_faces[i];
        vec2_t projected_a = project(vec3_rotate_z(vec3_rotate_y(vec3_rotate_x(mesh.vertices[sorted_face.face.a], angle_x), angle_y), angle_z));
        vec2_t projected_b = project(vec3_rotate_z(vec3_rotate_y(vec3_rotate_x(mesh.vertices[sorted_face.face.b], angle_x), angle_y), angle_z));
        vec2_t projected_c = project(vec3_rotate_z(vec3_rotate_y(vec3_rotate_x(mesh.vertices[sorted_face.face.c], angle_x), angle_y), angle_z));

        if (render_triangles) {
            draw_triangle(projected_a.x, projected_a.y, projected_b.x, projected_b.y, projected_c.x, projected_c.y, sorted_face.color);
        }

        if (render_edges) {
            draw_line_bresenham(projected_a.x, projected_a.y, projected_b.x, projected_b.y, 0xFFFFFF);
            draw_line_bresenham(projected_b.x, projected_b.y, projected_c.x, projected_c.y, 0xFFFFFF);
            draw_line_bresenham(projected_c.x, projected_c.y, projected_a.x, projected_a.y, 0xFFFFFF);
        }

        if (render_vertices) {
            draw_pixel(projected_a.x, projected_a.y, 0xFFFF0000);
            draw_pixel(projected_b.x, projected_b.y, 0xFFFF0000);
            draw_pixel(projected_c.x, projected_c.y, 0xFFFF0000);
        }
    }

    render_color_buffer();
    SDL_RenderPresent(renderer);
}

// main loop con actualización de ángulos de rotación
int SDL_main(int argc, char* argv[]) {
    bool is_running = init_window();
    if (!is_running) return -1;

    load_cube_mesh_data();
    initialize_colored_faces();

    float angle_x = 0.0f;
    float angle_y = 0.0f;
    float angle_z = 0.0f;
    const float ROTATION_SPEED = 1.0f;

    Uint32 last_frame_time = 0;

    while (is_running) {
        Uint32 frame_start = SDL_GetTicks();
        float delta_time = (frame_start - last_frame_time) / 1000.0f;
        last_frame_time = frame_start;

        process_input(&is_running);

        // Actualizar ángulos de rotación
        angle_x += ROTATION_SPEED * delta_time;
        angle_y += ROTATION_SPEED * delta_time;
        angle_z += ROTATION_SPEED * delta_time;

        // Renderizar el cubo con rotación y control de visibilidad
        render(angle_x, angle_y, angle_z);
    }

    destroy_window();
    return 0;
}
