#ifndef GRAPH_H
#define GRAPH_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Node {
public:
  Node(SDL_Point position);
  SDL_Rect getCircle() const;
  void setCircle(const SDL_Rect &circle);
  int getID() const;
  void addNeighbor(const std::shared_ptr<Node> &neighbor);
  void removeNeighbor(const std::shared_ptr<Node> &neighbor);
  const std::vector<std::shared_ptr<Node>> &getNeighbors() const;
  void draw(SDL_Renderer *renderer, TTF_Font *font) const;
  static int nextID;

private:
  int id;
  SDL_Rect circle;
  std::vector<std::shared_ptr<Node>> neighbors;
};

class Graph {
public:
  std::shared_ptr<Node> addNode(SDL_Point position);
  void pop_nodes();
  bool hasEdge(const std::shared_ptr<Node> &node1,
               const std::shared_ptr<Node> &node2) const;
  void addEdge(const std::shared_ptr<Node> &node1,
               const std::shared_ptr<Node> &node2,
               std::optional<int> weight = std::nullopt);
  void removeEdge(const std::shared_ptr<Node> &node1,
                  const std::shared_ptr<Node> &node2);
  bool hasCycle() const;
  const std::vector<std::shared_ptr<Node>> &getNodes() const;
  void draw(SDL_Renderer *renderer, TTF_Font *font) const;
  std::shared_ptr<Node> getNodeAt(SDL_Point point) const;
  void clear();
  void info() const;
  void setStartNode(std::shared_ptr<Node> searchStartNode);
  void setEndNode(std::shared_ptr<Node> searchStartNode);
  std::shared_ptr<Node> getStartNode() const;
  std::shared_ptr<Node> getEndNode() const;
  int shortestPath(const std::shared_ptr<Node> &start,
                   const std::shared_ptr<Node> &end,
                   std::vector<std::shared_ptr<Node>> &path) const;

private:
  std::vector<std::shared_ptr<Node>> nodes;
  std::vector<std::tuple<std::shared_ptr<Node>, std::shared_ptr<Node>,
                         std::optional<int>>>
      edges;
  std::shared_ptr<Node> searchStartNode;
  std::shared_ptr<Node> searchEndNode;
  bool hasCycleUtil(const std::shared_ptr<Node> &node,
                    std::unordered_set<int> &visited, int parent) const;
};

#endif // GRAPH_H
