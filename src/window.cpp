#include <stdexcept>
#include "window.h"
#include "log.hpp"

namespace RainbowLife {
    
    Window::Window()
    {
        SDL_DisplayMode current;

        SDL_Init(SDL_INIT_VIDEO);

        if (SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_INPUT_GRABBED, &window, &renderer) != 0)
        {
            throw std::runtime_error(SDL_GetError());
        }

        SDL_GetWindowSize(window, &width, &height);

        texture = SDL_CreateTexture(renderer,
                                    SDL_PIXELFORMAT_ARGB8888,
                                    SDL_TEXTUREACCESS_STREAMING,
                                    width, height);
        if (texture == NULL)
        {
            throw std::runtime_error(SDL_GetError());
        }

        surface = SDL_CreateRGBSurface(0, width, height, 32,
                                       0x00FF0000,
                                       0x0000FF00,
                                       0x000000FF,
                                       0xFF000000);
        if (surface == NULL)
        {
            throw std::runtime_error(SDL_GetError());
        }

        SDL_ShowCursor(SDL_DISABLE);
        SDL_ShowWindow(window);
        SDL_RaiseWindow(window);

        log("window = ", width, " x ", height);
    }

    SDL_Surface* Window::getSurface()
    {
        return surface;
    }

    void Window::render()
    {
        SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    Window::~Window()
    {
        SDL_DestroyWindow(window);
    }
};
