#include "Graph.h"

constexpr auto FONT = "Fonts/JetBrainsMono-Bold.ttf";
constexpr unsigned int APP_WIDTH = 1920;
constexpr unsigned int APP_HEIGHT = 768;

int main() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError()
              << std::endl;
    return 1;
  }

  if (TTF_Init() == -1) {
    std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError()
              << std::endl;
    SDL_Quit();
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow(
      "Graph Visualization App", SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED, APP_WIDTH, APP_HEIGHT, SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError()
              << std::endl;
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError()
              << std::endl;
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  TTF_Font *font = TTF_OpenFont(FONT, 20);
  if (font == nullptr) {
    std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError()
              << std::endl;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  Graph graph;

  bool quit = false;
  SDL_Event e;

  std::shared_ptr<Node> dragStart = nullptr;

  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      } else if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (e.button.button == SDL_BUTTON_LEFT) {
          SDL_Point clickPoint = {e.button.x, e.button.y};
          auto clickedNode = graph.getNodeAt(clickPoint);
          if (clickedNode) {
            dragStart = clickedNode;
          } else {
            graph.addNode(clickPoint);
          }
        }
      } else if (e.type == SDL_MOUSEBUTTONUP) {
        if (e.button.button == SDL_BUTTON_LEFT && dragStart) {
          SDL_Point releasePoint = {e.button.x, e.button.y};
          auto releaseNode = graph.getNodeAt(releasePoint);
          if (releaseNode && releaseNode != dragStart) {
            graph.addEdge(dragStart, releaseNode);
          }
          dragStart = nullptr;
        }
      } else if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_c:
          graph.clear();
          break;
        case SDLK_i:
          graph.info();
          break;
        case SDLK_ESCAPE:
          quit = true;
          break;
        }
      }
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White background
    SDL_RenderClear(renderer);

    graph.draw(renderer, font);

    // Draw drag line if dragging
    if (dragStart) {
      int mouseX, mouseY;
      SDL_GetMouseState(&mouseX, &mouseY);
      SDL_Rect startCircle = dragStart->getCircle();
      SDL_SetRenderDrawColor(renderer, 0, 0, 255,
                             255); // Blue color for drag line
      SDL_RenderDrawLine(renderer, startCircle.x + startCircle.w / 2,
                         startCircle.y + startCircle.h / 2, mouseX, mouseY);
    }

    SDL_RenderPresent(renderer);
  }

  TTF_CloseFont(font);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_Quit();
  SDL_Quit();

  return 0;
}
