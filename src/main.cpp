#include "../include/Graphics/Mesh.h"
#include "../include/Graphics/OBJLoader.h"
#include "../include/Graphics/Renderer.h"
#include <SDL.h>
#include <iostream>

int main(int argc, char *argv[]) {
  // Configuración
  const int WIDTH = 800;
  const int HEIGHT = 600;

  Renderer renderer(WIDTH, HEIGHT);
  if (!renderer.init()) {
    std::cerr << "Fallo al iniciar SDL.\n";
    return 1;
  }

  // Cargar Modelo
  Mesh mesh;
  // Intentamos cargar un OBJ si existe, si no, cargamos el cubo por defecto
  if (!OBJLoader::load("model.obj", mesh)) {
    std::cout << "No se encontro model.obj, cargando cubo por defecto.\n";
    mesh.addCube();
  }

  bool isRunning = true;
  SDL_Event event;

  float angleX = 0, angleY = 0, angleZ = 0;

  // Control de Tiempo (60 FPS)
  const int FPS = 60;
  const int FRAME_DELAY = 1000 / FPS;
  Uint32 frameStart;
  int frameTime;

  // Loop Principal
  while (isRunning) {
    frameStart = SDL_GetTicks();

    // Input
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT)
        isRunning = false;
      if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
          isRunning = false;
          break;
        case SDLK_f:
          renderer.toggleTriangles();
          break;
        case SDLK_l:
          renderer.toggleLines();
          break;
        case SDLK_v:
          renderer.togglePoints();
          break;
        case SDLK_p:
          renderer.togglePerspective();
          break;
        case SDLK_c:
          renderer.toggleCulling();
          break;
        }
      }
    }

    // Update
    angleX += 0.01f;
    angleY += 0.01f;
    angleZ += 0.01f;

    // Render
    renderer.clear(0x000000); // Negro

    renderer.renderMesh(mesh, angleX, angleY, angleZ);

    renderer.present();

    // Frame Cap
    frameTime = SDL_GetTicks() - frameStart;
    if (FRAME_DELAY > frameTime) {
      SDL_Delay(FRAME_DELAY - frameTime);
    }
  }

  renderer.destroy();
  return 0;
}
