#include "../include/Character.h"
#include "../include/Game.h"
#include <iostream>

Character::Character(SDL_Renderer* renderer, int startX, int startY, int floorY)
    : x(startX), y(startY), facingRight(true), currentState(IDLE),
      horizontalDirection(0), isJumping(false), jumpHeight(0)
{
    // Load basic animations
    animations[IDLE] = std::make_unique<AnimatedSprite>(renderer, "assets/sprite.png", 128, 128, 5);
    animations[WALKING] = std::make_unique<AnimatedSprite>(renderer, "assets/Walk.png", 128, 128, 6);
    animations[RUNNING] = std::make_unique<AnimatedSprite>(renderer, "assets/Run.png", 128, 128, 6);
    animations[JUMPING] = std::make_unique<AnimatedSprite>(renderer, "assets/Jump.png", 128, 128, 6);
}

Character::~Character() {
    // AnimatedSprite instances are automatically cleaned up by unique_ptr
}

void Character::handleEvents(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_SPACE && !isJumping) {
            jump();
        }
    }
}

void Character::update(const Uint8* keyState, int floorY) {
    // Update the current animation
    animations[currentState]->update();
    
    // Handle movement only if not jumping
    bool isMoving = false;
    horizontalDirection = 0;
    
    // Handle left/right movement - allow movement and direction change while jumping
    if (keyState[SDL_SCANCODE_LEFT]) { 
        moveLeft(SPRITE_SPEED);
        isMoving = true;
        horizontalDirection = -1;
        
        if (!isJumping) {
            if (keyState[SDL_SCANCODE_LSHIFT] || keyState[SDL_SCANCODE_RSHIFT]) {
                setState(RUNNING);
            } else {
                setState(WALKING);
            }
        }
    }
    
    if (keyState[SDL_SCANCODE_RIGHT]) { 
        moveRight(SPRITE_SPEED);
        isMoving = true;
        horizontalDirection = 1;
        
        if (!isJumping) {
            if (keyState[SDL_SCANCODE_LSHIFT] || keyState[SDL_SCANCODE_RSHIFT]) {
                setState(RUNNING);
            } else {
                setState(WALKING);
            }
        }
    }
    
    // Set to idle if not moving and not jumping
    if (!isMoving && !isJumping) {
        setState(IDLE);
    }
    
    // Handle jumping physics
    if (isJumping) {
        if (jumpHeight < MAX_JUMP_HEIGHT) {
            // Rising during jump
            y -= JUMP_SPEED;
            jumpHeight += JUMP_SPEED;
        } else if (y < floorY - 128) {
            // Falling back down
            y += JUMP_SPEED;
            
            // Check if landed
            if (y >= floorY - 128) {
                y = floorY - 128; // Snap to floor
                isJumping = false;
                
                // Set appropriate state based on movement when landing
                if (horizontalDirection != 0) {
                    if (keyState[SDL_SCANCODE_LSHIFT] || keyState[SDL_SCANCODE_RSHIFT]) {
                        setState(RUNNING);
                    } else {
                        setState(WALKING);
                    }
                } else {
                    setState(IDLE);
                }
            }
        }
    }
    
    // Ensure character stays within screen bounds
    if (x < 0) x = 0;
    if (x > Game::SCREEN_WIDTH - 128) x = Game::SCREEN_WIDTH - 128;
}

void Character::render(SDL_Renderer* renderer) {
    animations[currentState]->render(renderer, x, y, !facingRight);
}

void Character::setState(CharacterState state) {
    if (currentState != state) {
        currentState = state;
        animations[currentState]->setFrame(0);
    }
}

void Character::moveLeft(int speed) {
    x -= speed;
    facingRight = false;
}

void Character::moveRight(int speed) {
    x += speed;
    facingRight = true;
}

void Character::jump() {
    if (!isJumping) {
        setState(JUMPING);
        isJumping = true;
        jumpHeight = 0;
    }
}