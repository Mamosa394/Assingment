#include "dijkstra.h"

DijkstraResult runDijkstra(const std::unordered_map<std::string, std::vector<Edge>>& graph,
                           const std::string& source) {
    DijkstraResult result;

    // ---- 1. Reject negative edges up-front (Dijkstra cannot handle them) ----
    for (const auto& pair : graph) {
        for (const auto& edge : pair.second) {
            if (edge.weight < 0) {
                result.valid = false;
                result.errorMessage = QString("Negative edge detected: %1 -> %2 (weight %3). "
                                              "Dijkstra requires non-negative weights.")
                                          .arg(QString::fromStdString(pair.first))
                                          .arg(QString::fromStdString(edge.destination))
                                          .arg(edge.weight);
                return result;   // empty map as required
            }
        }
    }

    // ---- 2. Source must exist in the graph ----
    if (graph.find(source) == graph.end()) {
        result.valid = false;
        result.sourceExists = false;
        result.errorMessage = QString("Source vertex '%1' does not exist in the graph.")
                                  .arg(QString::fromStdString(source));
        return result;
    }

    // ---- 3. Initialise every vertex in the graph (not just reachable ones) ----
    std::unordered_map<std::string, int>  dist;
    std::unordered_map<std::string, std::string> pred;
    std::unordered_map<std::string, bool> visited;

    for (const auto& pair : graph) {
        dist[pair.first] = INT_MAX;
        visited[pair.first] = false;
        for (const auto& edge : pair.second) {
            if (dist.find(edge.destination) == dist.end()) {
                dist[edge.destination] = INT_MAX;
                visited[edge.destination] = false;
            }
        }
    }
    dist[source] = 0;

    // ---- 4. Min-heap Dijkstra ----
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;
    pq.push(Node(source, 0));

    while (!pq.empty()) {
        Node current = pq.top(); pq.pop();
        const std::string& u = current.vertex;

        if (visited[u]) continue;
        if (current.distance != dist[u]) continue;   // stale entry guard
        visited[u] = true;

        auto graphIt = graph.find(u);
        if (graphIt == graph.end()) continue;

        for (const auto& edge : graphIt->second) {
            const std::string& v = edge.destination;
            if (visited[v]) continue;
            if (dist[u] == INT_MAX) continue;
            if (edge.weight > INT_MAX - dist[u]) continue;   // overflow guard

            int newDist = dist[u] + edge.weight;
            if (newDist < dist[v]) {
                dist[v]    = newDist;
                pred[v]    = u;
                pq.push(Node(v, newDist));
            }
        }
    }

    result.distances = std::move(dist);
    result.previous  = std::move(pred);
    return result;
}

std::list<std::string> reconstructPath(const std::unordered_map<std::string, std::string>& pred,
                                       const std::string& source,
                                       const std::string& target) {
    std::list<std::string> path;
    std::string current = target;
    size_t steps = 0;
    const size_t limit = pred.size() + 2;    // safety against cycles

    while (current != source) {
        path.push_front(current);
        auto it = pred.find(current);
        if (it == pred.end()) return std::list<std::string>();   // unreachable
        current = it->second;
        if (++steps > limit) return std::list<std::string>();
    }
    path.push_front(source);
    return path;
}

QString printPath(const std::list<std::string>& path) {
    QString out;
    for (const auto& v : path) {
        if (!out.isEmpty()) out += " → ";
        out += QString::fromStdString(v);
    }
    return out;
}