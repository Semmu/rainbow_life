#include <string>
#include <stdexcept>
#include "board.h"
#include "util.h"
#include "log.hpp"

namespace RainbowLife {

    Board::Board(SDL_Surface *destination_surface, size_t table_width, size_t table_height, size_t cell_padding) :
        destination_surface(destination_surface),
        table_width(table_width),
        table_height(table_height),
        cell_padding(cell_padding),
        table(table_height, std::vector<Cell>(table_width, nullCell)),
        nullCell{0.0, false, false},
        hoveredCell{&nullCell},
        cursorEnabled{true},
        cursorPainting{NOT_PAINTING}
    {
        if (static_cast<int>(table_width * (1 + cell_padding) - cell_padding) > destination_surface->w) {
            throw std::runtime_error(::std::string("Insufficent surface width for cell table!\n") +
                                     "    - surface width: " + std::to_string(destination_surface->w) + "px\n" +
                                     "    - table width: " + std::to_string(table_width) + "\n" +
                                     "    - minimum cell size: 1px\n" +
                                     "    - required surface width: " + std::to_string(table_width * (1 + cell_padding) - cell_padding) + "px\n");
        }

        if (static_cast<int>(table_height * (1 + cell_padding) - cell_padding) > destination_surface->h) {
            throw std::runtime_error(std::string("Insufficent surface height for cell table!\n") +
                                     "    - surface height: " + std::to_string(destination_surface->h) + "px\n" +
                                     "    - table height: " + std::to_string(table_height) + "\n" +
                                     "    - minimum cell size: 1px\n" +
                                     "    - required surface width: " + std::to_string(table_height * (1 + cell_padding) - cell_padding) + "px\n");
        }

        // figuring out max cell size for given surface and table dimensions
        size_t cell_size_x = (destination_surface->w + cell_padding) / table_width - cell_padding;
        size_t cell_size_y = (destination_surface->h + cell_padding) / table_height - cell_padding;

        // min selection
        cell_size = cell_size_x < cell_size_y ? cell_size_x : cell_size_y;

        // padding in pixels, to align the table in the middle
        padding_top = (destination_surface->h - (table_height * (cell_size + cell_padding) - cell_padding)) / 2;
        padding_left = (destination_surface->w - (table_width * (cell_size + cell_padding) - cell_padding)) / 2;

        randomize(5);

        // color table precomputation
        color_table.reserve(precomputed_colors);
        HSV hsv;
        RGB rgb;
        for (size_t i = 0; i < precomputed_colors; i++)
        {
            double hue = 1.0 * i / precomputed_colors;

            hsv.h = hue * 360;
            hsv.s = 0.7;
            hsv.v = 0.7;

            rgb = HSV2RGB(hsv);

            color_table[i] = SDL_MapRGB(destination_surface->format, rgb.r * 255, rgb.g * 255, rgb.b * 255);
        }

        color_white = SDL_MapRGB(destination_surface->format, 255, 255, 255);
        color_black = SDL_MapRGB(destination_surface->format, 0, 0, 0);
    }

    void Board::randomize(size_t ratio) {
        for (size_t y = 0; y < table_height; y++) {
            for (size_t x = 0; x < table_width; x++) {
                cell(x, y).alive_now = rand() % ratio == 0;
                cell(x, y).color = static_cast<double>(rand()) / RAND_MAX;
            }
        }
    }

    // cell selection with coordinates
    Board::Cell& Board::cell(int x, int y) {
        // bounds check
        if (x < 0 || x > static_cast<int>(table_width - 1) || y < 0 || y > static_cast<int>(table_height - 1)) {
            return nullCell;
        }

        return table[y][x];
    }

    Board::Cell& Board::operator()(int x, int y) {
        return cell(x, y);
    }

