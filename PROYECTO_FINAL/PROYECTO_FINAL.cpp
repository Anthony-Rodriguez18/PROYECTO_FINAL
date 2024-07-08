#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <tuple>
#include <limits>
#include <regex>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <queue>
#include <memory>  

using namespace std;

struct Point {
    long long id;
    long double latitude;
    long double longitude;
    int street_count;
    string highway;
    string ref;

    Point() : id(0), latitude(0.0L), longitude(0.0L), street_count(0), highway(""), ref("") {}

    Point(long long id, long double lat, long double lon, int count, string hw, string r)
        : id(id), latitude(lat), longitude(lon), street_count(count), highway(hw), ref(r) {}

    friend ostream& operator<<(ostream& os, const Point& p) {
        os << "(" << p.id << ", " << fixed << setprecision(12) << p.latitude << ", " << fixed << setprecision(12) << p.longitude << ", " << p.street_count << ", " << p.highway << ", " << p.ref << ")";
        return os;
    }
};


struct Edge {
    int id;
    Point u;
    Point v;
    long long key;
    vector<long long> osmid;
    long double length;
    string name;

    Edge() : id(0), key(0), length(0.0L), name("") {}

    Edge(int iden, Point ini, Point fin, long long id, vector<long long> osmids, long double ltght, string lgth)
        : id(iden), u(ini), v(fin), key(id), osmid(osmids), length(ltght), name(lgth) {}

    friend ostream& operator<<(ostream& os, const Edge& e) {
        os << "(" << e.id << ", (" << e.u << "), (" << e.v << "), " << e.key << ", " << fixed << setprecision(12) << e.length << ", " << e.name << ")";
        return os;
    }
};


long double disteucl(const Point& a, const Point& b) {
    return sqrtl(powl(a.latitude - b.latitude, 2) + powl(a.longitude - b.longitude, 2));
}


long double distPerpendicular(const Point& point, const Edge& edge) {
    Point a = edge.u;
    Point b = edge.v;

    long double px = point.latitude;
    long double py = point.longitude;
    long double ax = a.latitude;
    long double ay = a.longitude;
    long double bx = b.latitude;
    long double by = b.longitude;

    long double abx = bx - ax;
    long double aby = by - ay;
    long double apx = px - ax;
    long double apy = py - ay;

    long double ab2 = abx * abx + aby * aby;
    long double ap_ab = apx * abx + apy * aby;
    long double t = ap_ab / ab2;

    if (t < 0.0L) {
        t = 0.0L;
    }
    else if (t > 1.0L) {
        t = 1.0L;
    }

    long double closestX = ax + t * abx;
    long double closestY = ay + t * aby;

    return sqrtl(powl(px - closestX, 2) + powl(py - closestY, 2));
}


string remove_quotes_backslash(const string& str) {
    string result = str;
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
        latitude = stold(field);
        getline(s, field, ',');
        longitude = stold(field);
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
    cerr << "No se encontró un punto con el ID " << id << endl;
    return Point(-1, 0.0, 0.0, 0, "", "");
}

vector<Edge> readCSV_edges(const string& filename, const vector<Point>& points) {
    ifstream file(filename);
    vector<Edge> edges;
    string line;

    regex num_regex(R"(\d+)");

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
        Point point_u = findPointById(points, u);
        getline(ss, token, ',');
        token = remove_quotes_backslash(token);
        v = stoll(token);
        Point point_v = findPointById(points, v);
        getline(ss, token, ',');
        token = remove_quotes_backslash(token);
        key = stoll(token);

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
        length = stold(token);
        getline(ss, token, ',');
        token = remove_quotes_backslash(token);
        name = token;

        edges.emplace_back(id, point_u, point_v, key, osmid, length, name);
    }

    return edges;
}

struct QuadTreeNode {
    long double x_min, x_max, y_min, y_max;  
    vector<Edge> edges; 
    unique_ptr<QuadTreeNode> nw, ne, sw, se; 

    QuadTreeNode(long double xmin, long double xmax, long double ymin, long double ymax)
        : x_min(xmin), x_max(xmax), y_min(ymin), y_max(ymax), nw(nullptr), ne(nullptr), sw(nullptr), se(nullptr) {}

