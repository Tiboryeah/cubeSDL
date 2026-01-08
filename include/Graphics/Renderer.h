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

public:
  // Render Flags
  bool renderTriangles = true;
  bool renderLines = true;
  bool renderPoints = false;
  bool renderBackface = false; // Por defecto culling enabled
  bool isPerspective = true;

  // Toggle methods
  void toggleTriangles() { renderTriangles = !renderTriangles; }
  void toggleLines() { renderLines = !renderLines; }
  void togglePoints() { renderPoints = !renderPoints; }
  void toggleCulling() { renderBackface = !renderBackface; }
  void togglePerspective() { isPerspective = !isPerspective; }

private:
  Vec2 project(Vec3 v) {
    if (isPerspective) {
      // Evitar división por cero o Z negativo (detrás de cámara)
      float z = v.z;
      if (z < 0.1f)
        z = 0.1f;
      Vec2 projected = {(v.x * fov_factor) / z + width / 2.0f,
                        (v.y * fov_factor) / z + height / 2.0f};
      return projected;
    } else {
      // Ortogonal: escala mucho mayor para que sea visible
      float scale = 150.0f;
      Vec2 projected = {v.x * scale + width / 2.0f,
                        v.y * scale + height / 2.0f};
      return projected;
    }
  }

  // Constructor y metodos de inicialización

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

  // Algoritmo de Rasterización Standard - Relleno de Triangulos
  void fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3,
                    uint32_t color) {
    // Ordenar vértices por Y descendente (y1 <= y2 <= y3)
    if (y1 > y2) {
      std::swap(y1, y2);
      std::swap(x1, x2);
    }
    if (y1 > y3) {
      std::swap(y1, y3);
      std::swap(x1, x3);
    }
    if (y2 > y3) {
      std::swap(y2, y3);
      std::swap(x2, x3);
    }

    auto drawScanline = [&](int y, int xA, int xB) {
      if (xA > xB)
        std::swap(xA, xB);
      for (int x = xA; x <= xB; x++) {
        drawPixel(x, y, color);
      }
    };

    // Alturas de los segmentos
    int h1 = y2 - y1;
    int h2 = y3 - y2;
    int h_total = y3 - y1;

    if (h_total == 0)
      return;

    // Parte superior (V1-V2)
    for (int y = y1; y <= y2; y++) {
      int dy = y - y1;
      float alpha = (float)dy / h_total;
      float beta = (h1 == 0) ? 0 : (float)dy / h1;
      int xA = x1 + (x3 - x1) * alpha;
      int xB = x1 + (x2 - x1) * beta;
      drawScanline(y, xA, xB);
    }

    // Parte inferior (V2-V3)
    for (int y = y2 + 1; y <= y3; y++) {
      int dy_total = y - y1;
      int dy_seg = y - y2;
      float alpha = (float)dy_total / h_total;
      float beta = (h2 == 0) ? 0 : (float)dy_seg / h2;
      int xA = x1 + (x3 - x1) * alpha;
      int xB = x2 + (x3 - x2) * beta;
      drawScanline(y, xA, xB);
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
    std::vector<Vec3> transformedVertices = mesh.vertices;

    for (auto &v : transformedVertices) {
      v = v.rotateX(angleX).rotateY(angleY).rotateZ(angleZ);

      if (isPerspective) {
        v.z += 5.0f; // Traslación de cámara
      } else {
        v.z += 0.0f;
      }
    }

    for (int i = 0; i < mesh.faces.size(); i++) {
      const Face &f = mesh.faces[i];
      Vec3 a = transformedVertices[f.a];
      Vec3 b = transformedVertices[f.b];
      Vec3 c = transformedVertices[f.c];

      Vec3 ab = b - a;
      Vec3 ac = c - a;
      Vec3 normal = ab.cross(ac);

      // Backface Culling logic adaptable
      bool is_visible = false;
      if (renderBackface) {
        is_visible = true;
      } else {
        if (isPerspective) {
          Vec3 cameraRay = a - Vec3(0, 0, 0);
          is_visible = (normal.dot(cameraRay) < 0);
        } else {
          // En ortogonal los rayos son paralelos (mirando hacia Z profundo)
          is_visible = (normal.z < 0);
        }
      }

      if (is_visible) {
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
        fillTriangle(pA.x, pA.y, pB.x, pB.y, pC.x, pC.y, f.color);
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
