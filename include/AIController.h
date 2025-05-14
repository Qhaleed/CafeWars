#ifndef AI_CONTROLLER_H
#define AI_CONTROLLER_H

#include <SDL2/SDL.h>
#include "Character.h"

// Forward declaration of Character to avoid circular dependency
class Character;

enum AIState {
    AI_IDLE,
    AI_PATROL,
    AI_CHASE,
    AI_ATTACK,
    AI_FLEE
};

class AIController {
private:
    Character* character;
    Character* playerCharacter;
    AIState currentState;
    bool isActiveCombatant;  // Flag indicating if this AI is currently engaged in combat
    
    // AI behavior parameters
    int detectionRange;
    int attackRange;
    int patrolSpeed;
    int chaseSpeed;
    
    // Patrol parameters
    int patrolLeftBound;
    int patrolRightBound;
    int patrolDirection;  // 1 for right, -1 for left
    
    // AI decision timers
    Uint32 lastDecisionTime;
    Uint32 decisionDelay;
    Uint32 lastAttackTime;
    Uint32 attackCooldown;  // milliseconds between attacks
    
    // Combat parameters
    int attackType;  // 1-4, corresponding to attack animations
    
    // Internal decision-making methods
    void makeDecision();
    void executeState(Uint32 currentTime);
    
public:
    AIController(Character* controlledCharacter, Character* player);
    ~AIController();
    
    void update(Uint32 currentTime);
    void setPatrolBounds(int leftBound, int rightBound);
    
    // Behavior configuration
    void setDetectionRange(int range) { detectionRange = range; }
    void setAttackRange(int range) { attackRange = range; }
    void setPatrolSpeed(int speed) { patrolSpeed = speed; }
    void setChaseSpeed(int speed) { chaseSpeed = speed; }
    void setAttackCooldown(Uint32 cooldown) { attackCooldown = cooldown; }
    
    // Combat engagement control
    void setActiveCombatant(bool active) { isActiveCombatant = active; }
    bool getIsActiveCombatant() const { return isActiveCombatant; }
    
    // State accessors
    AIState getState() const { return currentState; }
    bool isEngaged() const { return currentState == AI_CHASE || currentState == AI_ATTACK; }
    bool isDead() const { return character->getState() == DEAD; }
    
    Character* getCharacter() const { return character; }
    int getDistanceToPlayer() const;
};

#endif // AI_CONTROLLER_H