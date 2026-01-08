#ifndef MESH_H
#define MESH_H

#include "../Math/Vec3.h"
#include <cstdint>
#include <vector>

struct Face {
  int a, b, c; // Indices de los vertices
  uint32_t color;
};

class Mesh {
public:
  std::vector<Vec3> vertices;
  std::vector<Face> faces;

  void addCube(); // Mantener la funcionalidad original
                  // Futuro: loadFromObj()
};

#endif