    void Board::tick()
    {
        for (int y = 0; y < static_cast<int>(table_height); y++) {
            for (int x = 0; x < static_cast<int>(table_width); x++) {
                size_t neighbours_alive = cell(x - 1, y - 1).alive_now +
                                          cell(x - 1, y).alive_now +
                                          cell(x - 1, y + 1).alive_now +
                                          cell(x, y - 1).alive_now +
                                          cell(x, y + 1).alive_now +
                                          cell(x + 1, y - 1).alive_now +
                                          cell(x + 1, y).alive_now +
                                          cell(x + 1, y + 1).alive_now;

                if (cell(x, y).alive_now) {
                    cell(x, y).alive_next_tick = (neighbours_alive == 2 || neighbours_alive == 3);
                } else {
                    cell(x, y).alive_next_tick = (neighbours_alive == 3);
                    if (cell(x, y).alive_next_tick) {

                        // 0.0 itself is a color, so we must keep track of the first inheritance
                        // it should just copy the color the first time and average the color every other time
                        double inherited_color = 0.0;
                        bool color_undefined = true;

                        // some ugly macro functions to minimize code duplication
                        #define inherit_color(from) do { \
                            if(from.alive_now) { \
                                if(color_undefined) { \
                                    inherited_color = from.color; \
                                    color_undefined = false; \
                                } else { \
                                    inherited_color = average_hue(inherited_color, from.color); \
                                } \
                            } \
                        } while ( false )

                        // inheriting color from every neighbour
                        inherit_color(cell(x - 1, y - 1));
                        inherit_color(cell(x - 1, y));
                        inherit_color(cell(x - 1, y + 1));
                        inherit_color(cell(x, y - 1));
                        inherit_color(cell(x, y + 1));
                        inherit_color(cell(x + 1, y - 1));
                        inherit_color(cell(x + 1, y));
                        inherit_color(cell(x + 1, y + 1));

                        // random mutation in any direction
                        double mutation = max_cell_mutation * rand() / RAND_MAX - ( max_cell_mutation / 2 );
                        inherited_color = from_0_to_1(inherited_color + mutation);

                        cell(x, y).color = inherited_color;
                    }
                }
            }
        }

        for (int y = 0; y < static_cast<int>(table_height); y++) {
            for (int x = 0; x < static_cast<int>(table_width); x++) {
                cell(x, y).alive_now = cell(x, y).alive_next_tick;
            }
        }
    }

    void Board::setCursorCoordinates(size_t x, size_t y) {
        if (x > padding_left &&
            x < destination_surface->w - padding_left &&
            y > padding_top &&
            y < destination_surface->h - padding_top) {

            size_t x_index, y_index;
            x_index = (x - padding_left) / (cell_size + cell_padding);
            y_index = (y - padding_top) / (cell_size + cell_padding);

            hoveredCell = &cell(x_index, y_index);

            paint();

        } else {
            hoveredCell = &nullCell;
        }
    }

    void Board::toggleCursor() {
        cursorEnabled = !cursorEnabled;
    }

    void Board::setPaintingMode(PaintingMode mode) {
        cursorPainting = mode;
        paint();
    }

    void Board::paint() {
        if (cursorEnabled && hoveredCell != &nullCell && cursorPainting != NOT_PAINTING) {
            hoveredCell->alive_now = (cursorPainting == PAINTING_ALIVE);
        }
    }

    void Board::render() {
        SDL_FillRect(destination_surface, NULL, 0);

        Uint32 cell_color, highlight_color;
        size_t color_index;
        SDL_Rect cell_rect, highlight_rect;
        cell_rect.w = cell_rect.h = cell_size;
        highlight_rect.w = highlight_rect.h = cell_size + 2;

        for (size_t y = 0; y < table_height; y++) {
            for (size_t x = 0; x < table_width; x++) {
                cell_rect.x = padding_left + x * (cell_size + cell_padding);
                cell_rect.y = padding_top + y * (cell_size + cell_padding);
                highlight_rect.x = cell_rect.x - 1;
                highlight_rect.y = cell_rect.y - 1;

                color_index = cell(x, y).color * precomputed_colors;
                if (cell(x, y).alive_now) {
                    cell_color = color_table[color_index];
                    highlight_color = color_white;
                } else {
                    cell_color = color_black;
                    highlight_color = color_table[color_index];
                }

                if (cursorEnabled && hoveredCell == &cell(x, y)) {
                    SDL_FillRect(destination_surface, &highlight_rect, highlight_color);
                }
                SDL_FillRect(destination_surface, &cell_rect, cell_color);
            }
        }
    }
};
