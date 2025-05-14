CC = g++
CFLAGS = -Wall -std=c++14
LDFLAGS = -lSDL2 -lSDL2_image

SRCDIR = src
INCDIR = include
OBJDIR = obj

# Remove AIController.cpp from the source files list
SRCS = $(SRCDIR)/AnimatedSprite.cpp $(SRCDIR)/Character.cpp $(SRCDIR)/Game.cpp main.cpp
OBJS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(notdir $(SRCS)))

TARGET = game

# Create object directory if it doesn't exist
$(shell mkdir -p $(OBJDIR))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# Rule for compiling source files in the src directory
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

# Rule for compiling main.cpp in the root directory
$(OBJDIR)/main.o: main.cpp
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	rm -rf $(OBJDIR)/*.o $(TARGET)

.PHONY: all clean