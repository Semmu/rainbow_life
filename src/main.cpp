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

    RainbowLife::Window window;
    RainbowLife::Board board(window.getSurface(), 192, 108, 4);

    bool running = true;
    SDL_Event e;
    size_t mouseX{0}, mouseY{0};
    bool simulate{true};
    int latest_simulation = SDL_GetTicks(),
        simulation_gap = 100;
    const int max_simulation_gap = 1000,
              simulation_gap_delta = 20;

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
                            simulation_gap = simulation_gap - simulation_gap_delta > 0 ? simulation_gap - simulation_gap_delta : 1;
                        } break;

                        case SDLK_s: {
                            simulation_gap = simulation_gap + simulation_gap_delta < max_simulation_gap ? simulation_gap + simulation_gap_delta : max_simulation_gap;
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
            if (static_cast<int>(SDL_GetTicks()) - latest_simulation > simulation_gap) {
                board.tick();
                latest_simulation = SDL_GetTicks();
            }
        }

        board.render();

        SDL_Rect mouse_rect;
        mouse_rect.x = mouseX;
        mouse_rect.y = mouseY;
        mouse_rect.w = mouse_rect.h = 20;
        //SDL_FillRect(window.getSurface(), &mouse_rect, SDL_makeColor(255, 255, 255));

        window.update();

        SDL_Delay(1);
    }

    window.~Window();

    SDL_Quit();

    return 0;
}