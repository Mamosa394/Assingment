#include "dijkstra.h"

DijkstraResult runDijkstra(const std::unordered_map<std::string, std::vector<Edge>>& graph, const std::string& source) {
    DijkstraResult result;
    for (const auto& pair : graph) {
        for (const auto& edge : pair.second) {
            if (edge.weight < 0) {
                result.errorMessage = QString("Negative edge detected: %1 -> %2").arg(QString::fromStdString(pair.first)).arg(QString::fromStdString(edge.destination));
                result.valid = false;
                return result;
            }
        }
    }

    std::unordered_map<std::string, int> dist;
    std::unordered_map<std::string, std::string> pred;
    std::unordered_map<std::string, bool> visited;

    for (const auto& pair : graph) {
        dist[pair.first] = INT_MAX;
        visited[pair.first] = false;
        for (const auto& edge : pair.second) {
            dist[edge.destination] = INT_MAX;
            visited[edge.destination] = false;
        }
    }
    dist[source] = 0;

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;
    pq.push(Node(source, 0));

    while (!pq.empty()) {
        Node current = pq.top(); pq.pop();
        std::string u = current.vertex;
        if (visited[u]) continue;
        visited[u] = true;

        auto graphIt = graph.find(u);
        if (graphIt == graph.end()) continue;

        for (const auto& edge : graphIt->second) {
            const std::string& v = edge.destination;
            if (visited[v] || dist[u] == INT_MAX || edge.weight > INT_MAX - dist[u]) continue;
            int newDist = dist[u] + edge.weight;
            if (newDist < dist[v]) {
                dist[v] = newDist;
                pred[v] = u;
                pq.push(Node(v, newDist));
            }
        }
    }
    result.distances = dist;
    result.previous = pred;
    return result;
}

std::list<std::string> reconstructPath(const std::unordered_map<std::string, std::string>& pred, const std::string& source, const std::string& target) {
    std::list<std::string> path;
    std::string current = target;
    size_t steps = 0;
    while (current != source) {
        path.push_front(current);
        auto it = pred.find(current);
        if (it == pred.end()) return std::list<std::string>();
        current = it->second;
        if (++steps > pred.size() + 1) return std::list<std::string>();
    }
    path.push_front(source);
    return path;
}

QString printPath(const std::list<std::string>& path) {
    QString out;
    for (const auto& v : path) {
        if (!out.isEmpty()) out += " -> ";
        out += QString::fromStdString(v);
    }
    return out;
}