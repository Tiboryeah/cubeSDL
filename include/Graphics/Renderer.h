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
  float *z_buffer;
  SDL_Texture *color_buffer_texture;

  float fov_factor = 600.0f;

public:
  bool renderTriangles = true;
  bool renderLines = true;
  bool renderPoints = false;
  bool renderBackface = false;
  bool isPerspective = true;
  bool useShading = true;

  void toggleTriangles() { renderTriangles = !renderTriangles; }
  void toggleLines() { renderLines = !renderLines; }
  void togglePoints() { renderPoints = !renderPoints; }
  void toggleCulling() { renderBackface = !renderBackface; }
  void togglePerspective() { isPerspective = !isPerspective; }
  void toggleShading() { useShading = !useShading; }

private:
  Vec2 project(Vec3 v) {
    if (isPerspective) {
      float z = std::max(0.1f, v.z);
      // En SDL +Y es abajo, multiplicamos math-y por -1 para que el cubo rote
      // "hacia arriba" correctamente o simplemente dejamos que el sistema de
      // coordenadas sea el que es.
      return {(v.x * fov_factor) / z + width / 2.0f,
              (v.y * fov_factor) / z + height / 2.0f};
    } else {
      float scale = 200.0f;
      return {v.x * scale + width / 2.0f, v.y * scale + height / 2.0f};
    }
  }

  uint32_t applyShading(uint32_t color, float intensity) {
    if (!useShading)
      return color;

    // Curva premium: Sombreado más profundo y progresivo
    float ambient = 0.05f;
    float power = 3.0f;
    float k = ambient +
              (1.0f - ambient) *
                  std::pow(std::max(0.0f, std::min(1.0f, intensity)), power);

    uint8_t r = (uint8_t)(((color >> 16) & 0xFF) * k);
    uint8_t g = (uint8_t)(((color >> 8) & 0xFF) * k);
    uint8_t b = (uint8_t)((color & 0xFF) * k);
    return (0xFF000000) | (r << 16) | (g << 8) | b;
  }

  void clearZBuffer() { std::fill_n(z_buffer, width * height, 10000.0f); }

