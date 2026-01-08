# CubeSDL ++ (Motor 3D Refactorizado)

Este proyecto es un motor de renderizado 3D por software escrito en C++ moderno desde cero. Es una evolución masiva del código original en C, implementando un pipeline de gráficos real (similar a OpenGL/Vulkan pero ejecutado 100% en la CPU).

## 🚀 Características Técnicas

*   **Pipeline de Software**: Transformación de vértices, descarte de caras (Culling 3D), proyección y rasterización manual de píxeles.
*   **Z-Buffer (Depth Buffer)**: Gestión de profundidad real de alta precisión para evitar transparencias y errores de solapamiento.
*   **Sombreado Gouraud Progresivo**: Iluminación puntual dinámica con degradados suaves y dramáticos.
*   **Doble Modo de Proyección**: Soporte para Perspectiva real y Proyección Ortogonal.
*   **Cargador OBJ**: Capacidad para cargar modelos 3D externos en formato `.obj`.

---

## 🛠️ Instalación y Compilación (Windows)

Este proyecto utiliza **MinGW-w64** y **SDL2**. Sigue estos pasos para configurarlo:

### 1. Requisitos Previos
*   **MinGW-w64**: Asegúrate de tener `g++` y `mingw32-make` instalados en tu PATH.
*   **SDL2**: El repositorio ya espera las librerías en la carpeta `SDL2-2.30.0` (o puedes ajustar el `Makefile` si las tienes en otra ruta).

### 2. Estructura del Repositorio
Para que la compilación funcione, asegúrate de tener la carpeta `include/SDL2` y las librerías `.a` vinculadas correctamente en el `Makefile`.

### 3. Compilación
Abre una terminal en la raíz del proyecto y ejecuta:
```powershell
mingw32-make
```

### 4. Ejecución
Para iniciar el motor:
```powershell
mingw32-make run
```

---

## 🎮 Controles de Teclado

El motor permite alternar todos los modos en tiempo real:

| Tecla | Acción |
| :--- | :--- |
| **`1`** o **`V`** | Alternar visualización de **Vértices** (Puntos) |
| **`2`** o **`L`** | Alternar visualización de **Wireframe** (Líneas) |
| **`3`** o **`F`** | Alternar visualización de **Sólido** (Caras/Triángulos) |
| **`4`** | Alternar **Sombreado** (Gouraud Shading) |
| **`5`** o **`C`** | Alternar **Backface Culling** (Ocultar caras traseras) |
| **`6`** o **`P`** | Cambiar entre **Perspectiva** y **Ortogonal** |
| **`ESC`** | Cierra la aplicación |

---

## 📂 Uso de Modelos Personalizados
Por defecto, el motor genera un cubo. Sin embargo, puedes cargar cualquier modelo:
1.  Busca un archivo `.obj` (Low poly recomendado).
2.  Renómbralo a `model.obj`.
3.  Colócalo en la raíz del proyecto junto al `main.cpp`.
4.  Reinicia la aplicación.

---

## 🔥 Créditos
Desarrollado para demostrar la arquitectura de motores gráficos moderna y la optimización de cálculos vectoriales en C++. 
🧊✨ **¡Disfruta del renderizado!**