    bool isLeaf() const {
        return !nw && !ne && !sw && !se;
    }

    bool contains(const Point& point) const {
        return (point.latitude >= y_min && point.latitude <= y_max &&
            point.longitude >= x_min && point.longitude <= x_max);
    }
};

struct QuadTree {
    unique_ptr<QuadTreeNode> root;
    int maxEdgesPerNode;

    QuadTree(long double xmin, long double xmax, long double ymin, long double ymax, int maxEdges = 4)
        : root(make_unique<QuadTreeNode>(xmin, xmax, ymin, ymax)), maxEdgesPerNode(maxEdges) {}

    void insert(const Edge& edge) {
        insert(root, edge);
    }

    void print() const {
        print(root, 0);
    }

    Edge findNearestEdge(const Point& point) const {
        return findNearestEdge(root, point);
    }

private:
    void insert(unique_ptr<QuadTreeNode>& node, const Edge& edge) {
       
        long double margin = 0.0001;
        if (!(
            edge.u.latitude >= node->y_min - margin && edge.u.latitude <= node->y_max + margin &&
            edge.u.longitude >= node->x_min - margin && edge.u.longitude <= node->x_max + margin &&
            edge.v.latitude >= node->y_min - margin && edge.v.latitude <= node->y_max + margin &&
            edge.v.longitude >= node->x_min - margin && edge.v.longitude <= node->x_max + margin
            )) {
            return;
        }

        // Si el nodo es una hoja y tiene espacio para más aristas, agregar la arista al nodo
        if (node->isLeaf() && node->edges.size() < maxEdgesPerNode) {
            node->edges.push_back(edge);
            return;
        }

        // Si el nodo es una hoja y está lleno, subdividir el nodo
        if (node->isLeaf()) {
            subdivide(node);
        }

        // Insertar la arista en los nodos hijos apropiados
        insert(node->nw, edge);
        insert(node->ne, edge);
        insert(node->sw, edge);
        insert(node->se, edge);
    }


    Edge findNearestEdge(const unique_ptr<QuadTreeNode>& node, const Point& point) const {
        if (!node) {
            return Edge();
        }

        if (!node->contains(point)) {
            return Edge();
        }

        Edge nearestEdge;
        long double minDistance = numeric_limits<long double>::max();

        for (const auto& edge : node->edges) {
            long double distance = distPerpendicular(point, edge);
            if (distance < minDistance) {
                minDistance = distance;
                nearestEdge = edge;
            }
        }

        
        if (node->isLeaf()) {
            return nearestEdge;
        }

   
        if (node->nw) {
            Edge nearestInSubtree = findNearestEdge(node->nw, point);
            long double distanceInSubtree = distPerpendicular(point, nearestInSubtree);
            if (distanceInSubtree < minDistance) {
                minDistance = distanceInSubtree;
                nearestEdge = nearestInSubtree;
            }
        }

        if (node->ne) {
            Edge nearestInSubtree = findNearestEdge(node->ne, point);
            long double distanceInSubtree = distPerpendicular(point, nearestInSubtree);
            if (distanceInSubtree < minDistance) {
                minDistance = distanceInSubtree;
                nearestEdge = nearestInSubtree;
            }
        }

        if (node->sw) {
            Edge nearestInSubtree = findNearestEdge(node->sw, point);
            long double distanceInSubtree = distPerpendicular(point, nearestInSubtree);
            if (distanceInSubtree < minDistance) {
                minDistance = distanceInSubtree;
                nearestEdge = nearestInSubtree;
            }
        }

        if (node->se) {
            Edge nearestInSubtree = findNearestEdge(node->se, point);
            long double distanceInSubtree = distPerpendicular(point, nearestInSubtree);
            if (distanceInSubtree < minDistance) {
                minDistance = distanceInSubtree;
                nearestEdge = nearestInSubtree;
            }
        }

        return nearestEdge;
    }

