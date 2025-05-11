#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include "Board.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const std::string WINDOW_TITLE = "CafeWars";
const std::string FONT_PATH = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
const std::string CARD_TEXTURE_PATH = "card.png";
const std::string SAVE_FILE_PATH = "game.txt";

int main() {
    // Initialize random seed
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    // Create the SFML window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE);
    window.setFramerateLimit(60);
    
    // Create and initialize the game board
    Board gameBoard;
    gameBoard.initialize();
    gameBoard.setupGraphics(window);
    
    // Track time between frames for animation
    sf::Clock clock;
    
    // Main game loop
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        
        // Process events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
                else if (event.key.code == sf::Keyboard::Space) {
                    // End current player's turn
                    gameBoard.endTurn();
                }
                else if (event.key.code == sf::Keyboard::S) {
                    // Save game
                    if (gameBoard.saveGame(SAVE_FILE_PATH)) {
                        std::cout << "Game saved successfully to " << SAVE_FILE_PATH << std::endl;
                    } else {
                        std::cout << "Failed to save game" << std::endl;
                    }
                }
                else if (event.key.code == sf::Keyboard::L) {
                    // Load game
                    if (gameBoard.loadGame(SAVE_FILE_PATH)) {
                        std::cout << "Game loaded successfully from " << SAVE_FILE_PATH << std::endl;
                    } else {
                        std::cout << "Failed to load game" << std::endl;
                    }
                }
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    // Handle mouse clicks for card selection and placement
                    gameBoard.handleBoardClick(event.mouseButton.x, event.mouseButton.y);
                }
            }
        }
        
        // Update game state
        gameBoard.update(deltaTime);
        
        // Render the game
        window.clear(sf::Color(50, 100, 50)); // Dark green background
        gameBoard.render(window);
        window.display();
        
        // Check if game is over
        if (gameBoard.isGameOver()) {
            // Give players time to see the game over message
            sf::sleep(sf::seconds(5));
            window.close();
        }
    }
    
    return 0;
}