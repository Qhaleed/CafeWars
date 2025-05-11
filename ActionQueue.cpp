#include "ActionQueue.h"
#include "Board.h"
#include <iostream>

ActionQueue::ActionQueue() 
    : front(nullptr), rear(nullptr), 
      animationDuration(0.5f), currentTime(0.0f), isAnimating(false) {
    // Load animation textures
    // In a real implementation, you'd load actual texture files
    // attackTexture.loadFromFile("attack_animation.png");
    // healTexture.loadFromFile("heal_animation.png");
}

ActionQueue::~ActionQueue() {
    // Delete all actions in the queue
    while (front != nullptr) {
        Action* temp = front;
        front = front->next;
        delete temp;
    }
    front = nullptr;
    rear = nullptr;
}

void ActionQueue::enqueue(ActionType type, int sourcePlayer, int sourcePosition, 
                         int targetPlayer, int targetPosition, int value) {
    Action* newAction = new Action(type, sourcePlayer, sourcePosition, 
                                  targetPlayer, targetPosition, value);
    
    if (isEmpty()) {
        front = newAction;
        rear = newAction;
    } else {
        rear->next = newAction;
        rear = newAction;
    }
}

Action* ActionQueue::dequeue() {
    if (isEmpty()) {
        return nullptr;
    }
    
    Action* dequeuedAction = front;
    front = front->next;
    
    // If we just removed the last element, update rear as well
    if (front == nullptr) {
        rear = nullptr;
    }
    
    dequeuedAction->next = nullptr;
    return dequeuedAction;
}

Action* ActionQueue::peek() const {
    return front;
}

void ActionQueue::update(float deltaTime, Board& board) {
    // If we're currently animating an action
    if (isAnimating) {
        currentTime += deltaTime;
        
        // Check if the animation is complete
        if (currentTime >= animationDuration) {
            isAnimating = false;
            
            // Process the completed action
            Action* action = dequeue();
            if (action != nullptr) {
                // Apply the action's effect based on its type
                switch (action->type) {
                    case ActionType::ATTACK: {
                        Card* targetCard = board.getSlot(action->targetPlayer, action->targetPosition);
                        if (targetCard && targetCard->getType() == CardType::CREATURE) {
                            CreatureCard* creature = static_cast<CreatureCard*>(targetCard);
                            creature->takeDamage(action->value);
                            
                            // If the creature died, remove it from the board
                            if (creature->getHealth() <= 0) {
                                board.setSlot(action->targetPlayer, action->targetPosition, nullptr);
                                delete creature;
                            }
                        } else {
                            // If no card at target position, damage the player directly
                            Player& player = board.getPlayer(action->targetPlayer);
                            player.health -= action->value;
                            if (player.health < 0) player.health = 0;
                        }
                        break;
                    }
                    case ActionType::HEAL: {
                        Card* targetCard = board.getSlot(action->targetPlayer, action->targetPosition);
                        if (targetCard && targetCard->getType() == CardType::CREATURE) {
                            CreatureCard* creature = static_cast<CreatureCard*>(targetCard);
                            int newHealth = creature->getHealth() + action->value;
                            if (newHealth > creature->getMaxHealth()) {
                                newHealth = creature->getMaxHealth();
                            }
                            creature->setHealth(newHealth);
                        } else {
                            // If no card at target position, heal the player directly
                            Player& player = board.getPlayer(action->targetPlayer);
                            player.health += action->value;
                            if (player.health > player.maxHealth) player.health = player.maxHealth;
                        }
                        break;
                    }
                    case ActionType::BUFF: {
                        Card* targetCard = board.getSlot(action->targetPlayer, action->targetPosition);
                        if (targetCard && targetCard->getType() == CardType::CREATURE) {
                            CreatureCard* creature = static_cast<CreatureCard*>(targetCard);
                            creature->setAttack(creature->getAttack() + action->value);
                        }
                        break;
                    }
                    case ActionType::DEBUFF: {
                        Card* targetCard = board.getSlot(action->targetPlayer, action->targetPosition);
                        if (targetCard && targetCard->getType() == CardType::CREATURE) {
                            CreatureCard* creature = static_cast<CreatureCard*>(targetCard);
                            int newAttack = creature->getAttack() - action->value;
                            if (newAttack < 0) newAttack = 0;
                            creature->setAttack(newAttack);
                        }
                        break;
                    }
                    case ActionType::DRAW_CARD: {
                        Player& player = board.getPlayer(action->targetPlayer);
                        Card* drawnCard = player.deck.drawCard();
                        if (drawnCard) {
                            if (!player.hand.addCard(drawnCard)) {
                                // Hand is full, discard the card
                                delete drawnCard;
                            }
                        }
                        break;
                    }
                }
                
                delete action;
            }
        }
    }
    // If we're not animating and there are actions to process
    else if (!isEmpty()) {
        Action* nextAction = peek();
        
        // Get positions for the animation
        sf::Vector2f sourcePos, targetPos;
        
        // In a real implementation, you would get the actual positions 
        // based on the board layout
        sourcePos = sf::Vector2f(100.f + nextAction->sourcePosition * 100.f, 
                               100.f + nextAction->sourcePlayer * 200.f);
        targetPos = sf::Vector2f(100.f + nextAction->targetPosition * 100.f, 
                               100.f + nextAction->targetPlayer * 200.f);
        
        startAnimation(nextAction, sourcePos, targetPos);
    }
}

void ActionQueue::startAnimation(const Action* action, const sf::Vector2f& start, const sf::Vector2f& end) {
    isAnimating = true;
    currentTime = 0.0f;
    startPosition = start;
    endPosition = end;
    
    // Set the animation sprite based on the action type
    switch (action->type) {
        case ActionType::ATTACK:
            animationSprite.setTexture(attackTexture);
            break;
        case ActionType::HEAL:
            animationSprite.setTexture(healTexture);
            break;
        default:
            // For other actions, we could use different textures or just a generic one
            animationSprite.setTexture(attackTexture);
            break;
    }
    
    // Position the sprite at the start position
    animationSprite.setPosition(startPosition);
}

void ActionQueue::render(sf::RenderWindow& window) {
    if (isAnimating) {
        // Calculate the current position based on linear interpolation
        float t = currentTime / animationDuration;
        if (t > 1.0f) t = 1.0f;
        
        sf::Vector2f currentPos = startPosition + (endPosition - startPosition) * t;
        animationSprite.setPosition(currentPos);
        
        // Draw the animation sprite
        window.draw(animationSprite);
    }
}