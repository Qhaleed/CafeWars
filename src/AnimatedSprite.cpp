#include "../include/AnimatedSprite.h"
#include <iostream>

AnimatedSprite::AnimatedSprite(SDL_Renderer* renderer, const std::string& path, 
                             int frameWidth, int frameHeight, int totalFrames)
    : frameWidth(frameWidth), frameHeight(frameHeight), totalFrames(totalFrames),
      currentFrame(0), lastFrameTime(SDL_GetTicks()), flipHorizontal(false)
{
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cerr << "IMG_Load Error loading " << path << ": " << IMG_GetError() << std::endl;
        return;
    }
    
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    srcRect = { 0, 0, frameWidth, frameHeight };
    destRect = { 0, 0, frameWidth, frameHeight };
}

AnimatedSprite::~AnimatedSprite() {
    SDL_DestroyTexture(texture);
}

void AnimatedSprite::update() {
    Uint32 now = SDL_GetTicks();
    if (now - lastFrameTime >= FRAME_DELAY) {
        lastFrameTime = now;
        currentFrame = (currentFrame + 1) % totalFrames;
        srcRect.x = currentFrame * frameWidth;
    }
}

void AnimatedSprite::render(SDL_Renderer* renderer, int x, int y, bool flipped) {
    destRect.x = x;
    destRect.y = y;
    
    SDL_RendererFlip flip = flipped ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, 0, NULL, flip);
}

void AnimatedSprite::setFrame(int frame) {
    if (frame >= 0 && frame < totalFrames) {
        currentFrame = frame;
        srcRect.x = currentFrame * frameWidth;
    }
}