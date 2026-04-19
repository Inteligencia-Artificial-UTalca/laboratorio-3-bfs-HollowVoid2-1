#include "Search.h"
#include <queue>
#include <chrono>
#include <unordered_map>
#include <iostream>
#include <limits>
#include <deque>
#include <cmath>
#include <vector>

namespace std {
    // Necesario para usar std::pair<int,int> como clave en unordered_map
    template<>
    struct hash<std::pair<int,int>> {
        std::size_t operator()(const std::pair<int,int>& p) const noexcept {
            hash<int> hasher;
            return hasher(p.first) ^ (hasher(p.second) << 1);
        }
    };
}

// ========================
// Estructuras para Greedy
// ========================
struct Node {
    std::pair<int,int> pos;
    float priority;
};

struct CompareNodes {
    bool operator()(const Node& n1, const Node& n2) const {
        return n1.priority > n2.priority; // menor prioridad = mayor importancia
    }
};

// ========================
// Estructuras para A*
// ========================
struct AStarNode {
    std::pair<int,int> pos;
    float g;   // costo acumulado desde el inicio
    float f;   // f = g + h
};

struct CompareAStarNode {
    bool operator()(const AStarNode& a, const AStarNode& b) const {
        if (a.f != b.f) return a.f > b.f;   // menor f tiene prioridad
        return a.g < b.g;                   // desempate opcional
    }
};

float Search::Heuristic(std::pair<int,int> start, std::pair<int,int> goal) {
    // Distancia Manhattan
    return std::abs(start.first - goal.first) + std::abs(start.second - goal.second);
}

std::vector<std::pair<int,int>> Search::reconstruct(
    const std::unordered_map<std::pair<int,int>, std::pair<int,int>> &pathCache,
    const std::pair<int,int> &start) {

    std::deque<std::pair<int,int>> nodes;
    auto node = start;

    while (true) {
        nodes.push_front(node);

        auto it = pathCache.find(node);
        if (it == pathCache.end())
            break;

        node = it->second;
    }

    return std::vector<std::pair<int,int>>(nodes.begin(), nodes.end());
}

std::vector<std::pair<int,int>> Search::BFS(
    const Map& map, std::pair<int,int> start, std::pair<int,int> goal) {

    std::cout << "===========================\nRunning BFS...\n";
    auto startTime = std::chrono::high_resolution_clock::now();

    std::pair<int,int> dirs[]{{-1,0},{0,1},{1,0},{0,-1}};

    std::vector<std::vector<bool>> visited(
        map.getHeight(), std::vector<bool>(map.getWidth(), false));

    std::queue<std::pair<int,int>> OPEN;
    std::unordered_map<std::pair<int,int>, std::pair<int,int>> pathCache;

    OPEN.push(start);
    visited[start.first][start.second] = true;

    while (!OPEN.empty()) {
        auto pos = OPEN.front();
        OPEN.pop();

        if (pos == goal) {
            auto endTime = std::chrono::high_resolution_clock::now();
            std::cout << "FOUND in " << (endTime - startTime).count() / 1000000.0 << "ms\n";
            return reconstruct(pathCache, pos);
        }

        for (auto dir : dirs) {
            std::pair<int,int> next = {pos.first + dir.first, pos.second + dir.second};

            if (next.first < 0 || next.first >= map.getHeight() ||
                next.second < 0 || next.second >= map.getWidth())
                continue;

            if (map._map[next.first][next.second] == 1 || visited[next.first][next.second])
                continue;

            visited[next.first][next.second] = true;
            OPEN.push(next);
            pathCache[next] = pos;
        }
    }

    std::cout << "NOT FOUND!!!!\n";
    return {};
}

std::vector<std::pair<int,int>> Search::Greedy(
    const Map& map, std::pair<int,int> start, std::pair<int,int> goal) {

    std::cout << "===========================\nRunning Greedy Best-First Search...\n";
    auto startTime = std::chrono::high_resolution_clock::now();

    std::pair<int,int> dirs[]{{-1,0},{0,1},{1,0},{0,-1}};

    std::vector<std::vector<bool>> visited(
        map.getHeight(), std::vector<bool>(map.getWidth(), false));

    std::priority_queue<Node, std::vector<Node>, CompareNodes> OPEN;
    std::unordered_map<std::pair<int,int>, std::pair<int,int>> pathCache;

    OPEN.push({start, Heuristic(start, goal)});
    visited[start.first][start.second] = true;

    while (!OPEN.empty()) {
        auto current = OPEN.top();
        OPEN.pop();

        std::pair<int,int> pos = current.pos;

        if (pos == goal) {
            auto endTime = std::chrono::high_resolution_clock::now();
            std::cout << "FOUND in " << (endTime - startTime).count() / 1000000.0 << "ms\n";
            return reconstruct(pathCache, pos);
        }

        for (auto dir : dirs) {
            std::pair<int,int> next = {pos.first + dir.first, pos.second + dir.second};

            if (next.first < 0 || next.first >= map.getHeight() ||
                next.second < 0 || next.second >= map.getWidth())
                continue;

            if (map._map[next.first][next.second] == 1 || visited[next.first][next.second])
                continue;

            visited[next.first][next.second] = true;
            pathCache[next] = pos;
            OPEN.push({next, Heuristic(next, goal)});
        }
    }

    std::cout << "NOT FOUND!!!!\n";
    return {};
}

std::vector<std::pair<int,int>> Search::AStar(
    const Map& map, std::pair<int,int> start, std::pair<int,int> goal) {

    std::cout << "===========================\nRunning A*...\n";
    auto startTime = std::chrono::high_resolution_clock::now();

    std::pair<int,int> dirs[]{{-1,0},{0,1},{1,0},{0,-1}};

    const float INF = std::numeric_limits<float>::infinity();

    std::vector<std::vector<float>> gScore(
        map.getHeight(), std::vector<float>(map.getWidth(), INF));

    std::vector<std::vector<bool>> closed(
        map.getHeight(), std::vector<bool>(map.getWidth(), false));

    std::priority_queue<AStarNode, std::vector<AStarNode>, CompareAStarNode> OPEN;
    std::unordered_map<std::pair<int,int>, std::pair<int,int>> pathCache;

    gScore[start.first][start.second] = 0.0f;
    OPEN.push({start, 0.0f, Heuristic(start, goal)});

    while (!OPEN.empty()) {
        AStarNode current = OPEN.top();
        OPEN.pop();

        auto pos = current.pos;

        if (closed[pos.first][pos.second])
            continue;

        closed[pos.first][pos.second] = true;

        if (pos == goal) {
            auto endTime = std::chrono::high_resolution_clock::now();
            std::cout << "FOUND in " << (endTime - startTime).count() / 1000000.0 << "ms\n";
            return reconstruct(pathCache, goal);
        }

        for (auto dir : dirs) {
            std::pair<int,int> next = {pos.first + dir.first, pos.second + dir.second};

            if (next.first < 0 || next.first >= map.getHeight() ||
                next.second < 0 || next.second >= map.getWidth())
                continue;

            if (map._map[next.first][next.second] == 1)
                continue;

            float tentativeG = gScore[pos.first][pos.second] + 1.0f;

            if (tentativeG < gScore[next.first][next.second]) {
                gScore[next.first][next.second] = tentativeG;
                pathCache[next] = pos;

                float f = tentativeG + Heuristic(next, goal);
                OPEN.push({next, tentativeG, f});
            }
        }
    }

    std::cout << "NOT FOUND!!!!\n";
    return {};
}