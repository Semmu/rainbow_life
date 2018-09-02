# compiler settings
CC := g++
CFLAGS := -O -Wall -std=c++11 `sdl2-config --cflags` $(EXTRAFLAGS)
LFLAGS := `sdl2-config --libs` -lSDL2_ttf

# source files
SRCDIR := src
SRCEXT := cpp
SRCS := $(wildcard $(SRCDIR)/*.$(SRCEXT))

# build files
BUILDDIR := build
OBJS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SRCS:.$(SRCEXT)=.o))

# output
OUTPUTDIR := bin
TARGET := rainbow_life

default: run

run: $(OUTPUTDIR)/$(TARGET)
	$(OUTPUTDIR)/$(TARGET)

# linking rule
$(OUTPUTDIR)/$(TARGET): $(OBJS)
	$(CC) $^ $(LFLAGS) -o $(OUTPUTDIR)/$(TARGET)

# compilation rule
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	$(CC) $(CFLAGS) -I include -c -o $@ $<

# cleanup
clean:
	$(RM) -r $(BUILDDIR)/*.o
	$(RM) -r $(OUTPUTDIR)/$(TARGET)
