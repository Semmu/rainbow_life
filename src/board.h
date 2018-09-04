#ifndef BOARD_H
#define BOARD_H

#include <SDL2/SDL.h>
#include <vector>

namespace RainbowLife {
    class Board
    {
    public:
        struct Cell
        {
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

        // mutation on cell birth
        const double max_cell_mutation = 0.05;

    public:
        Board(SDL_Surface *destination_surface, size_t table_width, size_t table_height, size_t cell_padding = 4);

        Cell& cell(int x, int y);
        Cell& operator()(int x, int y);

        void randomize(size_t ratio = 5);
        void tick();

        void render();
    };
}

#endif /* BOARD_H */
