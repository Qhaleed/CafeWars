#include "../include/AIController.h"
#include "../include/Character.h"
#include <cmath>
#include <iostream>
#include <cstdlib>

AIController::AIController(Character* controlledCharacter, Character* player)
    : character(controlledCharacter), 
      playerCharacter(player),
      currentState(AI_IDLE),
      isActiveCombatant(false),
      detectionRange(300),
      attackRange(80),
      patrolSpeed(2),
      chaseSpeed(4),
      patrolLeftBound(100),
      patrolRightBound(700),
      patrolDirection(1),
      lastDecisionTime(SDL_GetTicks()),
      decisionDelay(1000), // Make decisions every 1 second
      lastAttackTime(0),
      attackCooldown(1500), // 1.5 seconds between attacks
      attackType(1)
{
    // Initialize with default patrol boundaries
}

AIController::~AIController() {
    // No dynamic allocations to clean up
}

void AIController::update(Uint32 currentTime) {
    // Skip if character is dead
    if (character->getState() == DEAD) {
        return;
    }
    
    // Make decisions based on timers
    if (currentTime - lastDecisionTime > decisionDelay) {
        makeDecision();
        lastDecisionTime = currentTime;
    }
    
    // Execute the current state behavior
    executeState(currentTime);
}

void AIController::setPatrolBounds(int leftBound, int rightBound) {
    patrolLeftBound = leftBound;
    patrolRightBound = rightBound;
}

int AIController::getDistanceToPlayer() const {
    if (!character || !playerCharacter) return 999999;
    return abs(character->getX() - playerCharacter->getX());
}

void AIController::makeDecision() {
    // Skip decision if character is dead or hurt
    if (character->getState() == DEAD || character->getState() == HURT) {
        return;
    }
    
    // Calculate distance to player
    int distanceToPlayer = getDistanceToPlayer();
    
    // Make decisions based on whether we're the active combatant
    if (isActiveCombatant) {
        // When active, focus on attacking if in range or chasing if not
        if (distanceToPlayer <= attackRange) {
            currentState = AI_ATTACK;
        } else {
            currentState = AI_CHASE;
        }
    } else {
        // When not the active combatant, just patrol or idle
        if (currentState == AI_CHASE || currentState == AI_ATTACK) {
            // If we were previously engaged, return to patrol
            currentState = AI_PATROL;
        } else if (currentState == AI_IDLE) {
            currentState = AI_PATROL;
        }
    }
}

void AIController::executeState(Uint32 currentTime) {
    // Skip execution if character is dead or hurt
    if (character->getState() == DEAD || character->getState() == HURT) {
        return;
    }
    
    int characterX = character->getX();
    int playerX = playerCharacter->getX();
    
    switch (currentState) {
        case AI_IDLE:
            // In idle state, just stand in place
            character->setCustomState(IDLE);
            break;
            
        case AI_PATROL:
            // Patrol back and forth between patrol boundaries
            if (patrolDirection > 0) {
                // Moving right
                character->setCustomState(WALKING);
                character->moveRight(patrolSpeed);
                
                if (characterX >= patrolRightBound) {
                    patrolDirection = -1; // Change direction
                }
            } else {
                // Moving left
                character->setCustomState(WALKING);
                character->moveLeft(patrolSpeed);
                
                if (characterX <= patrolLeftBound) {
                    patrolDirection = 1; // Change direction
                }
            }
            break;
            
        case AI_CHASE:
            // Chase the player character
            if (characterX < playerX) {
                character->setCustomState(RUNNING);
                character->moveRight(chaseSpeed);
            } else {
                character->setCustomState(RUNNING);
                character->moveLeft(chaseSpeed);
            }
            break;
            
        case AI_ATTACK:
            // Execute attack if in range and cooldown has passed
            if (character->isInAttackRange(playerCharacter) && character->canAttack() &&
                currentTime - lastAttackTime >= attackCooldown) {
                
                // Face the player
                if (characterX < playerX) {
                    character->setFacingRight(true);
                } else {
                    character->setFacingRight(false);
                }
                
                // Choose an attack randomly (1-4), with a bias toward attack 1
                int roll = rand() % 10;
                if (roll < 5) attackType = 1;      // 50% chance
                else if (roll < 7) attackType = 2; // 20% chance
                else if (roll < 9) attackType = 3; // 20% chance
                else attackType = 4;               // 10% chance
                
                // Perform the attack
                character->attack(attackType);
                lastAttackTime = currentTime;
            }
            else if (!character->isInAttackRange(playerCharacter)) {
                // If not in range, chase player
                currentState = AI_CHASE;
            }
            break;
            
        case AI_FLEE:
            // Run away from the player
            if (characterX < playerX) {
                character->setCustomState(RUNNING);
                character->moveLeft(chaseSpeed);
            } else {
                character->setCustomState(RUNNING);
                character->moveRight(chaseSpeed);
            }
            break;
    }
}