public:
  Renderer(int w, int h)
      : width(w), height(h), window(nullptr), renderer(nullptr),
        color_buffer(nullptr), z_buffer(nullptr) {}

  bool init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
      return false;
    window = SDL_CreateWindow("3D Renderer (C++)", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, width, height, 0);
    if (!window)
      return false;
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer)
      return false;

    color_buffer = new uint32_t[width * height];
    z_buffer = new float[width * height];
    color_buffer_texture =
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                          SDL_TEXTUREACCESS_STREAMING, width, height);
    return true;
  }

  void clear(uint32_t color) {
    std::fill_n(color_buffer, width * height, color);
    clearZBuffer();
  }

  void drawPixel(int x, int y, float z, uint32_t color) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
      int idx = width * y + x;
      // Precision check: Usar un pequeño margen para Z-fighting
      if (z < z_buffer[idx] - 0.001f) {
        color_buffer[idx] = color;
        z_buffer[idx] = z;
      }
    }
  }

  void drawLine(int x0, int y0, float z0, int x1, int y1, float z1,
                uint32_t color) {
    int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    float dist = (float)std::sqrt(dx * dx + dy * dy);
    int step = 0;
    while (true) {
      float t = (dist <= 0) ? 0 : (float)step / dist;
      float z = z0 + (z1 - z0) * t;
      drawPixel(x0, y0, z - 0.1f, color); // Líneas siempre un poco al frente
      if (x0 == x1 && y0 == y1)
        break;
      e2 = 2 * err;
      if (e2 >= dy) {
        err += dy;
        x0 += sx;
      }
      if (e2 <= dx) {
        err += dx;
        y0 += sy;
      }
      step++;
    }
  }

  void fillTriangle(int x1, int y1, float z1, float i1, int x2, int y2,
                    float z2, float i2, int x3, int y3, float z3, float i3,
                    uint32_t color) {
    if (y1 > y2) {
      std::swap(y1, y2);
      std::swap(x1, x2);
      std::swap(z1, z2);
      std::swap(i1, i2);
    }
    if (y1 > y3) {
      std::swap(y1, y3);
      std::swap(x1, x3);
      std::swap(z1, z3);
      std::swap(i1, i3);
    }
    if (y2 > y3) {
      std::swap(y2, y3);
      std::swap(x2, x3);
      std::swap(z2, z3);
      std::swap(i2, i3);
    }

    for (int y = y1; y <= y3; y++) {
      float alpha = (y3 == y1) ? 0 : (float)(y - y1) / (y3 - y1);
      bool second_half = (y > y2 || y2 == y1);
      float beta = second_half ? (y3 == y2 ? 0 : (float)(y - y2) / (y3 - y2))
                               : (y2 == y1 ? 0 : (float)(y - y1) / (y2 - y1));

      float xS = x1 + (x3 - x1) * alpha;
      float zS = z1 + (z3 - z1) * alpha;
      float iS = i1 + (i3 - i1) * alpha;

      float xE = second_half ? x2 + (x3 - x2) * beta : x1 + (x2 - x1) * beta;
      float zE = second_half ? z2 + (z3 - z2) * beta : z1 + (z2 - z1) * beta;
      float iE = second_half ? i2 + (i3 - i2) * beta : i1 + (i2 - i1) * beta;

      if (xS > xE) {
        std::swap(xS, xE);
        std::swap(zS, zE);
        std::swap(iS, iE);
      }

      for (int x = (int)xS; x <= (int)xE; x++) {
        float phi = (xE == xS) ? 0 : (float)(x - xS) / (xE - xS);
        phi = std::max(0.0f, std::min(1.0f, phi));
        float z = zS + (zE - zS) * phi;
        float intensity = iS + (iE - iS) * phi;
        drawPixel(x, y, z, applyShading(color, intensity));
      }
    }
  }

  void renderMesh(const Mesh &mesh, float angleX, float angleY, float angleZ) {
    std::vector<Vec3> tv = mesh.vertices;
    std::vector<Vec3> vn(mesh.vertices.size(), Vec3(0, 0, 0));

    // 1. Transformación de rotación
    for (auto &v : tv) {
      v = v.rotateX(angleX).rotateY(angleY).rotateZ(angleZ);
    }

    // Normales por vertice (promedio de caras)
    for (const auto &f : mesh.faces) {
      Vec3 n = (tv[f.b] - tv[f.a]).cross(tv[f.c] - tv[f.a]);
      vn[f.a] = vn[f.a] + n;
      vn[f.b] = vn[f.b] + n;
      vn[f.c] = vn[f.c] + n;
    }
    for (auto &n : vn)
      n.normalize();

    // 2. Traslación (Z-offset para que el cubo esté "dentro" de la escena)
    for (auto &v : tv)
      v.z += 5.0f;

    // Posición de la luz para degradado (Puntual desde el hombro superior)
    Vec3 lightPos(2, -2, 0);

    for (const auto &f : mesh.faces) {
      Vec3 a = tv[f.a], b = tv[f.b], c = tv[f.c];

      // Face Normal
      Vec3 fn = (b - a).cross(c - a);

      // View vector (desde cámara en 0,0,0 hacia el objeto)
      // En orto, la cámara "ve" paralela al eje Z.
      Vec3 view = isPerspective ? a : Vec3(0, 0, 1);

      // Culling: Si fn.dot(view) < 0, la cara mira a la camara
      bool isVisible = renderBackface ? true : (fn.dot(view) < 0);

      if (isVisible) {
        Vec2 pA = project(a), pB = project(b), pC = project(c);

        if (renderTriangles) {
          // Shading: Luz puntual para crear gradiante progresivo
          auto calcInt = [&](Vec3 v, Vec3 n) {
            Vec3 dir = (v - lightPos);
            dir.normalize();
            return std::max(0.0f, n.dot(dir * -1.0f));
          };

          float i1 = calcInt(a, vn[f.a]);
          float i2 = calcInt(b, vn[f.b]);
          float i3 = calcInt(c, vn[f.c]);

          fillTriangle((int)pA.x, (int)pA.y, a.z, i1, (int)pB.x, (int)pB.y, b.z,
                       i2, (int)pC.x, (int)pC.y, c.z, i3, f.color);
        }
        if (renderLines) {
          uint32_t lc = 0xFFFFFFFF;
          drawLine((int)pA.x, (int)pA.y, a.z, (int)pB.x, (int)pB.y, b.z, lc);
          drawLine((int)pB.x, (int)pB.y, b.z, (int)pC.x, (int)pC.y, c.z, lc);
          drawLine((int)pC.x, (int)pC.y, c.z, (int)pA.x, (int)pA.y, a.z, lc);
        }
        if (renderPoints) {
          uint32_t pc = 0xFFFF0000;
          drawPixel((int)pA.x, (int)pA.y, a.z - 0.2f, pc);
          drawPixel((int)pB.x, (int)pB.y, b.z - 0.2f, pc);
          drawPixel((int)pC.x, (int)pC.y, c.z - 0.2f, pc);
        }
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
    delete[] z_buffer;
    SDL_DestroyTexture(color_buffer_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }
};

#endif
