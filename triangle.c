#include "display.h"
#include "triangle.h"
#include <stdlib.h>  // Para la función abs()

// Algoritmo de Bresenham para dibujar líneas de manera eficiente
void draw_line_bresenham(int x0, int y0, int x1, int y1, uint32_t color) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        draw_pixel(x0, y0, color);  // Dibujamos el píxel en las coordenadas actuales

        if (x0 == x1 && y0 == y1) break;  // Si alcanzamos el final, terminamos el bucle

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

// Función auxiliar para intercambiar dos valores enteros
void swap_int(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Función para dibujar un triángulo flat-bottom (base horizontal abajo)
void draw_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    float inv_slope_1 = (float)(x1 - x0) / (y1 - y0);
    float inv_slope_2 = (float)(x2 - x0) / (y2 - y0);

    float curx1 = x0;
    float curx2 = x0;

    for (int y = y0; y <= y1; y++) {
        draw_line_bresenham((int)curx1, y, (int)curx2, y, color);
        curx1 += inv_slope_1;
        curx2 += inv_slope_2;
    }
}

// Función para dibujar un triángulo flat-top (base horizontal arriba)
void draw_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    float inv_slope_1 = (float)(x2 - x0) / (y2 - y0);
    float inv_slope_2 = (float)(x2 - x1) / (y2 - y1);

    float curx1 = x2;
    float curx2 = x2;

    for (int y = y2; y >= y0; y--) {
        draw_line_bresenham((int)curx1, y, (int)curx2, y, color);
        curx1 -= inv_slope_1;
        curx2 -= inv_slope_2;
    }
}

// Función general para dibujar triángulos usando la técnica flat-top/flat-bottom
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    // Ordenar los puntos para que y0 <= y1 <= y2
    if (y0 > y1) { swap_int(&x0, &x1); swap_int(&y0, &y1); }
    if (y1 > y2) { swap_int(&x1, &x2); swap_int(&y1, &y2); }
    if (y0 > y1) { swap_int(&x0, &x1); swap_int(&y0, &y1); }

    // Caso 1: triángulo flat-bottom
    if (y1 == y2) {
        draw_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
    }
    // Caso 2: triángulo flat-top
    else if (y0 == y1) {
        draw_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
    }
    // Caso 3: triángulo general, que se divide en un flat-bottom y un flat-top
    else {
        // Encontrar el punto de división utilizando un cálculo más preciso
        int my = y1;
        int mx = x0 + (int)((float)(y1 - y0) / (float)(y2 - y0) * (x2 - x0) + 0.5f); // Redondear correctamente

        draw_flat_bottom_triangle(x0, y0, x1, y1, mx, my, color);
        draw_flat_top_triangle(x1, y1, mx, my, x2, y2, color);
    }
}
