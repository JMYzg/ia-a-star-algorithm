# Simulador de A*

Aplicación de escritorio en C++ con Qt 6 que simula de forma interactiva el algoritmo A*
(A-star) sobre un grafo construido por el usuario: nodos, aristas y pesos editables, con
resolución paso a paso.

## Características (planeadas)

- Editor de grafos: crear, mover y eliminar nodos y aristas, asignar pesos
- Selección de nodo origen y destino
- Ejecución paso a paso del algoritmo A* mostrando la frontera (open set) y los visitados (closed set)
- Visualización de costes por nodo (g, h, f)

## Requisitos

- Compilador con soporte C++20 (GCC/Clang/MSVC)
- CMake 3.21 o superior
- Ninja (recomendado) o Make
- Qt 6.5 o superior (módulo Widgets)

En Arch Linux:

```bash
sudo pacman -S --needed cmake ninja qt6-base
```

## Compilar

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## Ejecutar

```bash
./build/a-star-simulator
```
