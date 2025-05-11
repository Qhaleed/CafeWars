#include "Board.h"
#include <iostream>
#include <fstream>

Board::Board() : currentPlayerIndex(0), gameEnded(false), 
                 selectedCard(nullptr), selectedSlotPlayer(-1), selectedSlotIndex(-1),
                 isAIEnabled(false) {
    // Initialize players
    players[0] = Player("Player 1");
    players[1] = Player("Player 2");
    
    // Initialize board slots to nullptr
    for (int p = 0; p < NUM_PLAYERS; p++) {
        for (int i = 0; i < BOARD_WIDTH; i++) {
            slots[p][i] = nullptr;
        }
    }
}

Board::~Board() {
    // Clean up any cards on the board
    for (int p = 0; p < NUM_PLAYERS; p++) {
        for (int i = 0; i < BOARD_WIDTH; i++) {
            if (slots[p][i] != nullptr) {
                delete slots[p][i];
                slots[p][i] = nullptr;
            }
        }
    }
}

void Board::initialize(bool enableAI) {
    isAIEnabled = enableAI;
    currentPlayerIndex = 0;
    gameEnded = false;
    
    // Clear the board
    for (int p = 0; p < NUM_PLAYERS; p++) {
        for (int i = 0; i < BOARD_WIDTH; i++) {
            if (slots[p][i] != nullptr) {
                delete slots[p][i];
                slots[p][i] = nullptr;
            }
        }
    }
    
    // Set up players
    for (int p = 0; p < NUM_PLAYERS; p++) {
        players[p].health = MAX_HEALTH;
        players[p].mana = 1;
        
        // Create and shuffle deck
        players[p].deck.createStandardDeck();
        
        // Draw initial hand
        for (int i = 0; i < 5; i++) {
            Card* card = players[p].deck.drawCard();
            if (card != nullptr) {
                players[p].hand.addCard(card);
            }
        }
    }
    
    // Start the first turn
    startTurn();
}

void Board::setupGraphics(sf::RenderWindow& window) {
    // Load font
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        std::cerr << "Error: Could not load font!" << std::endl;
    }
    
    // Set up player areas
    for (int p = 0; p < NUM_PLAYERS; p++) {
        playerAreas[p].setSize(sf::Vector2f(800.f, 250.f));
        playerAreas[p].setPosition(0.f, p * 300.f);
        playerAreas[p].setFillColor(p == 0 ? sf::Color(100, 100, 200, 100) : sf::Color(200, 100, 100, 100));
        playerAreas[p].setOutlineColor(sf::Color::White);
        playerAreas[p].setOutlineThickness(2.f);
        
        // Set up board slots
        for (int i = 0; i < BOARD_WIDTH; i++) {
            boardSlots[p][i].setSize(sf::Vector2f(80.f, 120.f));
            boardSlots[p][i].setPosition(100.f + i * 100.f, 100.f + p * 200.f);
            boardSlots[p][i].setFillColor(sf::Color(60, 120, 60, 150));
            boardSlots[p][i].setOutlineColor(sf::Color::White);
            boardSlots[p][i].setOutlineThickness(2.f);
            
            // Initialize text elements for cards on the board
            if (slots[p][i] != nullptr) {
                slots[p][i]->setupTextElements(font);
            }
        }
        
        // Set up player stats text
        statsText[p].setFont(font);
        statsText[p].setCharacterSize(18);
        statsText[p].setFillColor(sf::Color::White);
        statsText[p].setPosition(10.f, 10.f + p * 300.f);
        
        // Initialize text elements for cards in player's hand
        for (int i = 0; i < players[p].hand.getCount(); i++) {
            Card* card = players[p].hand.getCard(i);
            if (card != nullptr) {
                card->setupTextElements(font);
            }
        }
    }
    
    // Set up game over text
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(48);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setString("Game Over");
    gameOverText.setPosition(300.f, 250.f);
}

