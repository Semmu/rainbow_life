#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "log.hpp"
#include "util.h"
#include "board.h"
#include "window.h"
#include "timer.h"

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
    RainbowLife::Board board(window.getSurface(), 192, 108, 1);

    bool running = true;
    SDL_Event e;
    bool simulate{true}, before_painting_state{true};
    int latest_simulation = SDL_GetTicks(),
        simulation_gap = 100;
    const int max_simulation_gap = 1000,
              simulation_gap_delta = 20;

    Timer timer;

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

                        case SDLK_n: {
                            board.clear();
                        } break;

                        case SDLK_r: {
                            board.randomizeBoard(5);
                        } break;

                        case SDLK_g: {
                            board.randomizeColors();
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

                        case SDLK_c: {
                            board.toggleCursor();
                        } break;

                        case SDLK_d: {
                            board.toggleDeadCellVisibility();
                        } break;

                        case SDLK_w: {
                            board.toggleWrap();
                        } break;

                        case SDLK_ESCAPE: {
                            running = false;
                        } break;

                        default: break;
                    }
                } break;

                case SDL_MOUSEMOTION: {
                    board.setCursorCoordinates(e.motion.x, e.motion.y);
                } break;

                case SDL_MOUSEBUTTONDOWN: {
                    before_painting_state = simulate;

                    if (e.button.button == SDL_BUTTON_LEFT) {
                        board.setPaintingMode(RainbowLife::Board::PAINTING_ALIVE);
                    }
                    if (e.button.button == SDL_BUTTON_RIGHT) {
                        board.setPaintingMode(RainbowLife::Board::PAINTING_DEAD);
                    }

                    simulate = false;
                } break;

                case SDL_MOUSEBUTTONUP: {
                    simulate = before_painting_state;
                    board.setPaintingMode(RainbowLife::Board::NOT_PAINTING);
                } break;

                default: break;
            }
        }

        if (simulate) {
            if (static_cast<int>(SDL_GetTicks()) - latest_simulation > simulation_gap) {
                // timer.start();
                board.tick();
                // timer.stop();
                // log("tick took ", timer.duration(), "ms");
                latest_simulation = SDL_GetTicks();
            }
        }

        // timer.start();
        board.render();
        //SDL_FillRect(window.getSurface(), &mouse_rect, SDL_makeColor(255, 255, 255));
        window.update();
        // timer.stop();
        // log("render took ", timer.duration(), "ms");

        SDL_Delay(1);
    }

    window.~Window();

    SDL_Quit();

    return 0;
}