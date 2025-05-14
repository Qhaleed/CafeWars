#include "include/Game.h"
#include <iostream>

const int FPS = 60;
const int FRAME_DELAY = 1000 / FPS;

int main() {
    // Create game instance
    Game game;
    
    // Initialize the game
    if (!game.init()) {
        std::cerr << "Failed to initialize game!" << std::endl;
        return 1;
    }
    
    // Game loop variables
    Uint32 frameStart;
    int frameTime;
    
    // Main game loop
    while (game.running()) {
        frameStart = SDL_GetTicks();
        
        game.handleEvents();
        game.update();
        game.render();
        
        // Cap the frame rate
        frameTime = SDL_GetTicks() - frameStart;
        if (FRAME_DELAY > frameTime) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }
    
    // Game is done
    return 0;
}
