#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "../Graphics/Mesh.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


class OBJLoader {
public:
  static bool load(const std::string &filename, Mesh &mesh) {
    std::ifstream file(filename);
    if (!file.is_open()) {
      std::cerr << "Error: No se pudo abrir " << filename << std::endl;
      return false;
    }

    std::vector<Vec3> temp_vertices;
    std::string line;

    while (std::getline(file, line)) {
      std::stringstream ss(line);
      std::string prefix;
      ss >> prefix;

      if (prefix == "v") { // Vertice
        Vec3 v;
        ss >> v.x >> v.y >> v.z;
        mesh.vertices.push_back(v);
      } else if (prefix == "f") { // Cara
        Face f;
        // Parseo simplificado para formatos "v", "v/vt", "v/vt/vn"
        // Asumimos triangulos (3 indices)
        std::string segment;
        int indices[3];
        for (int i = 0; i < 3; i++) {
          ss >> segment;
          // Buscar primer '/' para tomar solo el indice del vertice
          size_t slash = segment.find('/');
          if (slash != std::string::npos) {
            indices[i] = std::stoi(segment.substr(0, slash));
          } else {
            indices[i] = std::stoi(segment);
          }
        }
        // OBJ usa indices 1-based, pasamos a 0-based
        f.a = indices[0] - 1;
        f.b = indices[1] - 1;
        f.c = indices[2] - 1;
        f.color = 0xFFFFFF; // Blanco por defecto
        mesh.faces.push_back(f);
      }
    }

    std::cout << "Modelo cargado: " << mesh.vertices.size() << " vertices, "
              << mesh.faces.size() << " caras.\n";
    return true;
  }
};

#endif
