#include "../include/Graphics/Mesh.h"

void Mesh::addCube() {
  // Vertices del cubo (-1 a 1)
  vertices = {
      Vec3(-1, -1, -1), // 1
      Vec3(-1, 1, -1),  // 2
      Vec3(1, 1, -1),   // 3
      Vec3(1, -1, -1),  // 4
      Vec3(1, 1, 1),    // 5
      Vec3(1, -1, 1),   // 6
      Vec3(-1, 1, 1),   // 7
      Vec3(-1, -1, 1)   // 8
  };

  // Caras (Indices de vertices, 1-based en dibujo original, 0-based aqui)
  faces = {// Front
           {0, 1, 2, 0xFF0000},
           {0, 2, 3, 0xFF0000},
           // Right
           {3, 2, 4, 0x00FF00},
           {3, 4, 5, 0x00FF00},
           // Back
           {5, 4, 6, 0x0000FF},
           {5, 6, 7, 0x0000FF},
           // Left
           {7, 6, 1, 0xFFFF00},
           {7, 1, 0, 0xFFFF00},
           // Top
           {1, 6, 4, 0xFF00FF},
           {1, 4, 2, 0xFF00FF},
           // Bottom
           {5, 7, 0, 0x00FFFF},
           {5, 0, 3, 0x00FFFF}};
}
