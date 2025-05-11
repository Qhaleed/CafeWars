#include "Deck.h"
#include <fstream>
#include <iostream>

// Hand implementation
Hand::Hand(int maxCards) : head(nullptr), count(0), maxCards(maxCards) {
}

Hand::~Hand() {
    // Delete all cards in the hand
    Card* current = head;
    while (current != nullptr) {
        Card* next = current->next;
        delete current;
        current = next;
    }
    head = nullptr;
    count = 0;
}

bool Hand::addCard(Card* card) {
    if (count >= maxCards || card == nullptr) {
        return false;
    }
    
    card->next = nullptr;
    
    // If the hand is empty, make this card the head
    if (head == nullptr) {
        head = card;
    } else {
        // Otherwise, find the last card and append to it
        Card* current = head;
        while (current->next != nullptr) {
            current = current->next;
        }
        current->next = card;
    }
    
    count++;
    return true;
}

Card* Hand::removeCard(int index) {
    if (index < 0 || index >= count || head == nullptr) {
        return nullptr;
    }
    
    Card* removedCard = nullptr;
    
    // If removing the first card
    if (index == 0) {
        removedCard = head;
        head = head->next;
    } else {
        // Find the card before the one we want to remove
        Card* current = head;
        for (int i = 0; i < index - 1; i++) {
            current = current->next;
            if (current == nullptr) {
                return nullptr;
            }
        }
        
        // Make sure the next card exists
        if (current->next == nullptr) {
            return nullptr;
        }
        
        // Remove the card
        removedCard = current->next;
        current->next = removedCard->next;
    }
    
    removedCard->next = nullptr;
    count--;
    return removedCard;
}

Card* Hand::getCard(int index) const {
    if (index < 0 || index >= count) {
        return nullptr;
    }
    
    Card* current = head;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }
    
    return current;
}

void Hand::render(sf::RenderWindow& window, float startX, float startY, float spacing) {
    Card* current = head;
    float xPos = startX;
    
    // Get the font from the window
    static bool fontLoaded = false;
    static sf::Font font;
    
    if (!fontLoaded) {
        if (font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
            fontLoaded = true;
        }
    }
    
    while (current != nullptr) {
        current->setPosition(xPos, startY);
        
        // Set up text elements for the card if the font is loaded
        if (fontLoaded) {
            current->setupTextElements(font);
        }
        
        current->render(window);
        xPos += spacing;
        current = current->next;
    }
}

Card* Hand::getCardAt(float x, float y) const {
    Card* current = head;
    
    while (current != nullptr) {
        if (current->contains(x, y)) {
            return current;
        }
        current = current->next;
    }
    
    return nullptr;
}

// Deck implementation
Deck::Deck() : head(nullptr), count(0) {
}

Deck::~Deck() {
    // Delete all cards in the deck
    Card* current = head;
    while (current != nullptr) {
        Card* next = current->next;
        delete current;
        current = next;
    }
    head = nullptr;
    count = 0;
}

void Deck::addCard(Card* card) {
    if (card == nullptr) {
        return;
    }
    
    card->next = nullptr;
    
    // If the deck is empty, make this card the head
    if (head == nullptr) {
        head = card;
    } else {
        // Otherwise, find the last card and append to it
        Card* current = head;
        while (current->next != nullptr) {
            current = current->next;
        }
        current->next = card;
    }
    
    count++;
}

Card* Deck::drawCard() {
    if (head == nullptr) {
        return nullptr;
    }
    
    Card* drawnCard = head;
    head = head->next;
    drawnCard->next = nullptr;
    count--;
    
    return drawnCard;
}

void Deck::shuffle() {
    // If the deck has fewer than 2 cards, it's already shuffled
    if (count < 2) {
        return;
    }
    
    // Convert the linked list to an array for easier shuffling
    Card** cards = new Card*[count];
    Card* current = head;
    
    for (int i = 0; i < count; i++) {
        cards[i] = current;
        current = current->next;
    }
    
    // Fisher-Yates shuffle algorithm
    for (int i = count - 1; i > 0; i--) {
        int j = std::rand() % (i + 1);
        Card* temp = cards[i];
        cards[i] = cards[j];
        cards[j] = temp;
    }
    
    // Rebuild the linked list
    head = cards[0];
    current = head;
    
    for (int i = 1; i < count; i++) {
        current->next = cards[i];
        current = current->next;
    }
    
    current->next = nullptr;
    
    // Clean up the temporary array
    delete[] cards;
}