    void subdivide(unique_ptr<QuadTreeNode>& node) {
        long double x_mid = node->x_min + (node->x_max - node->x_min) / 2;
        long double y_mid = node->y_min + (node->y_max - node->y_min) / 2;

        node->nw = make_unique<QuadTreeNode>(node->x_min, x_mid, y_mid, node->y_max);
        node->ne = make_unique<QuadTreeNode>(x_mid, node->x_max, y_mid, node->y_max);
        node->sw = make_unique<QuadTreeNode>(node->x_min, x_mid, node->y_min, y_mid);
        node->se = make_unique<QuadTreeNode>(x_mid, node->x_max, node->y_min, y_mid);
    }

    void print(const unique_ptr<QuadTreeNode>& node, int level) const {
        if (!node) {
            return;
        }

        cout << setw(level * 4) << "";
        cout << "Level " << level << ": ";
        for (const auto& edge : node->edges) {
            cout << edge << " ";
        }
        cout << endl;

        print(node->nw, level + 1);
        print(node->ne, level + 1);
        print(node->sw, level + 1);
        print(node->se, level + 1);
    }
};

void exportQuadTree(const QuadTree& qt, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file" << endl;
        return;
    }

    file << "x_min,x_max,y_min,y_max,edges\n";
    queue<const QuadTreeNode*> q;
    q.push(qt.root.get());

    while (!q.empty()) {
        const QuadTreeNode* node = q.front();
        q.pop();

        file << fixed << setprecision(12) << node->x_min << "," << node->x_max << "," << node->y_min << "," << node->y_max << ",";
        for (const auto& edge : node->edges) {
            file << edge.id << " ";
        }
        file << "\n";

        if (node->nw) q.push(node->nw.get());
        if (node->ne) q.push(node->ne.get());
        if (node->sw) q.push(node->sw.get());
        if (node->se) q.push(node->se.get());
    }

    file.close();
}


void print_data_edge(const Edge& e) {
    cout << "ID: " << e.id << endl;
    cout << "From Point U (ID, Lat, Lon): " << e.u.id << ", " << setprecision(12) << e.u.latitude << ", " << setprecision(12) << e.u.longitude << endl;
    cout << "To Point V (ID, Lat, Lon): " << e.v.id << ", " << setprecision(12) << e.v.latitude << ", " << setprecision(12) << e.v.longitude << endl;
    cout << "Key: " << e.key << endl;
    cout << "OSM IDs: [";
    for (const auto& osmid : e.osmid) {
        cout << osmid << " ";
    }
    cout << "]" << endl;
    cout << "Length: " << setprecision(numeric_limits<long double>::digits10 + 1) << e.length << endl;
    cout << "Name: " << e.name << endl;
    cout << endl;
}

void printNearestEdge(const Point& queryPoint, const Edge& nearestEdge) {
    cout << "Punto de busqueda: " << queryPoint << endl;
    cout << "Arista mas cercana: " << endl;  print_data_edge(nearestEdge); cout << endl;
    cout << "Distancia: " << fixed << setprecision(12) << distPerpendicular(queryPoint, nearestEdge) << endl;
}


int main() {
    vector<Point> points = readCSV_points("nodes.csv");
    vector<Edge> edges = readCSV_edges("edges_selected_with_id.csv", points);

    // Construir el QuadTree
    long double x_min = numeric_limits<long double>::max();
    long double x_max = numeric_limits<long double>::min();
    long double y_min = numeric_limits<long double>::max();
    long double y_max = numeric_limits<long double>::min();

    for (const auto& point : points) {
        if (point.longitude < x_min) x_min = point.longitude;
        if (point.longitude > x_max) x_max = point.longitude;
        if (point.latitude < y_min) y_min = point.latitude;
        if (point.latitude > y_max) y_max = point.latitude;
    }

    QuadTree quadtree(x_min, x_max, y_min, y_max);

    for (const auto& edge : edges) {
        quadtree.insert(edge);
    }

    exportQuadTree(quadtree, "quadtree_data.csv");


    int cause = 1;
    while (cause < 11) {
        cout << "--------------------" << cause << "--------------------" << endl;
        Point queryPoint;
        cout << "Ingrese la latitud y longitud del punto de consulta:" << endl;
        cin >> queryPoint.latitude >> queryPoint.longitude;


        Edge nearestEdge = quadtree.findNearestEdge(queryPoint);
        printNearestEdge(queryPoint, nearestEdge);
        cause++;
    }

    return 0;
}
