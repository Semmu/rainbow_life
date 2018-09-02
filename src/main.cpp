#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

const int WIDTH = 1920;
const int HEIGHT = 1080;

void fatal_error(const char *reason)
{
    std::cerr << std::endl
              << "[FATAL ERROR]: " << reason << std::endl
              << std::endl;
    exit(1);
}

Uint32 SDL_makeColor(Uint8 r, Uint8 g, Uint8 b) {
    return (r << 16) | (g << 8) | (b << 0);
}

bool running = true;
SDL_Event e;

int main(int argc, char const *argv[])
{
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

    if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_FULLSCREEN, &window, &renderer) != 0) {
        fatal_error(SDL_GetError());
    }

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

                        case SDLK_ESCAPE: {
                            running = false;
                        } break;

                        default: break;
                    }
                } break;

                default: break;
            }
        }

        SDL_FillRect(surface, NULL, SDL_makeColor(rand() % 255, 0, 0));

        SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        SDL_Delay(100);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
