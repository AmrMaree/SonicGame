#include <SFML/Graphics.hpp>
using namespace sf;

int main()
{
    RenderWindow window(VideoMode(1920, 1080), "Sonic Game   ");

    // Load the sprite sheet
    Texture sonictexture;
    sonictexture.loadFromFile("sonicsprite.png");

    // Create the sprite
    Sprite sonic(sonictexture);
    sonic.setTextureRect(IntRect(0, 0, 102, 105));
    sonic.setPosition(0, 720);

    int animeIndicator = 0;
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }
        if (Keyboard::isKeyPressed(Keyboard::Left))
        {
            sonic.move(-1, 0);
            animeIndicator++;
            sonic.setOrigin(sonic.getLocalBounds().width, 0);
            sonic.setScale(-1, 1);

        }
        else if (Keyboard::isKeyPressed(Keyboard::Right))
        {
            sonic.move(1, 0);
            animeIndicator++;
            sonic.setOrigin(0, 0);
            sonic.setScale(1, 1);
        }
        else if (Keyboard::isKeyPressed(Keyboard::Up))
        {
            sonic.move(0,-1);
        }
        else if (Keyboard::isKeyPressed(Keyboard::Down))
        {
            sonic.move(0,1);
        }
        animeIndicator %= 9;
        sonic.setTextureRect(IntRect(animeIndicator * 102, 0, 102, 105));


        // Draw the sprite
        window.clear();
        window.draw(sonic);
        window.display();
    }

    return 0;
}
