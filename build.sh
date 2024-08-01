emcc main.cpp Graph.cpp -o graphs.html -s USE_SDL=2 -s WASM=1 --preload-file Fonts -s USE_SDL_TTF=2 -s USE_SDL_GFX=2 -O3 -flto -s ASYNCIFY
