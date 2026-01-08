# CubeSDL ++ (Refactored)

Esta es una versión modernizada y orientada a objetos del motor de renderizado 3D por software original.

## 🌟 Mejoras Implementadas

1.  **Migración a C++ Moderno**:
    *   Uso de `classes` y `structs` con encapsulamiento.
    *   Implementación de `std::vector` para manejo dinámico de geometría.
    
2.  **Librería Matemática Propia**:
    *   `Vec3.h`: Clase vector con sobrecarga de operadores (`+`, `-`, `*`, `dot`, `cross`).
    *   Simplifica enormemente las fórmulas matemáticas de rotación y proyección.

3.  **Cargador de Modelos OBJ**:
    *   ¡Ya no estás limitado a un cubo!
    *   Soporta la carga de archivos `.obj` estándar (Wavefront OBJ).
    *   Si no encuentra un archivo `model.obj`, carga el cubo por defecto.

4.  **Pipeline de Renderizado**:
    *   Clase `Renderer` que abstrae SDL2.
    *   Implementación limpia de **Vertex Transformation** -> **Backface Culling** -> **Projection** -> **Rasterization**.

## 🛠️ Cómo Usar

1.  Asegúrate de tener las librerías de desarrollo de SDL2 configuradas en tu compilador.
2.  Compila con:
    ```bash
    make
    ```
3.  Coloca un archivo `model.obj` en la carpeta raíz si quieres ver un modelo personalizado.
4.  Ejecuta:
    ```bash
    make run
    ```

## 🎮 Controles

*   **ESC**: Salir de la aplicación.
*   (La rotación es automática para demostración).

---
*Hecho para demostrar la evolución de C legacy a Arquitectura de Motores Gráficos moderna.*
