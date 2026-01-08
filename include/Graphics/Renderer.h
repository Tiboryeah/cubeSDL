#ifndef RENDERER_H
#define RENDERER_H

#include "../Graphics/Mesh.h"
#include "../Math/Vec2.h"
#include "../Math/Vec3.h"
#include <SDL.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <vector>

class Renderer {
private:
  SDL_Window *window;
  SDL_Renderer *renderer;
  int width, height;
  uint32_t *color_buffer;
  SDL_Texture *color_buffer_texture;

  // Parametros de camara
  float fov_factor = 640.0f; // Ajuste manual de FOV

  // Render Flags
  bool renderTriangles = true;
  bool renderLines = true;
  bool renderPoints = false;
  bool renderBackface =
      false; // Por defecto culling enabled (no renderizar backfaces)
  bool isPerspective = true;

  Vec2 project(Vec3 v) {
    if (isPerspective) {
      Vec2 projected = {(v.x * fov_factor) / v.z + width / 2.0f,
                        (v.y * fov_factor) / v.z + height / 2.0f};
      return projected;
    } else {
      // Ortogonal simple (ignorar Z para escala)
      float scale = 120.0f; // Zoom fijo
      Vec2 projected = {v.x * scale + width / 2.0f,
                        v.y * scale + height / 2.0f};
      return projected;
    }
  }

public:
  // Toggle methods
  void toggleTriangles() { renderTriangles = !renderTriangles; }
  void toggleLines() { renderLines = !renderLines; }
  void togglePoints() { renderPoints = !renderPoints; }
  void toggleCulling() { renderBackface = !renderBackface; }
  void togglePerspective() { isPerspective = !isPerspective; }

public:
  Renderer(int w, int h)
      : width(w), height(h), window(nullptr), renderer(nullptr) {}

  bool init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
      return false;
    window = SDL_CreateWindow("3D Renderer (C++)", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, width, height, 0);
    if (!window)
      return false;
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer)
      return false; // Fallback a software si falla hardware es automático
                    // usualmente

    color_buffer = new uint32_t[width * height];
    color_buffer_texture =
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                          SDL_TEXTUREACCESS_STREAMING, width, height);
    return true;
  }

  void clear(uint32_t color) {
    std::fill_n(color_buffer, width * height, color);
  }

  void drawPixel(int x, int y, uint32_t color) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
      color_buffer[width * y + x] = color;
    }
  }

  // Algoritmo DDA para lineas
  void drawLine(int x0, int y0, int x1, int y1, uint32_t color) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int steps = std::abs(dx) > std::abs(dy) ? std::abs(dx) : std::abs(dy);
    float xInc = dx / (float)steps;
    float yInc = dy / (float)steps;
    float x = x0;
    float y = y0;
    for (int i = 0; i <= steps; i++) {
      drawPixel(std::round(x), std::round(y), color);
      x += xInc;
      y += yInc;
    }
  }

  // Pipeline simple
  void renderMesh(const Mesh &mesh, float angleX, float angleY, float angleZ) {
    // Estructura para ordenar caras
    struct SortedFace {
      int id;
      float depth;
    };
    std::vector<SortedFace> facesToDraw;

    // 1. Transformacion y Visibilidad
    std::vector<Vec3> transformedVertices =
        mesh.vertices; // Copia para transformar

    for (auto &v : transformedVertices) {
      v = v.rotateX(angleX).rotateY(angleY).rotateZ(angleZ);

      if (isPerspective) {
        v.z += 5.0f; // Traslación de cámara
      } else {
        v.z += 0.0f; // En ortogonal no importa tanto, pero centrado
      }
    }

    for (int i = 0; i < mesh.faces.size(); i++) {
      const Face &f = mesh.faces[i];
      Vec3 a = transformedVertices[f.a];
      Vec3 b = transformedVertices[f.b];
      Vec3 c = transformedVertices[f.c];

      // Backface Culling
      Vec3 ab = b - a;
      Vec3 ac = c - a;
      Vec3 normal = ab.cross(ac);
      Vec3 cameraRay = a - Vec3(0, 0, 0);

      // Si renderBackface es true, dibujamos todo.
      // Si es false, solo dibujamos si la normal apunta a camara (< 0)
      if (renderBackface || normal.dot(cameraRay) < 0) {
        float avgDepth = (a.z + b.z + c.z) / 3.0f;
        facesToDraw.push_back({i, avgDepth});
      }
    }

    // 2. Ordenamiento
    std::sort(facesToDraw.begin(), facesToDraw.end(),
              [](const SortedFace &a, const SortedFace &b) {
                return a.depth > b.depth;
              });

    // 3. Proyeccion y Dibujado
    for (auto &sf : facesToDraw) {
      const Face &f = mesh.faces[sf.id];
      Vec2 pA = project(transformedVertices[f.a]);
      Vec2 pB = project(transformedVertices[f.b]);
      Vec2 pC = project(transformedVertices[f.c]);

      if (renderTriangles) {
        // Dibuja triangulo solido
        drawLine(pA.x, pA.y, pB.x, pB.y, f.color);
        drawLine(pB.x, pB.y, pC.x, pC.y, f.color);
        drawLine(pC.x, pC.y, pA.x, pA.y, f.color);
        // *Aqui iria el relleno real (fillTriangle)*
      }

      if (renderLines) {
        uint32_t wireColor = 0xFFFFFFFF; // Blanco
        drawLine(pA.x, pA.y, pB.x, pB.y, wireColor);
        drawLine(pB.x, pB.y, pC.x, pC.y, wireColor);
        drawLine(pC.x, pC.y, pA.x, pA.y, wireColor);
      }

      if (renderPoints) {
        uint32_t vertColor = 0xFFFF0000; // Rojo
        drawPixel(pA.x, pA.y, vertColor);
        drawPixel(pB.x, pB.y, vertColor);
        drawPixel(pC.x, pC.y, vertColor);
      }
    }
  }

  void present() {
    SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer,
                      width * sizeof(uint32_t));
    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
    SDL_RenderPresent(renderer);
  }

  void destroy() {
    delete[] color_buffer;
    SDL_DestroyTexture(color_buffer_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }

  SDL_Renderer *getSDLRenderer() { return renderer; } // Backdoor si se necesita
};

#endif
