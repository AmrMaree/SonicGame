#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
using namespace sf;
using namespace std;
int main()
{
    RenderWindow window(sf::VideoMode(1920,1080), "Sonic Game");
    RectangleShape ground(Vector2f(1920, 10));
    ground.setPosition(0,800);

    // Load the sprite sheet
    Texture sonictexture;
    sonictexture.loadFromFile("sonicsprite.png");

    // Create the sprite
    Sprite sonic(sonictexture);
    sonic.setTextureRect(IntRect(0, 0, 102, 105));
    sonic.setPosition(0, 0);

    //Variables
    int animeIndicator = 0;
    int coinCount = 0;
    int coinAnimationIndicator = 0;
    bool isCoinVisible = true;
    bool landed = false;
    double velocityY = 0;

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
            if (sonic.getGlobalBounds().intersects(ground.getGlobalBounds()))
            {
                landed = true;
                velocityY = 0;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
                    velocityY = sqrtf(9.81f * 0.5f);
            }
            else
            {
                landed = false;
                velocityY -= 0.01;
            }
             
            // Check if Sonic is out of bounds
            if (sonic.getPosition().y < 0.f)
                sonic.setPosition(sonic.getPosition().x, 0.f);
            if (sonic.getPosition().x < 0.f)
                sonic.setPosition(0.f, sonic.getPosition().y);

        animeIndicator %= 10;
        sonic.setTextureRect(IntRect(animeIndicator * 102, 0, 102, 105));
        if (sonic.getGlobalBounds().intersects(coin.getGlobalBounds()))
            isCoinVisible = false;
        coin.setTextureRect(IntRect(coinAnimationIndicator * 134, 0, 134, 134));
        coinAnimationIndicator++;
        coinAnimationIndicator %= 9;

        //2D camera
        View view(Vector2f(0, 0), Vector2f(1920, 1080));
        view.setCenter(sonic.getPosition()); //update
        window.setView(view);

        
        // Draw the sprite
        window.clear();
        window.draw(sonic);
        sonic.move(0, -velocityY);
        if(isCoinVisible)
             window.draw(coin);
        window.display();
    }

    return 0;
}
