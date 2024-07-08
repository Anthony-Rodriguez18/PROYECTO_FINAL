#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <tuple>
#include <limits>
#include <regex>
#include <iomanip> // Para std::setprecision
#include <algorithm>

using namespace std;

struct Point {
    long long id;
    long double latitude;
    long double longitude;
    int street_count;
    string highway;
    string ref;

    Point(long long id, long double lat, long double lon, int count, string hw, string r)
        : id(id), latitude(lat), longitude(lon), street_count(count), highway(hw), ref(r) {}
};

struct Edge {
    int id;
    Point u;
    Point v;
    long long key;
    vector<long long> osmid;
    long double length;
    string name;

    Edge(int iden, Point ini, Point fin, long long id, vector<long long> osmids, long double ltght, string lgth)
        : id(iden), u(ini), v(fin), key(id), osmid(osmids), length(ltght), name(lgth) {}
};

string remove_quotes_backslash(const string& str) {
    string result = str;
    // Eliminar comillas dobles y barra invertida
    result.erase(remove(result.begin(), result.end(), '\"'), result.end());
    result.erase(remove(result.begin(), result.end(), '\\'), result.end());
    return result;
}

vector<Point> readCSV_points(const string& filename) {
    vector<Point> points;
    ifstream file(filename);
    string line;

    if (!file.is_open()) {
        cerr << "Error opening file" << endl;
        return points;
    }

    // Ignorar la primera línea (cabeceras)
    getline(file, line);

    while (getline(file, line)) {
        istringstream s(line);
        string field;

        long long id;
        long double latitude, longitude;
        int street_count;
        string highway, ref;

        getline(s, field, ',');
        id = stoll(field);
        getline(s, field, ',');
        latitude = stold(field); // Utiliza stold para long double
        getline(s, field, ',');
        longitude = stold(field); // Utiliza stold para long double
        getline(s, field, ',');
        street_count = stoi(field);
        getline(s, field, ',');
        highway = field;
        getline(s, field, ',');
        ref = field;

        points.emplace_back(id, latitude, longitude, street_count, highway, ref);
    }

    return points;
}

Point findPointById(const vector<Point>& points, long long id) {
    for (const auto& p : points) {
        if (p.id == id) {
            return p;
        }
    }
    // Si no se encuentra el punto, se imprime un mensaje de error y se devuelve un punto vacío
    cerr << "No se encontró un punto con el ID " << id << endl;
    return Point(-1, 0.0, 0.0, 0, "", "");
}

vector<Edge> readCSV_edges(const string& filename, const vector<Point>& points) {
    ifstream file(filename);
    vector<Edge> edges;
    string line;

    // Expresión regular para extraer números de una lista
    regex num_regex(R"(\d+)");

    // Leer la primera línea (encabezados) y descartarla
    getline(file, line);

    int id = 0;
    while (getline(file, line)) {
        stringstream ss(line);
        string token;

        long long u = 0, v = 0, key = 0;
        vector<long long> osmid;
        long double length;
        string name;

        getline(ss, token, ',');
        id++;
        getline(ss, token, ',');
        token = remove_quotes_backslash(token);
        u = stoll(token);
        Point point_u = findPointById(points, u); // Buscar el punto u por su ID
        getline(ss, token, ',');
        token = remove_quotes_backslash(token);
        v = stoll(token);
        Point point_v = findPointById(points, v); // Buscar el punto v por su ID
        getline(ss, token, ',');
        token = remove_quotes_backslash(token);
        key = stoll(token);

        // Leer osmid
        getline(ss, token, ',');
        token = remove_quotes_backslash(token);
        if (token.front() == '[') {
            token = token.substr(1);
            osmid.push_back(stoll(token));
            getline(ss, token, ']');
            stringstream num_stream(token);
            string num;
            while (getline(num_stream, num, ',')) {
                osmid.push_back(stoll(num));
            }
            getline(ss, token, ',');
        }
        else {
            osmid.push_back(stoll(token));
        }

        getline(ss, token, ',');
        token = remove_quotes_backslash(token);
        length = stold(token); // Utiliza stold para long double
        getline(ss, token, ',');
        token = remove_quotes_backslash(token);
        name = token;

        edges.emplace_back(id, point_u, point_v, key, osmid, length, name);
    }

    return edges;
}

Edge findEdgeById(const vector<Edge>& edges, int id) {
    for (const auto& edge : edges) {
        if (edge.id == id) {
            return edge;
        }
    }
    // Si no se encuentra el borde, se imprime un mensaje de error y se devuelve un borde vacío
    cerr << "No se encontró un borde con el ID " << id << endl;
    return Edge(-1, Point(-1, 0.0, 0.0, 0, "", ""), Point(-1, 0.0, 0.0, 0, "", ""), 0, {}, 0.0, "");
}

void print_data_point(const Point& p) {
    cout << "ID: " << p.id << endl;
    cout << "Latitude: " << fixed << setprecision(10) << p.latitude << endl;
    cout << "Longitude: " << fixed << setprecision(10) << p.longitude << endl;
    cout << "Street Count: " << p.street_count << endl;
    cout << "Highway: " << p.highway << endl;
    cout << "Reference: " << p.ref << endl;
    cout << endl;
}


void print_data_edge(const Edge& e) {
    cout << "ID: " << e.id << endl;
    cout << "From Point U (ID, Lat, Lon): " << e.u.id << ", " << fixed << setprecision(10) << e.u.latitude << ", " << fixed << setprecision(10) << e.u.longitude << endl;
    cout << "To Point V (ID, Lat, Lon): " << e.v.id << ", " << fixed << setprecision(10) << e.v.latitude << ", " << fixed << setprecision(10) << e.v.longitude << endl;
    cout << "Key: " << e.key << endl;
    cout << "OSM IDs: [";
    for (const auto& osmid : e.osmid) {
        cout << osmid << " ";
    }
    cout << "]" << endl;
    cout << "Length: " << fixed << setprecision(10) << e.length << endl;
    cout << "Name: " << e.name << endl;
    cout << endl;
}


int main() {
    string filename_points = "nodes.csv";
    vector<Point> puntos = readCSV_points(filename_points);

    string filename_edges = "edges_selected_with_id.csv";
    vector<Edge> edges = readCSV_edges(filename_edges, puntos);

    int searchEdgeId;
    cout << "INGRESE ID: ";
    cin >> searchEdgeId;
    Edge foundEdge = findEdgeById(edges, searchEdgeId);

    if (foundEdge.id != -1) {
        print_data_edge(foundEdge);
    }
    else {
        cout << "Edge con ID " << searchEdgeId << " no encontrado." << endl;
    }
    
    long long searchId;
    cout << "INGRESE ID: ";
    cin >> searchId;
    Point foundPoint = findPointById(puntos, searchId);
    if (foundPoint.id != -1) {
        print_data_point(foundPoint);
    }
    else {
        cout << "Punto con ID " << searchId << " no encontrado." << endl;
    }

    

    return 0;
}
