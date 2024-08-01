#include "Graph.h"

int Node::nextID = 0;

Node::Node(SDL_Point position) : id(nextID++) {
  circle = {position.x - 20, position.y - 20, 40, 40};
}

SDL_Rect Node::getCircle() const { return circle; }

void Node::setCircle(const SDL_Rect &circle) { this->circle = circle; }

int Node::getID() const { return id; }

void Node::addNeighbor(const std::shared_ptr<Node> &neighbor) {
  if (std::find(neighbors.begin(), neighbors.end(), neighbor) ==
      neighbors.end()) {
    neighbors.emplace_back(neighbor);
  }
}

void Node::removeNeighbor(const std::shared_ptr<Node> &neighbor) {
  auto it = std::find(neighbors.begin(), neighbors.end(), neighbor);
  if (it != neighbors.end()) {
    neighbors.erase(it);
  }
}

const std::vector<std::shared_ptr<Node>> &Node::getNeighbors() const {
  return neighbors;
}

void Node::draw(SDL_Renderer *renderer, TTF_Font *font) const {
  // Draw circle
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black color
  SDL_RenderDrawRect(renderer, &circle);

  // Draw text
  SDL_Color color = {0, 0, 0, 255};
  std::string idString = std::to_string(id);
  SDL_Surface *textSurface =
      TTF_RenderText_Blended(font, idString.c_str(), color);
  if (textSurface == nullptr) {
    std::cerr << "TTF_RenderText_Blended Error: " << TTF_GetError()
              << std::endl;
    return;
  }
  SDL_Texture *textTexture =
      SDL_CreateTextureFromSurface(renderer, textSurface);
  if (textTexture == nullptr) {
    std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError()
              << std::endl;
    SDL_FreeSurface(textSurface);
    return;
  }

  int textWidth, textHeight;
  SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);
  SDL_Rect textRect = {circle.x + circle.w / 2 - textWidth / 2,
                       circle.y + circle.h / 2 - textHeight / 2, textWidth,
                       textHeight};

  SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

  SDL_FreeSurface(textSurface);
  SDL_DestroyTexture(textTexture);
}

std::shared_ptr<Node> Graph::addNode(SDL_Point position) {
  auto newNode = std::make_shared<Node>(position);
  nodes.push_back(newNode);
  return newNode;
}

void Graph::pop_nodes() {
  if (!nodes.empty()) {
    nodes.pop_back();
  }
}

bool Graph::hasEdge(const std::shared_ptr<Node> &node1,
                    const std::shared_ptr<Node> &node2) const {
  return std::any_of(edges.begin(), edges.end(), [&](const auto &edge) {
    return (std::get<0>(edge) == node1 && std::get<1>(edge) == node2) ||
           (std::get<0>(edge) == node2 && std::get<1>(edge) == node1);
  });
}

void Graph::addEdge(const std::shared_ptr<Node> &node1,
                    const std::shared_ptr<Node> &node2,
                    std::optional<int> weight) {
  if (node1 != node2 && !hasEdge(node1, node2)) {
    edges.push_back({node1, node2, weight});
    node1->addNeighbor(node2);
    node2->addNeighbor(node1);
  }
}

void Graph::removeEdge(const std::shared_ptr<Node> &node1,
                       const std::shared_ptr<Node> &node2) {
  auto it = std::remove_if(edges.begin(), edges.end(), [&](const auto &edge) {
    return (std::get<0>(edge) == node1 && std::get<1>(edge) == node2) ||
           (std::get<0>(edge) == node2 && std::get<1>(edge) == node1);
  });
  edges.erase(it, edges.end());

  node1->removeNeighbor(node2);
  node2->removeNeighbor(node1);
}

const std::vector<std::shared_ptr<Node>> &Graph::getNodes() const {
  return nodes;
}

std::shared_ptr<Node> Graph::getNodeAt(SDL_Point point) const {
  for (const auto &node : nodes) {
    SDL_Rect circle = node->getCircle();
    int dx = point.x - (circle.x + circle.w / 2);
    int dy = point.y - (circle.y + circle.h / 2);
    if (dx * dx + dy * dy <= (circle.w / 2) * (circle.w / 2)) {
      return node;
    }
  }
  return nullptr;
}

