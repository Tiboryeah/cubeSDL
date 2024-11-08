#include "display.h"
#include <SDL.h>

SDL_Renderer* renderer = NULL;
SDL_Window* window = NULL;

void clear_screen(uint32_t color) {
    // Limpiar la pantalla con un color dado
    SDL_SetRenderDrawColor(renderer,
                           (color & 0xFF0000) >> 16,
                           (color & 0x00FF00) >> 8,
                           (color & 0x0000FF),
                           255);
    SDL_RenderClear(renderer);
}

void render_color_buffer(void) {
    SDL_RenderPresent(renderer);  // Presentar el buffer de renderizado
}

void draw_pixel(int x, int y, uint32_t color) {
    SDL_SetRenderDrawColor(renderer,
                           (color & 0xFF0000) >> 16,
                           (color & 0x00FF00) >> 8,
                           (color & 0x0000FF),
                           255);
    SDL_RenderDrawPoint(renderer, x, y);
}

bool init_window(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("No se pudo inicializar SDL: %s\n", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow(
        "Cubo 3D",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        printf("No se pudo crear la ventana: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("No se pudo crear el renderer: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void destroy_window(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
