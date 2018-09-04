#include <string>
#include <stdexcept>
#include "board.h"
#include "util.h"

namespace RainbowLife {

    Board::Board(SDL_Surface *destination_surface, size_t table_width, size_t table_height, size_t cell_padding) :
        destination_surface(destination_surface),
        table_width(table_width),
        table_height(table_height),
        cell_padding(cell_padding),
        table(table_height, std::vector<Cell>(table_width, nullCell)),
        nullCell{0.0, false, false}
    {
        if (static_cast<int>(table_width * (1 + cell_padding) - cell_padding) > destination_surface->w)
        {
            throw std::runtime_error(::std::string("Insufficent surface width for cell table!\n") +
                                     "    - surface width: " + std::to_string(destination_surface->w) + "px\n" +
                                     "    - table width: " + std::to_string(table_width) + "\n" +
                                     "    - minimum cell size: 1px\n" +
                                     "    - required surface width: " + std::to_string(table_width * (1 + cell_padding) - cell_padding) + "px\n");
        }

        if (static_cast<int>(table_height * (1 + cell_padding) - cell_padding) > destination_surface->h)
        {
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
    }

    void Board::randomize(size_t ratio)
    {
        for (size_t y = 0; y < table_height; y++)
        {
            for (size_t x = 0; x < table_width; x++)
            {
                cell(x, y).alive_now = rand() % ratio == 0;
                cell(x, y).color = static_cast<double>(rand()) / RAND_MAX;
            }
        }
    }

    // cell selection with coordinates
    Board::Cell& Board::cell(int x, int y)
    {
        // bounds check
        if (x < 0 || x > static_cast<int>(table_width - 1) || y < 0 || y > static_cast<int>(table_height - 1))
        {
            return nullCell;
        }

        return table[y][x];
    }

    Board::Cell& Board::operator()(int x, int y)
    {
        return cell(x, y);
    }

    void Board::tick()
    {
        for (int y = 0; y < static_cast<int>(table_height); y++)
        {
            for (int x = 0; x < static_cast<int>(table_width); x++)
            {
                size_t neighbours_alive = cell(x - 1, y - 1).alive_now +
                                          cell(x - 1, y).alive_now +
                                          cell(x - 1, y + 1).alive_now +
                                          cell(x, y - 1).alive_now +
                                          cell(x, y + 1).alive_now +
                                          cell(x + 1, y - 1).alive_now +
                                          cell(x + 1, y).alive_now +
                                          cell(x + 1, y + 1).alive_now;

                if (cell(x, y).alive_now)
                {
                    cell(x, y).alive_next_tick = (neighbours_alive == 2 || neighbours_alive == 3);
                }
                else
                {
                    cell(x, y).alive_next_tick = (neighbours_alive == 3);
                }
            }
        }

        for (int y = 0; y < static_cast<int>(table_height); y++)
        {
            for (int x = 0; x < static_cast<int>(table_width); x++)
            {
                cell(x, y).alive_now = cell(x, y).alive_next_tick;
            }
        }
    }

    void Board::render()
    {
        SDL_FillRect(destination_surface, NULL, 0);

        SDL_Rect cell_rect;
        cell_rect.w = cell_size;
        cell_rect.h = cell_size;

        for (size_t y = 0; y < table_height; y++)
        {
            for (size_t x = 0; x < table_width; x++)
            {
                if (!cell(x, y).alive_now)
                {
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