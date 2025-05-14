#ifndef ANIMATED_SPRITE_H
#define ANIMATED_SPRITE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

class AnimatedSprite {
private:
    SDL_Texture* texture;
    SDL_Rect srcRect, destRect;
    int frameWidth;
    int frameHeight;
    int totalFrames;
    int currentFrame;
    Uint32 lastFrameTime;
    bool flipHorizontal;
    
public:
    static const int FRAME_DELAY = 100; // milliseconds
    
    AnimatedSprite(SDL_Renderer* renderer, const std::string& path, 
                  int frameWidth, int frameHeight, int totalFrames);
    ~AnimatedSprite();
    
    void update();
    void render(SDL_Renderer* renderer, int x, int y, bool flipped = false);
    
    void setFrame(int frame);
    void nextFrame() { currentFrame = (currentFrame + 1) % totalFrames; srcRect.x = currentFrame * frameWidth; }
    void setX(int x) { destRect.x = x; }
    void setY(int y) { destRect.y = y; }
    
    int getX() const { return destRect.x; }
    int getY() const { return destRect.y; }
    int getWidth() const { return destRect.w; }
    int getHeight() const { return destRect.h; }
    int getCurrentFrame() const { return currentFrame; }
    int getTotalFrames() const { return totalFrames; }
};

#endif // ANIMATED_SPRITE_H