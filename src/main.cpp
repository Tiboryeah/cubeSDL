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

  // Loop Principal
  while (isRunning) {
    // Input
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT)
        isRunning = false;
      if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
        isRunning = false;
    }

    // Update
    angleX += 0.01f;
    angleY += 0.02f;
    angleZ += 0.01f;

    // Render
    renderer.clear(0x000000); // Negro

    // Grid de fondo opcional o HUD podría ir aqui

    renderer.renderMesh(mesh, angleX, angleY, angleZ);

    renderer.present();
  }

  renderer.destroy();
  return 0;
}
