#include <stdexcept>
#include "window.h"
#include "log.hpp"

namespace RainbowLife {
    
    Window::Window()
    {
        window = SDL_CreateWindow("Rainbow Life",
                                  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0,
                                  SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_INPUT_GRABBED);

        SDL_GetWindowSize(window, &width, &height);

        SDL_ShowCursor(SDL_DISABLE);
        SDL_ShowWindow(window);
        SDL_RaiseWindow(window);
    }

    SDL_Surface* Window::getSurface()
    {
        return SDL_GetWindowSurface(window);
    }

    void Window::update()
    {
        SDL_UpdateWindowSurface(window);
    }

    Window::~Window()
    {
        SDL_DestroyWindow(window);
    }
};
