#ifndef INC_3DRENDERER_TRIANGLE_H
#define INC_3DRENDERER_TRIANGLE_H

#include <stdint.h>
#include "vector.h"
#include "display.h"

typedef struct {
    int a;
    int b;
    int c;
    uint32_t color;
} face_t;

typedef struct {
    vec2_t points[3];
} triangle_t;

void draw_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void draw_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void swap_int(int* a, int* b);

void draw_line_bresenham(int x0, int y0, int x1, int y1, uint32_t color);  // Declarar la nueva función

#endif //INC_3DRENDERER_TRIANGLE_H
