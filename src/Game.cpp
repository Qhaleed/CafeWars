#include "../include/Game.h"
#include <iostream>

Game::Game() 
    : window(nullptr), renderer(nullptr), isRunning(false), player(nullptr) {
    floorRect = { 0, FLOOR_Y, SCREEN_WIDTH, 50 };
}

Game::~Game() {
    clean();
}

bool Game::init() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create window
    window = SDL_CreateWindow("CafeWars", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "CreateWindow Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer) {
        std::cerr << "CreateRenderer Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
        return false;
    }

    // Create player character
    player = new Character(renderer, 100, FLOOR_Y - 128, FLOOR_Y);
    
    isRunning = true;
    return true;
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }
        
        player->handleEvents(event);
    }
}

void Game::update() {
    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    
    // Update player
    player->update(keyState, FLOOR_Y);
}

void Game::render() {
    // Clear the screen with sky blue background
    SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255);
    SDL_RenderClear(renderer);

    // Draw the floor with brown color
    SDL_SetRenderDrawColor(renderer, 101, 67, 33, 255);
    SDL_RenderFillRect(renderer, &floorRect);
    
    // Render the player character
    player->render(renderer);
    
    // Present the renderer
    SDL_RenderPresent(renderer);
}

void Game::clean() {
    // Clean up player
    if (player) {
        delete player;
        player = nullptr;
    }
    
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    
    IMG_Quit();
    SDL_Quit();
}