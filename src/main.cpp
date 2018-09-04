#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "log.hpp"
#include "util.h"
#include "board.h"

void fatal_error(const char *reason) {
    std::cerr << std::endl
              << "[FATAL ERROR]: " << reason << std::endl
              << std::endl;
    exit(1);
}

const int WIDTH = 1920;
const int HEIGHT = 1080;


int main(int argc, char const *argv[])
{
    bool running = true;
    SDL_Event e;

    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fatal_error(SDL_GetError());
    }

    if (TTF_Init() != 0) {
        fatal_error(TTF_GetError());
    }

    atexit(SDL_Quit);

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Surface *surface;

    if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_INPUT_GRABBED, &window, &renderer) != 0) {
        fatal_error(SDL_GetError());
    }

    SDL_RaiseWindow(window);
    SDL_ShowCursor(SDL_DISABLE);

    texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING,
                                WIDTH, HEIGHT);
    if (texture == NULL) {
        fatal_error(SDL_GetError());
    }

    surface = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32,
                                   0x00FF0000,
                                   0x0000FF00,
                                   0x000000FF,
                                   0xFF000000);
    if (surface == NULL) {
        fatal_error(SDL_GetError());
    }

    RainbowLife::Board board(surface, 192, 80, 6);
    size_t mouseX{0}, mouseY{0};

    while (running)
    {
        while (SDL_PollEvent(&e))
        {
            switch (e.type) {
                case SDL_QUIT: {
                    running = false;
                } break;

                case SDL_KEYDOWN: {
                    switch (e.key.keysym.sym) {
                        case SDLK_q: {
                            running = false;
                        } break;

                        case SDLK_t: {
                            board.tick();
                        } break;

                        case SDLK_ESCAPE: {
                            running = false;
                        } break;

                        default: break;
                    }
                } break;

                case SDL_MOUSEMOTION: {
                    mouseX = e.motion.x;
                    mouseY = e.motion.y;
                } break;

                default: break;
            }
        }

        board.render();

        SDL_Rect mouse_rect;
        mouse_rect.x = mouseX;
        mouse_rect.y = mouseY;
        mouse_rect.w = mouse_rect.h = 20;
        SDL_FillRect(surface, &mouse_rect, SDL_makeColor(255, 255, 255));

        SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        SDL_Delay(10);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
