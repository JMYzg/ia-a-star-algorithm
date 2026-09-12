# Simulador de A*

Aplicación de escritorio en C++ con Qt 6 que simula de forma interactiva el algoritmo A*
(A-star) sobre un grafo construido por el usuario: nodos, aristas y pesos editables, con
resolución paso a paso.

## Características

- Editor de grafos: crear, mover, renombrar, colorear y eliminar nodos y aristas
- Pesos calculados automáticamente por distancia entre nodos (1 decimal)
- Selección de nodo inicio y destino (exclusivos)
- Botón ordenar: reacomoda los nodos sin solapamiento (relajación Fruchterman-Reingold)
- Ejecución paso a paso del algoritmo A* (h = distancia euclídea al destino) con:
  - Resaltado de frontera (abierta), visitados (cerrada) y nodo actual
  - Tablas de lista abierta (N, Father, f()) y cerrada (N, Father)
  - Barra de progreso animada (ease) y modo auto con avance continuo
  - Resaltado del camino final encontrado
- Zoom con rueda y pan arrastrando el board (o botón medio)

## Atajos de teclado

| Tecla | Acción |
|---|---|
| Espacio / → | Siguiente paso (en modo Resolver) |
| Retroceso / ← | Paso anterior |
| Supr | Eliminar selección |
| Esc | Cancelar modo / detener resolución |

## Requisitos

- Compilador con soporte C++20 (GCC/Clang/MSVC)
- CMake 3.21 o superior
- Ninja (recomendado) o Make
- Qt 6.5 o superior (módulos Core, Gui, Widgets)

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

## Tests

Suite de tests del motor A* (rutas, empates, relajación, validaciones):

```bash
./build/a-star-tests
```
o

```bash
ctest --test-dir build
```
