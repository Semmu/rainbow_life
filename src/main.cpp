#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "util.h"

void fatal_error(const char *reason) {
    std::cerr << std::endl
              << "[FATAL ERROR]: " << reason << std::endl
              << std::endl;
    exit(1);
}

template <typename subject_type>
void _log(subject_type subject)
{
    std::cout << subject << std::endl;
}

template <typename first_type, typename... remaining>
void _log(first_type first, remaining... parameters)
{
    std::cout << first;
    _log(parameters...);
}

template <typename... types>
void log(types... parameters)
{
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::cout << "[ " << std::put_time(std::localtime(&now_c), "%T") << " ] ";
    _log(parameters...);
}

Uint32 SDL_makeColor(Uint8 r, Uint8 g, Uint8 b) {
    return (r << 16) | (g << 8) | (b << 0);
}

const int WIDTH = 1920;
const int HEIGHT = 1080;

class Board {
public:
    struct Cell {
        // hue, 0-1 interval
        double color;

        bool alive_now;
        bool alive_next_tick;
    };

private:
    // provided
    SDL_Surface *destination_surface;
    size_t table_width, table_height, cell_padding;

    // computed
    size_t cell_size, padding_top, padding_left;

    // cells
    std::vector<std::vector<Cell>> table;
    Cell nullCell;

public:
    Board(SDL_Surface* destination_surface, size_t table_width, size_t table_height, size_t cell_padding = 4) :
        destination_surface(destination_surface),
        table_width(table_width),
        table_height(table_height),
        cell_padding(cell_padding),
        nullCell{0.0, false, false},
        table(table_height, std::vector<Cell>(table_width, nullCell)) {

        if (table_width * ( 1 + cell_padding ) - cell_padding > destination_surface->w ) {
            throw std::runtime_error(std::string("Insufficent surface width for cell table!\n") +
                                     "    - surface width: " + std::to_string(destination_surface->w) + "px\n" +
                                     "    - table width: " + std::to_string(table_width) + "\n" +
                                     "    - minimum cell size: 1px\n" +
                                     "    - required surface width: " + std::to_string(table_width * ( 1 + cell_padding ) - cell_padding) + "px\n");
        }

        if (table_height * ( 1 + cell_padding ) - cell_padding > destination_surface->h ) {
            throw std::runtime_error(std::string("Insufficent surface height for cell table!\n") +
                                     "    - surface height: " + std::to_string(destination_surface->h) + "px\n" +
                                     "    - table height: " + std::to_string(table_height) + "\n" +
                                     "    - minimum cell size: 1px\n" +
                                     "    - required surface width: " + std::to_string(table_height * ( 1 + cell_padding ) - cell_padding) + "px\n");
        }

        // figuring out max cell size for given surface and table dimensions
        size_t cell_size_x = ( destination_surface->w + cell_padding ) / table_width - cell_padding;
        log(destination_surface->h, "+", cell_padding, " / ", table_height, " - ", cell_padding);
        size_t cell_size_y = ( destination_surface->h + cell_padding ) / table_height - cell_padding;
        log("cell s x ", cell_size_x, ", cell s y ", cell_size_y);

        // min selection
        cell_size = cell_size_x < cell_size_y ? cell_size_x : cell_size_y;
        log("cell s ", cell_size);

        log("table h ", table_height, ", w ", table_width);
        log("surface h ", destination_surface->h, ", w ", destination_surface->w);

        log("table px h ", table_height * (cell_size + cell_padding) - cell_padding);
        log("table px w ", table_width * (cell_size + cell_padding) - cell_padding);

        // padding in pixels, to align the table in the middle
        padding_top = (destination_surface->h - (table_height * (cell_size + cell_padding) - cell_padding)) / 2;
        padding_left = ( destination_surface->w - ( table_width * ( cell_size + cell_padding ) - cell_padding ) ) / 2;
        log("pad top ", padding_top, ", left ", padding_left);

        randomize(5);
    }

    void randomize(size_t ratio = 5) {
        for (size_t y = 0; y < table_height; y++) {
            for (size_t x = 0; x < table_width; x++) {
                cell(x, y).alive_now = rand() % ratio == 0;
                cell(x, y).color = static_cast<double>(rand()) / RAND_MAX;
            }
        }
    }

    // cell selection with coordinates
    Cell& cell(int x, int y) {
        // bounds check
        if (x < 0 || x > table_width - 1 || y < 0 || y > table_height - 1) {
            return nullCell;
        }

        return table[y][x];
    }

    Cell& operator()(int x, int y)
    {
        return cell(x, y);
    }

    void tick() {
        for (int y = 0; y < table_height; y++) {
            for (int x = 0; x < table_width; x++) {
                size_t neighbours_alive = cell(x - 1, y - 1).alive_now +
                                          cell(x - 1, y).alive_now +
                                          cell(x - 1, y + 1).alive_now +
                                          cell(x, y - 1).alive_now +
                                          cell(x, y + 1).alive_now +
                                          cell(x + 1, y - 1).alive_now +
                                          cell(x + 1, y).alive_now +
                                          cell(x + 1, y + 1).alive_now;

                if (cell(x, y).alive_now) {
                    cell(x, y).alive_next_tick = ( neighbours_alive == 2 || neighbours_alive == 3 );
                } else {
                    cell(x, y).alive_next_tick = ( neighbours_alive == 3 );
                }
            }
        }

        for (int y = 0; y < table_height; y++) {
            for (int x = 0; x < table_width; x++) {
                cell(x, y).alive_now = cell(x, y).alive_next_tick;
            }
        }
    }

    void render() {
        SDL_FillRect(destination_surface, NULL, 0);

        SDL_Rect cell_rect;
        cell_rect.w = cell_size;
        cell_rect.h = cell_size;

        for (size_t y = 0; y < table_height; y++) {
            for (size_t x = 0; x < table_width; x++) {
                if (!cell(x, y).alive_now) {
                    continue;
                }

                cell_rect.x = padding_left + x * (cell_size + cell_padding);
                cell_rect.y = padding_top + y * (cell_size + cell_padding);

                HSV hsv;
                hsv.h = cell(x, y).color * 360;
                hsv.s = 0.7;
                hsv.v = 0.7;

                RGB rgb = HSV2RGB(hsv);

                SDL_FillRect(destination_surface, &cell_rect, SDL_makeColor(rgb.r * 255, rgb.g * 255, rgb.b * 255));
            }
        }
    }
};

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

    Board board(surface, 192, 80, 6);
    size_t mouseX, mouseY;

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
