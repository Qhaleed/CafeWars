#ifndef CARD_H
#define CARD_H

#include <SFML/Graphics.hpp>
#include <string>

enum class CardType {
    CREATURE,
    SPELL,
    BUILDING
};

class Card {
protected:
    std::string name;
    std::string description;
    int manaCost;
    sf::Texture texture;
    sf::Sprite sprite;
    CardType type;
    
    // Text elements for card details
    sf::Text nameText;
    sf::Text costText;
    sf::Text descriptionText;
    
public:
    Card* next; // For linked list implementation

    Card(std::string name, std::string description, int manaCost, CardType type);
    virtual ~Card() {}
    
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    int getManaCost() const { return manaCost; }
    CardType getType() const { return type; }
    sf::Sprite& getSprite() { return sprite; }
    
    void setPosition(float x, float y);
    bool contains(float x, float y) const;
    
    void setupTextElements(const sf::Font& font);
    virtual void render(sf::RenderWindow& window);
    virtual void play(class Board& board, int playerIndex, int position) = 0;
    virtual Card* clone() const = 0;
};

class CreatureCard : public Card {
private:
    int attack;
    int health;
    int maxHealth;
    bool canAttackThisTurn;
    
public:
    CreatureCard(std::string name, std::string description, int manaCost, int attack, int health);
    
    int getAttack() const { return attack; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    bool canAttack() const { return canAttackThisTurn; }
    
    void setAttack(int value) { attack = value; }
    void setHealth(int value) { health = value; }
    void setCanAttack(bool value) { canAttackThisTurn = value; }
    void takeDamage(int damage);
    
    virtual void render(sf::RenderWindow& window) override;
    virtual void play(Board& board, int playerIndex, int position) override;
    virtual Card* clone() const override;
};

class SpellCard : public Card {
private:
    int effectValue;
    
public:
    SpellCard(std::string name, std::string description, int manaCost, int effectValue);
    
    int getEffectValue() const { return effectValue; }
    
    virtual void render(sf::RenderWindow& window) override;
    virtual void play(Board& board, int playerIndex, int position) override;
    virtual Card* clone() const override;
};

class BuildingCard : public Card {
private:
    int durability;
    int boost;
    
public:
    BuildingCard(std::string name, std::string description, int manaCost, int durability, int boost);
    
    int getDurability() const { return durability; }
    int getBoost() const { return boost; }
    
    void reduceDurability();
    
    virtual void render(sf::RenderWindow& window) override;
    virtual void play(Board& board, int playerIndex, int position) override;
    virtual Card* clone() const override;
};

#endif // CARD_H