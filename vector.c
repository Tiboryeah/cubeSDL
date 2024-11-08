#include "vector.h"
#include <math.h>
#include "display.h"
#include <stdio.h>

extern bool is_perspective;  // Si necesitas usarla en otros archivos también

// Funciones de Vector 2D
float vec2_length(vec2_t v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

vec2_t vec2_add(vec2_t a, vec2_t b) {
    return (vec2_t){a.x + b.x, a.y + b.y};
}

vec2_t vec2_sub(vec2_t a, vec2_t b) {
    return (vec2_t){a.x - b.x, a.y - b.y};
}

vec2_t vec2_mul(vec2_t v, float factor) {
    return (vec2_t){v.x * factor, v.y * factor};
}

vec2_t vec2_div(vec2_t v, float factor) {
    return (vec2_t){v.x / factor, v.y / factor};
}

void vec2_normalize(vec2_t* v) {
    float length = vec2_length(*v);
    if (length != 0) {
        v->x /= length;
        v->y /= length;
    }
}

// Funciones de Vector 3D
float vec3_length(vec3_t v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3_t vec3_add(vec3_t a, vec3_t b) {
    return (vec3_t){a.x + b.x, a.y + b.y, a.z + b.z};
}

vec3_t vec3_sub(vec3_t a, vec3_t b) {
    return (vec3_t){a.x - b.x, a.y - b.y, a.z - b.z};
}

vec3_t vec3_mul(vec3_t v, float factor) {
    return (vec3_t){v.x * factor, v.y * factor, v.z * factor};
}

vec3_t vec3_div(vec3_t v, float factor) {
    return (vec3_t){v.x / factor, v.y / factor, v.z / factor};
}

vec3_t vec3_cross(vec3_t a, vec3_t b) {
    return (vec3_t){a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

float vec3_dot(vec3_t a, vec3_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

void vec3_normalize(vec3_t* v) {
    float length = vec3_length(*v);
    if (length != 0) {
        v->x /= length;
        v->y /= length;
        v->z /= length;
    }
}

// Implementación de vec4_from_vec3
vec4_t vec4_from_vec3(vec3_t v) {
    return (vec4_t){v.x, v.y, v.z, 1.0f};  // La coordenada w se establece en 1.0 por defecto
}

// Implementación de vec3_from_vec4
vec3_t vec3_from_vec4(vec4_t v) {
    return (vec3_t){v.x, v.y, v.z};  // Se ignora la coordenada w
}

/*vec2_t project(vec3_t point) {
    vec2_t projected_point;
    float fov_factor = 640.0;  // Ajusta según tu necesidad
    float z_offset = 5.0;  // Desplaza el cubo en el eje Z para verlo mejor

    // Evitamos dividir por cero
    float z = point.z + z_offset;
    if (z == 0) z = 0.1;  // Evita que el punto z sea exactamente 0

    // Proyección en 2D desde el espacio 3D
    projected_point.x = (point.x / z) * fov_factor;
    projected_point.y = (point.y / z) * fov_factor;

    // Centramos la proyección en la ventana
    projected_point.x += window_width / 2;
    projected_point.y += window_height / 2;

    return projected_point;
}
*/
#include "vector.h"
#include "display.h"

extern bool is_perspective;

// Configuración de parámetros de la cámara virtual
float fov = 60.0f;  // Reducir el campo de visión para una proyección más controlada
float near_plane = 1.0f;  // Distancia mínima de la cámara para evitar un acercamiento extremo
float far_plane = 100.0f; // Distancia máxima de la cámara

// Función para convertir grados a radianes
float degrees_to_radians(float degrees) {
    return degrees * (3.14159f / 180.0f);
}

// Función de proyección en perspectiva utilizando una matriz de proyección
vec2_t project(vec3_t point) {
    vec2_t projected_point;

    if (is_perspective) {
        // Parámetros simplificados para evitar proyecciones exageradas
        float fov_factor = 600.0f;  // Reducido para evitar tamaño excesivo
        float z_offset = 5.0f;      // Alejamos el cubo para evitar distorsión

        // Asegurar que el valor de `z` no sea demasiado pequeño para evitar distorsión
        float z = point.z + z_offset;
        if (z < 1.0f) z = 1.0f;  // Evitar que z se acerque demasiado

        // Proyección en perspectiva ajustada y centrada en pantalla
        projected_point.x = (point.x / z) * fov_factor + (WINDOW_WIDTH / 2);
        projected_point.y = (point.y / z) * fov_factor + (WINDOW_HEIGHT / 2);
    } else {
        // Proyección ortográfica (sin cambios)
        float scale_factor = 100.0f;
        projected_point.x = (point.x * scale_factor) + (WINDOW_WIDTH / 2);
        projected_point.y = (point.y * scale_factor) + (WINDOW_HEIGHT / 2);
    }

    return projected_point;
}


vec3_t vec3_rotate_x(vec3_t v, float angle) {
    vec3_t result;
    result.x = v.x;
    result.y = v.y * cos(angle) - v.z * sin(angle);
    result.z = v.y * sin(angle) + v.z * cos(angle);
    return result;
}

vec3_t vec3_rotate_y(vec3_t v, float angle) {
    vec3_t result;
    result.x = v.x * cos(angle) + v.z * sin(angle);
    result.y = v.y;
    result.z = -v.x * sin(angle) + v.z * cos(angle);
    return result;
}

vec3_t vec3_rotate_z(vec3_t v, float angle) {
    vec3_t result;
    result.x = v.x * cos(angle) - v.y * sin(angle);
    result.y = v.x * sin(angle) + v.y * cos(angle);
    result.z = v.z;
    return result;
}

