# PROYECTO_FINAL
QuadTree para búsqueda de aristas cercanas en datos geoespaciales
Este programa implementa un QuadTree para buscar aristas geoespaciales cercanas a puntos específicos en un conjunto de datos.

Estructura de archivos
main.cpp: Contiene la implementación principal del programa que lee datos de puntos y aristas desde archivos CSV, construye un QuadTree con las aristas y busca la arista más cercana a puntos de consulta.
nodes.csv: Archivo CSV que contiene información sobre los puntos geoespaciales (nodos).
edges_selected_with_id.csv: Archivo CSV que contiene información detallada sobre las aristas (edges) que conectan los puntos.
Estructuras de datos
Point y Edge
Point: Estructura que representa un punto geoespacial con campos como ID, latitud, longitud, cantidad de calles, tipo de carretera y referencia.

Edge: Estructura que representa una arista entre dos puntos, con campos como ID, puntos de inicio y fin (u y v), clave, identificadores OSM, longitud y nombre.

Funciones principales
readCSV_points: Función para leer puntos desde un archivo CSV y almacenarlos en un vector de Point.
readCSV_edges: Función para leer aristas desde un archivo CSV, utilizando datos de puntos previamente leídos, y almacenarlas en un vector de Edge.
QuadTree: Estructura que implementa un árbol Quad para almacenar y buscar aristas en regiones geoespaciales.
insert: Método para insertar aristas en el QuadTree.
findNearestEdge: Método para encontrar la arista más cercana a un punto de consulta utilizando el método de búsqueda en el árbol Quad.
Uso
Compilar el programa (g++ main.cpp -o quadtree).
Ejecutar el programa (./quadtree).
Introducir latitud y longitud para puntos de consulta.
El programa devolverá la arista más cercana a cada punto de consulta junto con la distancia perpendicular al punto de consulta.