void Graph::draw(SDL_Renderer *renderer, TTF_Font *font) const {
  // Draw nodes and edges
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black color for edges
  for (const auto &node : nodes) {
    node->draw(renderer, font);
    for (const auto &neighbor : node->getNeighbors()) {
      SDL_Rect circle1 = node->getCircle();
      SDL_Rect circle2 = neighbor->getCircle();
      SDL_RenderDrawLine(renderer, circle1.x + circle1.w / 2,
                         circle1.y + circle1.h / 2, circle2.x + circle2.w / 2,
                         circle2.y + circle2.h / 2);
    }
  }
}

void Graph::clear() {
  nodes.clear();
  edges.clear();
  Node::nextID = 0;
  searchStartNode = nullptr;
  searchEndNode = nullptr;
}

void Graph::info() const {
  for (const auto &node : nodes) {
    std::cout << "Node ID: " << node->getID() << std::endl;
    const auto &neighbors = node->getNeighbors();
    std::cout << "Neighbors: ";
    if (neighbors.empty()) {
      std::cout << "Empty " << std::endl;
    } else {
      for (const auto &neighbor : neighbors) {
        std::cout << neighbor->getID() << " ";
      }
      std::cout << std::endl;
    }
    std::cout << "-------------------" << std::endl;
  }
  std::string hasCycle = (this->hasCycle()) ? "Yes" : "No";
  std::cout << "Contains a cycle: " << hasCycle << std::endl;
  std::cout << "-------------------" << std::endl;
  if (searchStartNode && searchEndNode) {
    std::vector<std::shared_ptr<Node>> path;
    std::cout << "Shortest Path length is: "
              << shortestPath(searchStartNode, searchEndNode, path)
              << std::endl;
    std::cout << "Path is: ";
    for (auto it = path.begin(); it != path.end(); ++it) {
      if (it == path.begin()) {
        std::cout << (*it)->getID();
      } else {
        std::cout << "->" << (*it)->getID();
      }
    }
    std::cout << std::endl;
  }
  std::cout << "-------------------" << std::endl;
  std::cout << std::endl;
}

void Graph::setStartNode(std::shared_ptr<Node> searchStartNode) {
  this->searchStartNode = searchStartNode;
}

void Graph::setEndNode(std::shared_ptr<Node> searchEndNode) {
  this->searchEndNode = searchEndNode;
}

std::shared_ptr<Node> Graph::getStartNode() const { return searchStartNode; }

std::shared_ptr<Node> Graph::getEndNode() const { return searchEndNode; }

bool Graph::hasCycle() const {
  std::unordered_set<int> visited;
  for (const auto &node : nodes) {
    if (visited.find(node->getID()) == visited.end()) {
      if (hasCycleUtil(node, visited, -1)) {
        return true;
      }
    }
  }
  return false;
}

bool Graph::hasCycleUtil(const std::shared_ptr<Node> &node,
                         std::unordered_set<int> &visited, int parent) const {
  visited.insert(node->getID());

  for (const auto &neighbor : node->getNeighbors()) {
    if (visited.find(neighbor->getID()) == visited.end()) {
      if (hasCycleUtil(neighbor, visited, node->getID())) {
        return true;
      }
    } else if (neighbor->getID() != parent) {
      return true;
    }
  }
  return false;
}

int Graph::shortestPath(const std::shared_ptr<Node> &start,
                        const std::shared_ptr<Node> &end,
                        std::vector<std::shared_ptr<Node>> &path) const {
  std::unordered_map<int, int> distances;
  std::unordered_map<int, std::shared_ptr<Node>> previous;
  std::priority_queue<std::pair<int, std::shared_ptr<Node>>,
                      std::vector<std::pair<int, std::shared_ptr<Node>>>,
                      std::greater<>>
      pq;

  for (const auto &node : nodes) {
    distances[node->getID()] = std::numeric_limits<int>::max();
  }

  distances[start->getID()] = 0;
  pq.push({0, start});

  while (!pq.empty()) {
    auto current = pq.top().second;
    pq.pop();

    if (current == end) {
      break;
    }

    for (const auto &neighbor : current->getNeighbors()) {
      int newDist =
          distances[current->getID()] + 1; // Assuming edge weight is 1
      if (newDist < distances[neighbor->getID()]) {
        distances[neighbor->getID()] = newDist;
        previous[neighbor->getID()] = current;
        pq.push({newDist, neighbor});
      }
    }
  }

  if (previous.find(end->getID()) == previous.end()) {
    return -1; // No path found
  }

  // Reconstruct path
  for (auto at = end; at != nullptr; at = previous[at->getID()]) {
    path.push_back(at);
  }
  std::reverse(path.begin(), path.end());

  return distances[end->getID()];
}
