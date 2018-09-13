#ifndef BOARD_H
#define BOARD_H

#include <SDL2/SDL.h>
#include <vector>

namespace RainbowLife {
    class Board {
    public:
        struct Cell {
            // hue, 0-1 interval
            double color;

            bool alive_now;
            bool alive_next_tick;
        };
        enum PaintingMode {
            NOT_PAINTING,
            PAINTING_ALIVE,
            PAINTING_DEAD
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
        bool deadCellsVisible,
             wrap;

        // cursor
        Cell* hoveredCell;
        bool cursorEnabled;
        PaintingMode cursorPainting;

        // mutation on cell birth
        const double max_cell_mutation = 0.05;

        // precomputed color table for hues
        const size_t precomputed_colors = 100; 
        std::vector<Uint32> color_table;

        // color consts
        Uint32 color_white, color_black;

    public:
        Board(SDL_Surface *destination_surface, size_t table_width, size_t table_height, size_t cell_padding = 4);

        Cell& cell(int x, int y);
        Cell& operator()(int x, int y);

        void toggleWrap();
        void clear();
        void randomizeBoard(size_t fillRatio = 5);
        void randomizeColors();
        void tick();

        void setCursorCoordinates(size_t x, size_t y);
        void toggleCursor();
        void setPaintingMode(PaintingMode mode);
        void paint();

        void toggleDeadCellVisibility();
        void render();
    };
}

#endif /* BOARD_H */
