#include "Card.h"
#include "Board.h"
#include <iostream>

// Base Card class implementation
Card::Card(std::string name, std::string description, int manaCost, CardType type)
    : name(name), description(description), manaCost(manaCost), type(type), next(nullptr) {
    
    // Load texture from file
    static bool textureLoaded = false;
    static sf::Texture cardTexture;
    
    if (!textureLoaded) {
        if (!cardTexture.loadFromFile("card.png")) {
            std::cerr << "Error loading card texture!" << std::endl;
            // Create a default colored rectangle texture
            cardTexture.create(80, 120);
            sf::Image img;
            img.create(80, 120, sf::Color(60, 120, 60));
            cardTexture.update(img);
        }
        textureLoaded = true;
    }
    
    texture = cardTexture;
    sprite.setTexture(texture);
    
    // Calculate the scale to fit the card into the intended 80x120 size
    sf::Vector2u textureSize = texture.getSize();
    float scaleX = 80.0f / textureSize.x;
    float scaleY = 120.0f / textureSize.y;
    sprite.setScale(scaleX, scaleY);
}

void Card::setupTextElements(const sf::Font& font) {
    // Set up name text
    nameText.setFont(font);
    nameText.setCharacterSize(8);
    nameText.setFillColor(sf::Color::White);
    nameText.setString(name);
    
    // Set up cost text
    costText.setFont(font);
    costText.setCharacterSize(12);
    costText.setFillColor(sf::Color::Yellow);
    costText.setString(std::to_string(manaCost));
    
    // Set up description text
    descriptionText.setFont(font);
    descriptionText.setCharacterSize(6);
    descriptionText.setFillColor(sf::Color::White);
    descriptionText.setString(description);
}

void Card::setPosition(float x, float y) {
    sprite.setPosition(x, y);
    
    // Update text positions relative to card position
    nameText.setPosition(x + 5, y + 5);
    costText.setPosition(x + 70, y + 5);
    descriptionText.setPosition(x + 5, y + 70);
}

bool Card::contains(float x, float y) const {
    return sprite.getGlobalBounds().contains(x, y);
}

void Card::render(sf::RenderWindow& window) {
    window.draw(sprite);
    window.draw(nameText);
    window.draw(costText);
    window.draw(descriptionText);
}

// CreatureCard implementation
CreatureCard::CreatureCard(std::string name, std::string description, int manaCost, int attack, int health)
    : Card(name, description, manaCost, CardType::CREATURE), 
      attack(attack), health(health), maxHealth(health), canAttackThisTurn(false) {
}

void CreatureCard::takeDamage(int damage) {
    health -= damage;
    if (health < 0) health = 0;
}

void CreatureCard::render(sf::RenderWindow& window) {
    Card::render(window);
    
    // Add rendering for attack and health stats
    float x = sprite.getPosition().x;
    float y = sprite.getPosition().y;
    
    // Only attempt to render these stats if the font is available
    const sf::Font* fontPtr = nameText.getFont();
    if (fontPtr) {
        sf::Text attackText;
        attackText.setFont(*fontPtr);
        attackText.setCharacterSize(12);
        attackText.setFillColor(sf::Color::Red);
        attackText.setString(std::to_string(attack));
        attackText.setPosition(x + 5, y + 95);
        
        sf::Text healthText;
        healthText.setFont(*fontPtr);
        healthText.setCharacterSize(12);
        healthText.setFillColor(sf::Color::Green);
        healthText.setString(std::to_string(health) + "/" + std::to_string(maxHealth));
        healthText.setPosition(x + 40, y + 95);
        
        window.draw(attackText);
        window.draw(healthText);
    }
}

void CreatureCard::play(Board& board, int playerIndex, int position) {
    // Place the creature card on the board at the specified position
    board.setSlot(playerIndex, position, this);
    
    // Set the creature to not be able to attack this turn (summoning sickness)
    canAttackThisTurn = false;
}

