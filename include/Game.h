#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Character.h"

class Game {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;
    
    // Player character
    Character* player;
    
    // Floor rendering
    SDL_Rect floorRect;
    const int FLOOR_Y = 400;
    
public:
    // Using enum for constants to avoid linking issues
    enum {
        SCREEN_WIDTH = 1000,
        SCREEN_HEIGHT = 600
    };
    
    Game();
    ~Game();
    
    bool init();
    void handleEvents();
    void update();
    void render();
    void clean();
    
    bool running() const { return isRunning; }
    SDL_Renderer* getRenderer() const { return renderer; }
    int getFloorY() const { return FLOOR_Y; }
};

#endif // GAME_H