void Board::setSlot(int playerIndex, int slotIndex, Card* card) {
    // Delete any existing card in this slot
    if (slots[playerIndex][slotIndex] != nullptr) {
        delete slots[playerIndex][slotIndex];
    }
    
    // Set the new card
    slots[playerIndex][slotIndex] = card;
    
    // Update the card's position if it exists
    if (card != nullptr) {
        sf::Vector2f slotPos = boardSlots[playerIndex][slotIndex].getPosition();
        card->setPosition(slotPos.x + 10.f, slotPos.y + 10.f);
    }
}

bool Board::canPlayCard(Card* card, int playerIndex) const {
    if (card == nullptr || playerIndex != currentPlayerIndex) {
        return false;
    }
    
    // Check if the player has enough mana
    return players[playerIndex].mana >= card->getManaCost();
}

bool Board::playCard(Card* card, int playerIndex, int slotIndex) {
    if (!canPlayCard(card, playerIndex)) {
        return false;
    }
    
    // Make sure the slot is empty if it's a creature or building
    if ((card->getType() == CardType::CREATURE || card->getType() == CardType::BUILDING) && 
        slots[playerIndex][slotIndex] != nullptr) {
        return false;
    }
    
    // Deduct the mana cost
    players[playerIndex].mana -= card->getManaCost();
    
    // Play the card (effect will be handled by the card's play method)
    card->play(*this, playerIndex, slotIndex);
    
    return true;
}

void Board::startTurn() {
    // Regenerate mana
    players[currentPlayerIndex].mana += players[currentPlayerIndex].manaRegeneration;
    if (players[currentPlayerIndex].mana > players[currentPlayerIndex].maxMana) {
        players[currentPlayerIndex].mana = players[currentPlayerIndex].maxMana;
    }
    
    // Draw a card at the start of the turn
    Card* drawnCard = players[currentPlayerIndex].deck.drawCard();
    if (drawnCard != nullptr) {
        if (!players[currentPlayerIndex].hand.addCard(drawnCard)) {
            // Hand is full, discard the card
            delete drawnCard;
        }
    }
    
    // Reset creature attack flags for current player
    for (int i = 0; i < BOARD_WIDTH; i++) {
        Card* card = slots[currentPlayerIndex][i];
        if (card != nullptr && card->getType() == CardType::CREATURE) {
            CreatureCard* creature = static_cast<CreatureCard*>(card);
            creature->setCanAttack(true);
        }
    }
    
    // If it's the AI's turn, perform AI actions
    if (isAIEnabled && currentPlayerIndex == 1) {
        performAITurn();
    }
}

void Board::endTurn() {
    // Process end-of-turn effects for buildings
    for (int i = 0; i < BOARD_WIDTH; i++) {
        Card* card = slots[currentPlayerIndex][i];
        if (card != nullptr && card->getType() == CardType::BUILDING) {
            BuildingCard* building = static_cast<BuildingCard*>(card);
            building->reduceDurability();
            
            // If the building is destroyed, remove it
            if (building->getDurability() <= 0) {
                // Undo any effects (like mana regen boost)
                players[currentPlayerIndex].manaRegeneration -= building->getBoost();
                
                // Remove the building
                setSlot(currentPlayerIndex, i, nullptr);
            }
        }
    }
    
    // Switch to the next player
    currentPlayerIndex = (currentPlayerIndex + 1) % NUM_PLAYERS;
    
    // Start the next player's turn
    startTurn();
}

void Board::handleCardClick(Card* card, int mouseX, int mouseY) {
    // If the game is over, ignore clicks
    if (gameEnded) return;
    
    // Check which player's hand this card is from
    int cardPlayerIndex = -1;
    int cardIndex = -1;
    
    for (int p = 0; p < NUM_PLAYERS; p++) {
        for (int i = 0; i < players[p].hand.getCount(); i++) {
            if (players[p].hand.getCard(i) == card) {
                cardPlayerIndex = p;
                cardIndex = i;
                break;
            }
        }
        if (cardPlayerIndex != -1) break;
    }
    
    // If the card is in the current player's hand, select it
    if (cardPlayerIndex == currentPlayerIndex) {
        selectedCard = card;
        selectedSlotPlayer = -1;
        selectedSlotIndex = -1;
    }
}

