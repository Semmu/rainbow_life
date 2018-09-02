#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "util.h"

const int WIDTH = 1920;
const int HEIGHT = 1080;
const int CELL_SIZE = 10;
const int CELL_PADDING = 4;

const size_t CELL_COUNT_X = (WIDTH + CELL_PADDING) / (CELL_SIZE + CELL_PADDING);
const size_t CELL_COUNT_Y = (HEIGHT + CELL_PADDING) / (CELL_SIZE + CELL_PADDING);
const size_t WINDOW_PADDING_LEFT = (WIDTH - (CELL_SIZE + CELL_PADDING) * CELL_COUNT_X) / 2;
const size_t WINDOW_PADDING_TOP = (HEIGHT - (CELL_SIZE + CELL_PADDING) * CELL_COUNT_Y) / 2;

struct Cell
{
    double color; // hue, 0-360 degrees

    bool alive_now;
    bool alive_next_tick;
};

typedef Cell CellRow[CELL_COUNT_X];
typedef CellRow CellBoard[CELL_COUNT_Y];

CellBoard board;

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
        
        SDL_Rect cell_rect;
        cell_rect.w = CELL_SIZE;
        cell_rect.h = CELL_SIZE;

        for (size_t y = 0; y < CELL_COUNT_Y; y++)
        {
            for (size_t x = 0; x < CELL_COUNT_X; x++)
            {
                board[y][x].alive_now = rand() % 3 == 0;
                board[y][x].color = rand() % 360;
            }
        }

        for(size_t y = 0; y < CELL_COUNT_Y; y++)
        {
            for(size_t x = 0; x < CELL_COUNT_X; x++)
            {
                if (!board[y][x].alive_now) {
                    continue;
                }

                cell_rect.x = WINDOW_PADDING_LEFT + x * (CELL_SIZE + CELL_PADDING) + CELL_PADDING / 2;
                cell_rect.y = WINDOW_PADDING_TOP + y * (CELL_SIZE + CELL_PADDING) + CELL_PADDING / 2;

                HSV hsv;
                hsv.h = board[y][x].color;
                hsv.s = 0.7;
                hsv.v = 0.7;

                RGB rgb = HSV2RGB(hsv);

                SDL_FillRect(surface, &cell_rect, SDL_makeColor(rgb.r * 255, rgb.g * 255, rgb.b * 255));
            }
        }

        SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        SDL_Delay(100);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
