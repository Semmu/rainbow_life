#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "log.hpp"
#include "util.h"
#include "board.h"
#include "window.h"

int main(int argc, char const *argv[])
{
    bool running = true;
    SDL_Event e;

    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        throw std::runtime_error(SDL_GetError());
    }

    if (TTF_Init() != 0) {
        throw std::runtime_error(TTF_GetError());
    }

    atexit([]() {
        SDL_Quit();
    });

    size_t mouseX{0}, mouseY{0};

    RainbowLife::Window window;
    RainbowLife::Board board(window.getSurface(), 192, 80, 6);

    bool simulate{true};
    size_t tick_gap{20}, tick_gap_counter{0};

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

                        case SDLK_r: {
                            board.randomize(5);
                        } break;

                        case SDLK_a: {
                            simulate = !simulate;
                        } break;

                        case SDLK_f: {
                            tick_gap = tick_gap - 1 > 0 ? tick_gap - 1 : 1;
                        } break;

                        case SDLK_s: {
                            tick_gap = tick_gap + 1 < 50 ? tick_gap + 1 : 50;
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

        if (simulate) {
            tick_gap_counter++;
            if (tick_gap_counter > tick_gap) {
                tick_gap_counter = 0;
                board.tick();
            }
        }

        board.render();

        SDL_Rect mouse_rect;
        mouse_rect.x = mouseX;
        mouse_rect.y = mouseY;
        mouse_rect.w = mouse_rect.h = 20;
        SDL_FillRect(window.getSurface(), &mouse_rect, SDL_makeColor(255, 255, 255));

        window.update();

        SDL_Delay(1);
    }

    window.~Window();

    SDL_Quit();

    return 0;
}