void Board::handleBoardClick(int mouseX, int mouseY) {
    // If the game is over, ignore clicks
    if (gameEnded) return;
    
    // Check if a card in hand was clicked
    for (int p = 0; p < NUM_PLAYERS; p++) {
        Card* clickedCard = players[p].hand.getCardAt(mouseX, mouseY);
        if (clickedCard != nullptr) {
            handleCardClick(clickedCard, mouseX, mouseY);
            return;
        }
    }
    
    // Check if a board slot was clicked
    for (int p = 0; p < NUM_PLAYERS; p++) {
        for (int i = 0; i < BOARD_WIDTH; i++) {
            if (boardSlots[p][i].getGlobalBounds().contains(mouseX, mouseY)) {
                // If we have a card selected, try to play it in this slot
                if (selectedCard != nullptr && selectedSlotPlayer == -1) {
                    // Find the card in the player's hand
                    int cardIndex = -1;
                    for (int j = 0; j < players[currentPlayerIndex].hand.getCount(); j++) {
                        if (players[currentPlayerIndex].hand.getCard(j) == selectedCard) {
                            cardIndex = j;
                            break;
                        }
                    }
                    
                    if (cardIndex != -1) {
                        // Remove the card from hand
                        Card* playedCard = players[currentPlayerIndex].hand.removeCard(cardIndex);
                        
                        // Try to play the card
                        if (!playCard(playedCard, currentPlayerIndex, i)) {
                            // If playing the card failed, return it to the hand
                            players[currentPlayerIndex].hand.addCard(playedCard);
                        }
                    }
                    
                    selectedCard = nullptr;
                }
                // If we have a creature selected on the board, try to attack
                else if (selectedSlotPlayer != -1 && selectedSlotIndex != -1) {
                    Card* attackerCard = slots[selectedSlotPlayer][selectedSlotIndex];
                    
                    // Make sure it's a creature and can attack
                    if (attackerCard != nullptr && 
                        attackerCard->getType() == CardType::CREATURE && 
                        selectedSlotPlayer == currentPlayerIndex) {
                        
                        CreatureCard* attacker = static_cast<CreatureCard*>(attackerCard);
                        
                        if (attacker->canAttack()) {
                            // Queue an attack action
                            actionQueue.enqueue(ActionType::ATTACK, 
                                               selectedSlotPlayer, selectedSlotIndex, 
                                               p, i, attacker->getAttack());
                            
                            // Mark the creature as having attacked this turn
                            attacker->setCanAttack(false);
                        }
                    }
                    
                    selectedSlotPlayer = -1;
                    selectedSlotIndex = -1;
                }
                // Otherwise, select this slot
                else {
                    selectedCard = nullptr;
                    selectedSlotPlayer = p;
                    selectedSlotIndex = i;
                }
                
                return;
            }
        }
    }
    
    // If we clicked somewhere else, clear the selection
    selectedCard = nullptr;
    selectedSlotPlayer = -1;
    selectedSlotIndex = -1;
}