Card* CreatureCard::clone() const {
    return new CreatureCard(name, description, manaCost, attack, health);
}

// SpellCard implementation
SpellCard::SpellCard(std::string name, std::string description, int manaCost, int effectValue)
    : Card(name, description, manaCost, CardType::SPELL), effectValue(effectValue) {
}

void SpellCard::render(sf::RenderWindow& window) {
    Card::render(window);
    
    // Add rendering for spell effect value
    float x = sprite.getPosition().x;
    float y = sprite.getPosition().y;
    
    // Only attempt to render these stats if the font is available
    const sf::Font* fontPtr = nameText.getFont();
    if (fontPtr) {
        sf::Text effectText;
        effectText.setFont(*fontPtr);
        effectText.setCharacterSize(12);
        effectText.setFillColor(sf::Color::Blue);
        effectText.setString("Effect: " + std::to_string(effectValue));
        effectText.setPosition(x + 5, y + 95);
        
        window.draw(effectText);
    }
}

void SpellCard::play(Board& board, int playerIndex, int position) {
    // Implementation depends on the specific spell effect
    // For example, a healing spell might do:
    // board.getPlayer(playerIndex).health += effectValue;
    
    // For this example, let's make a simple damage spell:
    int targetPlayerIndex = (playerIndex + 1) % 2; // Target the opponent
    
    // Check if there's a card at the target position
    Card* targetCard = board.getSlot(targetPlayerIndex, position);
    if (targetCard && targetCard->getType() == CardType::CREATURE) {
        CreatureCard* creature = static_cast<CreatureCard*>(targetCard);
        creature->takeDamage(effectValue);
        
        // If the creature died, remove it from the board
        if (creature->getHealth() <= 0) {
            board.setSlot(targetPlayerIndex, position, nullptr);
            delete creature;
        }
    } else {
        // If no card at target position, damage the opponent directly
        Player& opponent = board.getPlayer(targetPlayerIndex);
        opponent.health -= effectValue;
    }
    
    // Spells are one-time use, so they get destroyed after being played
    // The board's playCard method should handle deleting this card
}

Card* SpellCard::clone() const {
    return new SpellCard(name, description, manaCost, effectValue);
}

// BuildingCard implementation
BuildingCard::BuildingCard(std::string name, std::string description, int manaCost, int durability, int boost)
    : Card(name, description, manaCost, CardType::BUILDING), durability(durability), boost(boost) {
}

void BuildingCard::reduceDurability() {
    durability--;
}

void BuildingCard::render(sf::RenderWindow& window) {
    Card::render(window);
    
    // Add rendering for building durability and boost
    float x = sprite.getPosition().x;
    float y = sprite.getPosition().y;
    
    // Only attempt to render these stats if the font is available
    const sf::Font* fontPtr = nameText.getFont();
    if (fontPtr) {
        sf::Text durabilityText;
        durabilityText.setFont(*fontPtr);
        durabilityText.setCharacterSize(12);
        durabilityText.setFillColor(sf::Color::White);
        durabilityText.setString("Dur: " + std::to_string(durability));
        durabilityText.setPosition(x + 5, y + 95);
        
        sf::Text boostText;
        boostText.setFont(*fontPtr);
        boostText.setCharacterSize(12);
        boostText.setFillColor(sf::Color::Yellow);
        boostText.setString("+" + std::to_string(boost));
        boostText.setPosition(x + 60, y + 95);
        
        window.draw(durabilityText);
        window.draw(boostText);
    }
}

void BuildingCard::play(Board& board, int playerIndex, int position) {
    // Place the building card on the board at the specified position
    board.setSlot(playerIndex, position, this);
    
    // Apply passive boost effect (implementation would vary based on the building type)
    // For this example, let's boost the player's mana regeneration
    Player& player = board.getPlayer(playerIndex);
    player.manaRegeneration += boost;
}

Card* BuildingCard::clone() const {
    return new BuildingCard(name, description, manaCost, durability, boost);
}