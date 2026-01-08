#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

struct Vec3 {
  float x, y, z;

  Vec3() : x(0), y(0), z(0) {}
  Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

  // Operadores
  Vec3 operator+(const Vec3 &v) const {
    return Vec3(x + v.x, y + v.y, z + v.z);
  }
  Vec3 operator-(const Vec3 &v) const {
    return Vec3(x - v.x, y - v.y, z - v.z);
  }
  Vec3 operator*(float s) const { return Vec3(x * s, y * s, z * s); }
  Vec3 operator/(float s) const { return Vec3(x / s, y / s, z / s); }

  // Producto Punto
  float dot(const Vec3 &v) const { return x * v.x + y * v.y + z * v.z; }

  // Producto Cruz
  Vec3 cross(const Vec3 &v) const {
    return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
  }

  // Magnitud y Normalizacion
  float length() const { return std::sqrt(x * x + y * y + z * z); }

  void normalize() {
    float l = length();
    if (l > 0) {
      x /= l;
      y /= l;
      z /= l;
    }
  }

  // Rotaciones (Euler)
  Vec3 rotateX(float angle) const {
    return Vec3(x, y * cos(angle) - z * sin(angle),
                y * sin(angle) + z * cos(angle));
  }

  Vec3 rotateY(float angle) const {
    return Vec3(x * cos(angle) - z * sin(angle), y,
                x * sin(angle) + z * cos(angle));
  }

  Vec3 rotateZ(float angle) const {
    return Vec3(x * cos(angle) - y * sin(angle),
                x * sin(angle) + y * cos(angle), z);
  }
};

#endif
