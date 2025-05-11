#ifndef DECK_H
#define DECK_H

#include "Card.h"
#include <cstdlib>
#include <ctime>

class Hand {
private:
    Card* head;
    int count;
    int maxCards;
    
public:
    Hand(int maxCards = 5);
    ~Hand();
    
    bool isEmpty() const { return head == nullptr; }
    int getCount() const { return count; }
    int getMaxCards() const { return maxCards; }
    
    bool addCard(Card* card);
    Card* removeCard(int index);
    Card* getCard(int index) const;
    
    void render(sf::RenderWindow& window, float startX, float startY, float spacing);
    Card* getCardAt(float x, float y) const;
};

class Deck {
private:
    Card* head;
    int count;
    
public:
    Deck();
    ~Deck();
    
    bool isEmpty() const { return head == nullptr; }
    int getCount() const { return count; }
    
    void addCard(Card* card);
    Card* drawCard();
    void shuffle();
    
    // Create a standard 20-card cafe-themed deck
    void createStandardDeck();
    
    // Save/load deck to file
    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);
};

#endif // DECK_H