void Board::performAITurn() {
    // Simple AI implementation:
    // 1. Play as many cards as possible from the hand
    // 2. Attack with all creatures
    
    // Try to play cards
    for (int i = 0; i < players[1].hand.getCount(); i++) {
        Card* card = players[1].hand.getCard(i);
        
        if (players[1].mana >= card->getManaCost()) {
            // Find an empty slot for creatures and buildings
            if (card->getType() == CardType::CREATURE || card->getType() == CardType::BUILDING) {
                for (int j = 0; j < BOARD_WIDTH; j++) {
                    if (slots[1][j] == nullptr) {
                        // Remove card from hand
                        players[1].hand.removeCard(i);
                        
                        // Play the card
                        playCard(card, 1, j);
                        
                        // Adjust index since we removed a card
                        i--;
                        break;
                    }
                }
            }
            // For spells, target the opponent's creatures or the opponent directly
            else if (card->getType() == CardType::SPELL) {
                bool spellPlayed = false;
                
                // Try to target an enemy creature
                for (int j = 0; j < BOARD_WIDTH; j++) {
                    if (slots[0][j] != nullptr && slots[0][j]->getType() == CardType::CREATURE) {
                        // Remove card from hand
                        players[1].hand.removeCard(i);
                        
                        // Play the spell
                        SpellCard* spell = static_cast<SpellCard*>(card);
                        players[1].mana -= spell->getManaCost();
                        
                        // Attack the enemy creature
                        CreatureCard* target = static_cast<CreatureCard*>(slots[0][j]);
                        target->takeDamage(spell->getEffectValue());
                        
                        // If the creature died, remove it
                        if (target->getHealth() <= 0) {
                            setSlot(0, j, nullptr);
                        }
                        
                        delete spell; // Spell is used up
                        
                        // Adjust index since we removed a card
                        i--;
                        spellPlayed = true;
                        break;
                    }
                }
                
                // If no creature to target, attack the opponent directly
                if (!spellPlayed) {
                    // Remove card from hand
                    players[1].hand.removeCard(i);
                    
                    // Play the spell
                    SpellCard* spell = static_cast<SpellCard*>(card);
                    players[1].mana -= spell->getManaCost();
                    
                    // Damage the opponent
                    players[0].health -= spell->getEffectValue();
                    if (players[0].health < 0) players[0].health = 0;
                    
                    delete spell; // Spell is used up
                    
                    // Adjust index since we removed a card
                    i--;
                }
            }
        }
    }
    
    // Attack with all creatures
    for (int i = 0; i < BOARD_WIDTH; i++) {
        Card* card = slots[1][i];
        if (card != nullptr && card->getType() == CardType::CREATURE) {
            CreatureCard* attacker = static_cast<CreatureCard*>(card);
            
            if (attacker->canAttack()) {
                // Try to attack an enemy creature
                bool attacked = false;
                for (int j = 0; j < BOARD_WIDTH; j++) {
                    if (slots[0][j] != nullptr && slots[0][j]->getType() == CardType::CREATURE) {
                        // Queue an attack action
                        actionQueue.enqueue(ActionType::ATTACK, 
                                           1, i, 
                                           0, j, attacker->getAttack());
                        
                        // Mark the creature as having attacked this turn
                        attacker->setCanAttack(false);
                        attacked = true;
                        break;
                    }
                }
                
                // If no creature to attack, attack the opponent directly
                if (!attacked) {
                    // Damage the opponent
                    players[0].health -= attacker->getAttack();
                    if (players[0].health < 0) players[0].health = 0;
                    
                    // Mark the creature as having attacked this turn
                    attacker->setCanAttack(false);
                }
            }
        }
    }
    
    // End the turn after a short delay
    // In a real implementation, you would use a timer
    // For this example, we'll just end it immediately
    endTurn();
}

void Board::update(float deltaTime) {
    // Update action queue
    actionQueue.update(deltaTime, *this);
    
    // Check for game over conditions
    for (int p = 0; p < NUM_PLAYERS; p++) {
        if (players[p].health <= 0) {
            gameEnded = true;
            gameOverText.setString("Game Over\n" + players[(p + 1) % 2].name + " Wins!");
            break;
        }
    }
}

