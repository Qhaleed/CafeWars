#ifndef BOARD_H
#define BOARD_H

#include "Card.h"
#include "Deck.h"
#include "ActionQueue.h"
#include <SFML/Graphics.hpp>

const int NUM_PLAYERS = 2;
const int BOARD_WIDTH = 4;
const int SLOTS_PER_PLAYER = BOARD_WIDTH;
const int MAX_HEALTH = 20;
const int BASE_MANA_REGEN = 3;

struct Player {
    std::string name;
    int health;
    int maxHealth;
    int mana;
    int maxMana;
    int manaRegeneration;
    Deck deck;
    Hand hand;
    
    Player() : name("Player"), health(MAX_HEALTH), maxHealth(MAX_HEALTH), 
               mana(1), maxMana(10), manaRegeneration(BASE_MANA_REGEN) {}

    Player(const std::string& playerName) 
        : name(playerName), health(MAX_HEALTH), maxHealth(MAX_HEALTH), 
          mana(1), maxMana(10), manaRegeneration(BASE_MANA_REGEN) {}
};

class Board {
private:
    Card* slots[NUM_PLAYERS][BOARD_WIDTH];
    Player players[NUM_PLAYERS];
    ActionQueue actionQueue;
    int currentPlayerIndex;
    bool gameEnded;
    
    // SFML elements
    sf::Font font;
    sf::RectangleShape boardSlots[NUM_PLAYERS][BOARD_WIDTH];
    sf::RectangleShape playerAreas[NUM_PLAYERS];
    sf::Text statsText[NUM_PLAYERS];
    sf::Text gameOverText;
    
    // Selected card tracking
    Card* selectedCard;
    int selectedSlotPlayer;
    int selectedSlotIndex;
    
    // AI opponent (for single-player mode)
    bool isAIEnabled;
    
public:
    Board();
    ~Board();
    
    void initialize(bool enableAI = false);
    void setupGraphics(sf::RenderWindow& window);
    
    Player& getPlayer(int playerIndex) { return players[playerIndex]; }
    Card* getSlot(int playerIndex, int slotIndex) { return slots[playerIndex][slotIndex]; }
    void setSlot(int playerIndex, int slotIndex, Card* card);
    int getCurrentPlayerIndex() const { return currentPlayerIndex; }
    bool isGameOver() const { return gameEnded; }
    
    bool canPlayCard(Card* card, int playerIndex) const;
    bool playCard(Card* card, int playerIndex, int slotIndex);
    
    void startTurn();
    void endTurn();
    
    void handleCardClick(Card* card, int mouseX, int mouseY);
    void handleBoardClick(int mouseX, int mouseY);
    
    void performAITurn();
    
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    
    bool saveGame(const std::string& filename) const;
    bool loadGame(const std::string& filename);
};

#endif // BOARD_H