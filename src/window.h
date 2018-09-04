#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>

namespace RainbowLife {
    class Window {
    private:
        int width, height;

        SDL_Window *window;
        SDL_Renderer *renderer;
        SDL_Texture *texture;
        SDL_Surface *surface;

    public:
        Window();
        SDL_Surface* getSurface();
        void render();
        ~Window();
    };
}

#endif /* WINDOW_H */