void Board::render(sf::RenderWindow& window) {
    // Draw player areas
    for (int p = 0; p < NUM_PLAYERS; p++) {
        window.draw(playerAreas[p]);
    }
    
    // Draw board slots
    for (int p = 0; p < NUM_PLAYERS; p++) {
        for (int i = 0; i < BOARD_WIDTH; i++) {
            // Highlight selected slot
            if (p == selectedSlotPlayer && i == selectedSlotIndex) {
                boardSlots[p][i].setOutlineColor(sf::Color::Yellow);
                boardSlots[p][i].setOutlineThickness(3.f);
            } else {
                boardSlots[p][i].setOutlineColor(sf::Color::White);
                boardSlots[p][i].setOutlineThickness(2.f);
            }
            
            window.draw(boardSlots[p][i]);
            
            // Draw cards in slots
            if (slots[p][i] != nullptr) {
                slots[p][i]->render(window);
            }
        }
    }
    
    // Draw player hands
    float handStartY = 220.f;
    for (int p = 0; p < NUM_PLAYERS; p++) {
        float startX = 100.f;
        float startY = p == 0 ? handStartY + 300.f : handStartY - 150.f;
        players[p].hand.render(window, startX, startY, 100.f);
    }
    
    // Update and draw player stats
    for (int p = 0; p < NUM_PLAYERS; p++) {
        statsText[p].setString(
            players[p].name + "\n" +
            "Health: " + std::to_string(players[p].health) + "/" + std::to_string(players[p].maxHealth) + "\n" +
            "Mana: " + std::to_string(players[p].mana) + "/" + std::to_string(players[p].maxMana) + " (+" + std::to_string(players[p].manaRegeneration) + ")"
        );
        
        // Highlight the current player
        if (p == currentPlayerIndex && !gameEnded) {
            statsText[p].setOutlineColor(sf::Color::Yellow);
            statsText[p].setOutlineThickness(1.f);
        } else {
            statsText[p].setOutlineThickness(0.f);
        }
        
        window.draw(statsText[p]);
    }
    
    // Draw action animations
    actionQueue.render(window);
    
    // Draw game over message if the game has ended
    if (gameEnded) {
        window.draw(gameOverText);
    }
}

bool Board::saveGame(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Save the current player index
    file << currentPlayerIndex << std::endl;
    
    // Save player data
    for (int p = 0; p < NUM_PLAYERS; p++) {
        file << players[p].name << std::endl;
        file << players[p].health << " " << players[p].maxHealth << std::endl;
        file << players[p].mana << " " << players[p].maxMana << " " << players[p].manaRegeneration << std::endl;
        
        // Save the deck
        players[p].deck.saveToFile(filename + ".deck" + std::to_string(p));
        
        // Save the hand
        file << players[p].hand.getCount() << std::endl;
        for (int i = 0; i < players[p].hand.getCount(); i++) {
            Card* card = players[p].hand.getCard(i);
            
            // Save card data: type, name, description, manaCost, and extra values
            file << static_cast<int>(card->getType()) << std::endl;
            file << card->getName() << std::endl;
            file << card->getDescription() << std::endl;
            file << card->getManaCost() << std::endl;
            
            if (card->getType() == CardType::CREATURE) {
                CreatureCard* creature = static_cast<CreatureCard*>(card);
                file << creature->getAttack() << " " << creature->getHealth() << " " << creature->getMaxHealth() << " " << (creature->canAttack() ? 1 : 0) << std::endl;
            } else if (card->getType() == CardType::SPELL) {
                SpellCard* spell = static_cast<SpellCard*>(card);
                file << spell->getEffectValue() << std::endl;
            } else if (card->getType() == CardType::BUILDING) {
                BuildingCard* building = static_cast<BuildingCard*>(card);
                file << building->getDurability() << " " << building->getBoost() << std::endl;
            }
        }
        
        // Save the board slots
        for (int i = 0; i < BOARD_WIDTH; i++) {
            if (slots[p][i] == nullptr) {
                file << "empty" << std::endl;
            } else {
                Card* card = slots[p][i];
                
                file << static_cast<int>(card->getType()) << std::endl;
                file << card->getName() << std::endl;
                file << card->getDescription() << std::endl;
                file << card->getManaCost() << std::endl;
                
                if (card->getType() == CardType::CREATURE) {
                    CreatureCard* creature = static_cast<CreatureCard*>(card);
                    file << creature->getAttack() << " " << creature->getHealth() << " " << creature->getMaxHealth() << " " << (creature->canAttack() ? 1 : 0) << std::endl;
                } else if (card->getType() == CardType::SPELL) {
                    SpellCard* spell = static_cast<SpellCard*>(card);
                    file << spell->getEffectValue() << std::endl;
                } else if (card->getType() == CardType::BUILDING) {
                    BuildingCard* building = static_cast<BuildingCard*>(card);
                    file << building->getDurability() << " " << building->getBoost() << std::endl;
                }
            }
        }
    }
    
    file.close();
    return true;
}

