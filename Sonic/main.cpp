#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
using namespace sf;
using namespace std;
int main()
{
    RenderWindow window(VideoMode(1920, 1080), "Sonic Game");

    // Load the sprite sheet
    Texture sonictexture;
    sonictexture.loadFromFile("sonicsprite.png");

    // Create the sprite
    Sprite sonic(sonictexture);
    sonic.setTextureRect(IntRect(0, 0, 102, 105));
    sonic.setPosition(0, 720);

    //Variables
    int animeIndicator = 0;
    int coinCount = 0;
    int coinAnimationIndicator = 0;

    // Set up coins
    Texture coinsTextures;
    coinsTextures.loadFromFile("sonicRingsprite.png");
    Sprite coin(coinsTextures);
    coin.setPosition(500, 500);
    coin.setTextureRect(IntRect(0, 0, 134, 134));
    coin.setScale(0.4f, 0.4f);
   

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

        coin.setTextureRect(IntRect(coinAnimationIndicator * 134, 0, 134, 134));
        coinAnimationIndicator++;
        coinAnimationIndicator %= 9;

        // Draw the sprite
        window.clear();
        window.draw(sonic);
        window.draw(coin);
        window.display();
    }

    return 0;
}