void Deck::createStandardDeck() {
    // Clear any existing cards
    Card* current = head;
    while (current != nullptr) {
        Card* next = current->next;
        delete current;
        current = next;
    }
    head = nullptr;
    count = 0;
    
    // Create cafe-themed creature cards
    addCard(new CreatureCard("Barista", "A skilled coffee maker", 2, 2, 3));
    addCard(new CreatureCard("Cafe Manager", "Oversees cafe operations", 4, 3, 5));
    addCard(new CreatureCard("Regular Customer", "Loyal cafe patron", 1, 1, 2));
    addCard(new CreatureCard("Food Critic", "Reviews cafe quality", 3, 4, 2));
    addCard(new CreatureCard("Pastry Chef", "Creates delicious treats", 3, 2, 4));
    addCard(new CreatureCard("Waiter", "Serves customers efficiently", 2, 2, 2));
    addCard(new CreatureCard("Busboy", "Cleans tables quickly", 1, 1, 1));
    addCard(new CreatureCard("Dishwasher", "Keeps utensils clean", 2, 1, 3));
    
    // Create cafe-themed spell cards
    addCard(new SpellCard("Espresso Shot", "Boosts energy temporarily", 1, 2));
    addCard(new SpellCard("Coffee Spill", "Creates a mess", 2, 3));
    addCard(new SpellCard("Free Wifi", "Attracts customers", 3, 4));
    addCard(new SpellCard("Happy Hour", "Special discount time", 4, 5));
    addCard(new SpellCard("Special Blend", "Limited edition coffee", 2, 3));
    addCard(new SpellCard("Health Inspection", "Official cafe review", 3, 4));
    
    // Create cafe-themed building cards
    addCard(new BuildingCard("Coffee Machine", "Produces various coffees", 3, 4, 1));
    addCard(new BuildingCard("Display Case", "Shows off pastries", 2, 3, 1));
    addCard(new BuildingCard("Outdoor Seating", "Additional customer space", 4, 5, 2));
    addCard(new BuildingCard("Drive-Through", "Quick service option", 5, 5, 2));
    addCard(new BuildingCard("WiFi Router", "Provides internet access", 1, 2, 1));
    addCard(new BuildingCard("Cash Register", "Processes payments", 2, 3, 1));
    
    // Shuffle the deck
    shuffle();
}

bool Deck::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << count << std::endl;
    
    Card* current = head;
    while (current != nullptr) {
        // Format: CardType,Name,Description,ManaCost,ExtraValue1,ExtraValue2
        file << static_cast<int>(current->getType()) << ","
             << current->getName() << ","
             << current->getDescription() << ","
             << current->getManaCost() << ",";
        
        if (current->getType() == CardType::CREATURE) {
            CreatureCard* creature = static_cast<CreatureCard*>(current);
            file << creature->getAttack() << ","
                 << creature->getHealth() << std::endl;
        } else if (current->getType() == CardType::SPELL) {
            SpellCard* spell = static_cast<SpellCard*>(current);
            file << spell->getEffectValue() << ",0" << std::endl;
        } else if (current->getType() == CardType::BUILDING) {
            BuildingCard* building = static_cast<BuildingCard*>(current);
            file << building->getDurability() << ","
                 << building->getBoost() << std::endl;
        }
        
        current = current->next;
    }
    
    file.close();
    return true;
}

bool Deck::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Clear any existing cards
    Card* current = head;
    while (current != nullptr) {
        Card* next = current->next;
        delete current;
        current = next;
    }
    head = nullptr;
    count = 0;
    
    int cardCount;
    file >> cardCount;
    file.ignore(); // Skip newline
    
    for (int i = 0; i < cardCount; i++) {
        std::string line;
        std::getline(file, line);
        
        size_t commaPos = line.find(',');
        int cardType = std::stoi(line.substr(0, commaPos));
        line = line.substr(commaPos + 1);
        
        commaPos = line.find(',');
        std::string name = line.substr(0, commaPos);
        line = line.substr(commaPos + 1);
        
        commaPos = line.find(',');
        std::string description = line.substr(0, commaPos);
        line = line.substr(commaPos + 1);
        
        commaPos = line.find(',');
        int manaCost = std::stoi(line.substr(0, commaPos));
        line = line.substr(commaPos + 1);
        
        commaPos = line.find(',');
        int value1 = std::stoi(line.substr(0, commaPos));
        line = line.substr(commaPos + 1);
        
        int value2 = std::stoi(line);
        
        Card* newCard = nullptr;
        
        switch (static_cast<CardType>(cardType)) {
            case CardType::CREATURE:
                newCard = new CreatureCard(name, description, manaCost, value1, value2);
                break;
                
            case CardType::SPELL:
                newCard = new SpellCard(name, description, manaCost, value1);
                break;
                
            case CardType::BUILDING:
                newCard = new BuildingCard(name, description, manaCost, value1, value2);
                break;
        }
        
        if (newCard != nullptr) {
            addCard(newCard);
        }
    }
    
    file.close();
    return true;
}