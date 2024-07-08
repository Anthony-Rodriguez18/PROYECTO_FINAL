import pandas as pd
import matplotlib.pyplot as plt

# Estructuras de datos en Python
class Point:
    def __init__(self, osmid, latitude, longitude, street_count, highway, ref):
        self.osmid = osmid
        self.latitude = float(latitude)
        self.longitude = float(longitude)
        self.street_count = int(street_count) if street_count else 0
        self.highway = highway if isinstance(highway, str) else ""
        self.ref = ref if isinstance(ref, str) else ""

class Edge:
    def __init__(self, id, u, v, key, osmid, length, name):
        self.id = int(id)
        self.u = u
        self.v = v
        self.key = int(key)
        self.osmid = list(map(int, osmid)) if isinstance(osmid, list) else []
        self.length = float(length)
        self.name = name if isinstance(name, str) else ""

# Cargar datos de los archivos CSV especificando la codificación
quadtree_df = pd.read_csv(r'C:\Users\RVDO\Documents\EDA\PROYECTO_FINAL\quadtree_edges.csv', encoding='latin1')
nodes_df = pd.read_csv(r'C:\Users\RVDO\Documents\EDA\PROYECTO_FINAL\nodes.csv', encoding='latin1')
edges_df = pd.read_csv(r'C:\Users\RVDO\Documents\EDA\PROYECTO_FINAL\edges.csv', encoding='latin1')

# Mostrar nombres de columnas
print("Columnas en nodes_df:", nodes_df.columns)

# Verificar valores nulos
print("Valores nulos en nodes_df:", nodes_df.isnull().sum())

# Transformar los datos del quadtree a un formato utilizable
quadtree_df['edges'] = quadtree_df['edges'].apply(lambda x: list(map(int, x.split())))

# Crear diccionario de nodos
nodes_dict = {}
for index, row in nodes_df.iterrows():
    try:
        print(f"Procesando fila {index}: {row}")  # Depuración: imprimir la fila actual
        point = Point(row['osmid'], row['y'], row['x'], row['street_cou'], row['highway'], row['ref'])
        nodes_dict[point.osmid] = point
    except Exception as e:
        print(f"Error en la fila {index}: {e}")  # Depuración: mostrar el error específico

# Crear lista de aristas
edges_list = []
for _, row in edges_df.iterrows():
    u = nodes_dict[int(row['u'])]
    v = nodes_dict[int(row['v'])]
    osmid_str = row['osmid'].strip('[]').split(',')
    osmid = [int(x) for x in osmid_str]
    edge = Edge(row['id'], u, v, row['key'], osmid, row['length'], row['name'])
    edges_list.append(edge)

def plot_quadtree(quadtree, nodes, edges):
    fig, ax = plt.subplots()

    # Dibujar los límites de los cuadrantes
    for _, row in quadtree.iterrows():
        xmin, ymin, xmax, ymax = row['xmin'], row['ymin'], row['xmax'], row['ymax']
        width = xmax - xmin if xmax != 2.22507e-308 else 0
        height = ymax - ymin if ymax != 2.22507e-308 else 0
        rect = plt.Rectangle((xmin, ymin), width, height, edgecolor='red', linestyle=':', facecolor='none')
        ax.add_patch(rect)
    
    # Dibujar los edges
    for edge in edges:
        x_values = [edge.u.longitude, edge.v.longitude]
        y_values = [edge.u.latitude, edge.v.latitude]
        ax.plot(x_values, y_values, color='black')
    
    # Dibujar los puntos
    x_coords = [point.longitude for point in nodes.values()]
    y_coords = [point.latitude for point in nodes.values()]
    ax.scatter(x_coords, y_coords, color='green')

    plt.xlabel('Longitude')
    plt.ylabel('Latitude')
    plt.title('QuadTree Visualization')
    plt.grid(True)
    plt.show()

# Llamar a la función de visualización
plot_quadtree(quadtree_df, nodes_dict, edges_list)

def plot_quadtree_to_file(quadtree, nodes, edges, filename='quadtree_visualization.png'):
    fig, ax = plt.subplots()

    # Dibujar los límites de los cuadrantes
    for _, row in quadtree.iterrows():
        xmin, ymin, xmax, ymax = row['xmin'], row['ymin'], row['xmax'], row['ymax']
        width = xmax - xmin if xmax != 2.22507e-308 else 0
        height = ymax - ymin if ymax != 2.22507e-308 else 0
        rect = plt.Rectangle((xmin, ymin), width, height, edgecolor='red', linestyle=':', facecolor='none')
        ax.add_patch(rect)
    
    # Dibujar los edges
    for edge in edges:
        x_values = [edge.u.longitude, edge.v.longitude]
        y_values = [edge.u.latitude, edge.v.latitude]
        ax.plot(x_values, y_values, color='black')
    
    # Dibujar los puntos
    x_coords = [point.longitude for point in nodes.values()]
    y_coords = [point.latitude for point in nodes.values()]
    ax.scatter(x_coords, y_coords, color='green')

    plt.xlabel('Longitude')
    plt.ylabel('Latitude')
    plt.title('QuadTree Visualization')
    plt.grid(True)
    plt.savefig(filename)
    plt.close()

# Llamar a la función para guardar el gráfico
plot_quadtree_to_file(quadtree_df, nodes_dict, edges_list)
