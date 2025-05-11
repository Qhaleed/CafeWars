#ifndef ACTION_QUEUE_H
#define ACTION_QUEUE_H

#include <SFML/Graphics.hpp>

enum class ActionType {
    ATTACK,
    HEAL,
    BUFF,
    DEBUFF,
    DRAW_CARD
};

struct Action {
    ActionType type;
    int sourcePlayer;
    int sourcePosition;
    int targetPlayer;
    int targetPosition;
    int value;
    Action* next;
    
    Action(ActionType type, int sourcePlayer, int sourcePosition, int targetPlayer, int targetPosition, int value)
        : type(type), sourcePlayer(sourcePlayer), sourcePosition(sourcePosition), 
          targetPlayer(targetPlayer), targetPosition(targetPosition), value(value), next(nullptr) {}
};

class ActionQueue {
private:
    Action* front;
    Action* rear;
    
    // Animation properties
    float animationDuration;
    float currentTime;
    bool isAnimating;
    sf::Sprite animationSprite;
    sf::Texture attackTexture;
    sf::Texture healTexture;
    sf::Vector2f startPosition;
    sf::Vector2f endPosition;
    
public:
    ActionQueue();
    ~ActionQueue();
    
    bool isEmpty() const { return front == nullptr; }
    
    void enqueue(ActionType type, int sourcePlayer, int sourcePosition, int targetPlayer, int targetPosition, int value);
    Action* dequeue();
    Action* peek() const;
    
    void update(float deltaTime, class Board& board);
    void render(sf::RenderWindow& window);
    
    void startAnimation(const Action* action, const sf::Vector2f& start, const sf::Vector2f& end);
    bool isAnimationPlaying() const { return isAnimating; }
};

#endif // ACTION_QUEUE_H