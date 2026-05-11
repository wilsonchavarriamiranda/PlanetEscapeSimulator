# Integrantes
- Wilson Chavarría Miranda - C22114
- Raúl Gadea Alfaro - C12989
- Fabricio Padilla Madrigal - C35837

# Proyecto 2 Simulador de Rutas y Construcción de Tuberías en un Mundo Dirigido y Ponderado

Este proyecto implementa un simulador interactivo basado en teoría de grafos, donde un jugador encarna a un astronauta varado en un planeta desconocido. El planeta está representado como un grafo dirigido y ponderado, cargado desde un archivo externo. Cada nodo corresponde a un sector del mapa (Base, Terreno o Recurso) y cada arista representa un camino transitable que consume energía.

El objetivo final del juego es acumular suficientes recursos para construir el motor de plasma de Franklin Chang y escapar del planeta. Para lograrlo, el jugador deberá explorar, gestionar su batería, construir máquinas extractoras y comparar distintos algoritmos clásicos de búsqueda de caminos.

## Mecánica General del Simulador

El jugador inicia en la Base, un nodo especial que funciona como punto seguro y como origen principal de las rutas generadas. La batería del traje disminuye según el peso de las aristas recorridas, por lo que planificar rutas es esencial. Si la batería llega a cero lejos de la Base, la partida termina.

Al descubrir un nodo de recurso, el jugador puede invertir sus fondos para instalar una máquina extractora. Cada máquina crea automáticamente una tubería hacia la Base utilizando un algoritmo distinto, y la ganancia final depende del costo de mantenimiento del camino elegido.

## Algoritmos de Construcción de Tuberías

El proyecto integra tres tecnologías de máquinas, cada una basada en un algoritmo distinto:

### Nivel 1 — Algoritmos ciegos (BFS / DFS)

- Ignoran los pesos del grafo.
- Construyen rutas potencialmente largas y costosas.
- Son la opción más barata pero generan las peores ganancias.

### Nivel 2 — Algoritmo Greedy

- Siempre elige el siguiente vecino de menor peso.
- Más eficiente que BFS/DFS, pero no garantiza optimalidad global.
- Puede caer en mínimos locales.

### Nivel 3 — Dijkstra

- Calcula el camino de costo mínimo absoluto.
- Maximiza la rentabilidad y minimiza el costo de mantenimiento.
- Utiliza una cola de prioridad para optimizar tiempo de ejecución.

### Nivel 4 — A*

- Variante informada que utiliza heurísticas.
- Permite comparar rendimiento con Dijkstra.


## Compilación y ejecución

Para compilar el proyecto es necesario tener instalado SFML, y desde la carpeta juegoProyecto ejecutar el comando make run en la terminal.