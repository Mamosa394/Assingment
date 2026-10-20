#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <unordered_map>
#include <vector>
#include <string>
#include <list>
#include <queue>
#include <climits>
#include <QString>

struct Edge {
    std::string destination;
    int weight;
    Edge(const std::string& dest, int w) : destination(dest), weight(w) {}
};

struct DijkstraResult {
    std::unordered_map<std::string, int> distances;
    std::unordered_map<std::string, std::string> previous;
    bool valid = true;
    QString errorMessage;
};

struct Node {
    std::string vertex;
    int distance;
    Node(const std::string& v, int d) : vertex(v), distance(d) {}
    bool operator>(const Node& other) const { return distance > other.distance; }
};

DijkstraResult runDijkstra(const std::unordered_map<std::string, std::vector<Edge>>& graph, const std::string& source);
std::list<std::string> reconstructPath(const std::unordered_map<std::string, std::string>& pred, const std::string& source, const std::string& target);
QString printPath(const std::list<std::string>& path);

#endif