bool Board::loadGame(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Clean up any existing cards
    for (int p = 0; p < NUM_PLAYERS; p++) {
        for (int i = 0; i < BOARD_WIDTH; i++) {
            if (slots[p][i] != nullptr) {
                delete slots[p][i];
                slots[p][i] = nullptr;
            }
        }
    }
    
    // Load the current player index
    file >> currentPlayerIndex;
    file.ignore(); // Skip newline
    
    // Load player data
    for (int p = 0; p < NUM_PLAYERS; p++) {
        std::getline(file, players[p].name);
        file >> players[p].health >> players[p].maxHealth;
        file >> players[p].mana >> players[p].maxMana >> players[p].manaRegeneration;
        file.ignore(); // Skip newline
        
        // Load the deck
        players[p].deck.loadFromFile(filename + ".deck" + std::to_string(p));
        
        // Load the hand
        int handCount;
        file >> handCount;
        file.ignore(); // Skip newline
        
        for (int i = 0; i < handCount; i++) {
            int cardType;
            std::string name, description;
            int manaCost;
            
            file >> cardType;
            file.ignore(); // Skip newline
            std::getline(file, name);
            std::getline(file, description);
            file >> manaCost;
            
            Card* newCard = nullptr;
            
            if (static_cast<CardType>(cardType) == CardType::CREATURE) {
                int attack, health, maxHealth, canAttack;
                file >> attack >> health >> maxHealth >> canAttack;
                
                CreatureCard* creature = new CreatureCard(name, description, manaCost, attack, health);
                creature->setHealth(health);
                creature->setCanAttack(canAttack == 1);
                newCard = creature;
            } else if (static_cast<CardType>(cardType) == CardType::SPELL) {
                int effectValue;
                file >> effectValue;
                
                newCard = new SpellCard(name, description, manaCost, effectValue);
            } else if (static_cast<CardType>(cardType) == CardType::BUILDING) {
                int durability, boost;
                file >> durability >> boost;
                
                newCard = new BuildingCard(name, description, manaCost, durability, boost);
            }
            
            if (newCard != nullptr) {
                players[p].hand.addCard(newCard);
            }
            
            file.ignore(); // Skip newline
        }
        
        // Load the board slots
        for (int i = 0; i < BOARD_WIDTH; i++) {
            std::string slotType;
            std::getline(file, slotType);
            
            if (slotType != "empty") {
                int cardType = std::stoi(slotType);
                std::string name, description;
                int manaCost;
                
                std::getline(file, name);
                std::getline(file, description);
                file >> manaCost;
                
                Card* newCard = nullptr;
                
                if (static_cast<CardType>(cardType) == CardType::CREATURE) {
                    int attack, health, maxHealth, canAttack;
                    file >> attack >> health >> maxHealth >> canAttack;
                    
                    CreatureCard* creature = new CreatureCard(name, description, manaCost, attack, health);
                    creature->setHealth(health);
                    creature->setCanAttack(canAttack == 1);
                    newCard = creature;
                } else if (static_cast<CardType>(cardType) == CardType::SPELL) {
                    int effectValue;
                    file >> effectValue;
                    
                    newCard = new SpellCard(name, description, manaCost, effectValue);
                } else if (static_cast<CardType>(cardType) == CardType::BUILDING) {
                    int durability, boost;
                    file >> durability >> boost;
                    
                    newCard = new BuildingCard(name, description, manaCost, durability, boost);
                }
                
                if (newCard != nullptr) {
                    setSlot(p, i, newCard);
                }
                
                file.ignore(); // Skip newline
            }
        }
    }
    
    file.close();
    gameEnded = false;
    return true;
}