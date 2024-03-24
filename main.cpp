#include "chip8.h"
#include <SFML/Graphics.hpp>

int main() {
    Chip8 chip8;

    sf::RenderWindow window(
        sf::VideoMode(hardware::display_width * hardware::pixel_size, 
                      hardware::display_height * hardware::pixel_size),
        "Chip-8 Emulator");

    // Create texture and sprite to represent pixels
    sf::Texture texture;
    texture.create(hardware::display_width, hardware::display_height);
    sf::Sprite sprite(texture);

    // Get reference to display data
    auto& display = chip8.getDisplay();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Update Chip8 emulation
        chip8.emulateCycle();

        // Update texture directly from display data
        texture.update((const sf::Uint8*)display.data());

        // Clear the window
        window.clear();

        // Draw the sprite
        window.draw(sprite);

        // Display
        window.display();
    }

    return 0;
}
