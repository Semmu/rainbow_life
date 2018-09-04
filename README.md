# Rainbow Life, a Game of Life with colorful cells

(a screenshot here, soon)

This is just a simple Game of Life implementation in C++ using SDL - with a small twist: cells are colored, they inherit the average color of their neighbourhood upon birth with a slight change to mimic evolution.

## Requirements

* C++ compiler
* SDL2 development libraries

## Building

```
make
```

## TODO

(for my own notes)

* cell selection on mouse hover
* cell toggling with cursor click
* toggle board wrap
* render board in pattern if wrap is enabled
* usage screen (which button does what), status display (speed, size, etc)
* better game loop (tie to fps)
* cell and board "rework" (cells shouldnt next tick state, there should be 2 boards instead, switching active and next on tick) (a basic test version of this did not perform better, should figure out more ways to optimize, for example with a precomputed color map)
* board constructor rework (get surface, cell padding and cell size, should fill surface with as many cells as possible)
* fix suspicious hue bug (red and yellow appears next to each other all the time, they shouldnt)
* multithread support for simulation (and rendering?)

## License

MIT.