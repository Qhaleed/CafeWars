#ifndef CHARACTER_H
#define CHARACTER_H

#include <SDL2/SDL.h>
#include <map>
#include <memory>
#include "AnimatedSprite.h"

enum CharacterState {
    IDLE,
    WALKING,
    RUNNING,
    JUMPING
};

class Character {
private:
    int x, y;
    bool facingRight;
    CharacterState currentState;
    std::map<CharacterState, std::unique_ptr<AnimatedSprite>> animations;
    
    // Character properties
    const int SPRITE_SPEED = 5;
    const int JUMP_SPEED = 8;
    const int MAX_JUMP_HEIGHT = 100;
    
    // Movement tracking for jumping
    int horizontalDirection; // -1 for left, 0 for none, 1 for right
    
    // State variables
    bool isJumping;
    int jumpHeight;
    
public:
    // Constructor for character
    Character(SDL_Renderer* renderer, int startX, int startY, int floorY);
    ~Character();
    
    void handleEvents(const SDL_Event& event);
    void update(const Uint8* keyState, int floorY);
    void render(SDL_Renderer* renderer);
    
    // State management
    void setState(CharacterState state);
    CharacterState getState() const { return currentState; }
    bool isFacingRight() const { return facingRight; }
    
    // Movement methods
    void moveLeft(int speed);
    void moveRight(int speed);
    void jump();
    
    // Position and properties
    int getX() const { return x; }
    int getY() const { return y; }
    void setX(int newX) { x = newX; }
    void setY(int newY) { y = newY; }
    int getWidth() const { return 128; } // Assuming character width is 128
};

#endif // CHARACTER_H