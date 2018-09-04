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

* Window class, fake fullscreen window, correct size identification (for multimonitor systems)
* cell selection on mouse hover
* cell toggling with cursor click
* toggle board wrap
* render board in pattern if wrap is enable
* auto simulation with changeable speed

## License

MIT.