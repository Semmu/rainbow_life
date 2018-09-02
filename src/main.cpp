#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "util.h"

const int WIDTH = 1920;
const int HEIGHT = 1080;
const int CELL_SIZE = 10;
const int CELL_PADDING = 4;

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

        SDL_FillRect(surface, NULL, 0);

        const size_t cell_count_x = (WIDTH + CELL_PADDING) / (CELL_SIZE + CELL_PADDING);
        const size_t cell_count_y = (HEIGHT + CELL_PADDING) / (CELL_SIZE + CELL_PADDING);
        const size_t padding_left = (WIDTH - (CELL_SIZE + CELL_PADDING) * cell_count_x) / 2;
        const size_t padding_top = (HEIGHT - (CELL_SIZE + CELL_PADDING) * cell_count_y) / 2;
        
        SDL_Rect cell_rect;
        cell_rect.w = CELL_SIZE;
        cell_rect.h = CELL_SIZE;

        for(size_t y = 0; y < cell_count_y; y++)
        {
            for(size_t x = 0; x < cell_count_x; x++)
            {
                cell_rect.x = padding_left + x * (CELL_SIZE + CELL_PADDING) + CELL_PADDING / 2;
                cell_rect.y = padding_top + y * (CELL_SIZE + CELL_PADDING) + CELL_PADDING / 2;

                HSV hsv;
                hsv.h = rand() % 360;
                hsv.s = 0.7;
                hsv.v = 0.7;

                RGB rgb = HSV2RGB(hsv);

                SDL_FillRect(surface, &cell_rect, SDL_makeColor(rgb.r * 255, rgb.g * 255, rgb.b * 255));
            }
        }

        SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        SDL_Delay(1000);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
