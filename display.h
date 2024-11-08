#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_Texture* color_buffer_texture;
extern uint32_t* color_buffer;
extern int window_width;
extern int window_height;

#define WINDOW_WIDTH 800    // Cambia según el tamaño real de tu ventana
#define WINDOW_HEIGHT 800   // Cambia según el tamaño real de tu ventana

bool init_window(void);
void clear_screen(uint32_t color);
void render_frame(void);
void process_input(bool* is_running);
void destroy_window(void);
void render_color_buffer(void);


void draw_pixel(int x, int y, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);

#endif  // DISPLAY_H
