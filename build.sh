emcc main.cpp Graph.cpp -o index.html -s USE_SDL=2 -s WASM=1 --preload-file Fonts -s USE_SDL_TTF=2 -s USE_SDL_GFX=2 -O3 -flto -s ASYNCIFY

# For reformatting the output files of emcc
js-beautify index.html -o temp_index.html && mv temp_index.html index.html
js-beautify graphs.js -o temp_graphs.js && mv temp_graphs.js graphs.js

