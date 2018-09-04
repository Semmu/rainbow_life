#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>

namespace RainbowLife {
    class Window {
    private:
        int width, height;
        SDL_Window *window;

    public:
        Window();
        SDL_Surface* getSurface();
        void update();
        ~Window();
    };
}

#endif /* WINDOW_H */