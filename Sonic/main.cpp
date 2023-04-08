#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
using namespace sf;
using namespace std;

int main()
{
    RenderWindow window(sf::VideoMode(1696,1024), "Sonic Game");
    
    //Load the sprite sheet
    Texture sonictexture;
    sonictexture.loadFromFile("sonicsprite.png");

    //Create the sprite
    Sprite sonic(sonictexture);
    sonic.setTextureRect(IntRect(0, 0, 102, 105));
    sonic.setPosition(0, 0);
    
    
    //setting ground
    Texture groundtexture;
    groundtexture.loadFromFile("ground1.png");
    Sprite ground(groundtexture);
    ground.setPosition(0, 980);
    
    /*RectangleShape ground(Vector2f(100000, 30));
    ground.setPosition(-200, 900);
    ground.setFillColor(Color::White);*/

    //Variables
    int animeIndicator = 0;
    int coinCount = 0;
    int coinAnimationIndicator = 0;
    bool isCoinVisible = true;
    bool landed = false;
    double velocityY = 0;
    int positioncount = 1;
    int bgindex = 0;

    // Set up coins
    Texture coinsTextures;
    coinsTextures.loadFromFile("sonicRingsprite.png");
    Sprite coin(coinsTextures);
    coin.setPosition(500, 800);
    coin.setTextureRect(IntRect(0, 0, 134, 134));
    coin.setScale(0.4f, 0.4f);

    //background 
    Texture backgroundtexture;
    backgroundtexture.loadFromFile("lapper-bggreenhill1.jpg");
    Sprite background[2];

    for (int i = 0; i < 2; ++i)
    {
        background[i].setScale(2.5f, 2);
        background[i].setTexture(backgroundtexture);
        background[i].setPosition(Vector2f(i * 1696, 0));
    }

    //2D camera
    View view(Vector2f(0, 0), Vector2f(1696, 1024));
    view.setCenter(sonic.getPosition()); //update
    window.setView(view);

    //setting time 
    Clock clock;
    window.setFramerateLimit(60);

   
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }
        
        // set frames 
        float currentTime = clock.restart().asSeconds();
        float fps = 1.0f / (currentTime);
        cout << "fps:" << fps << endl;


        // Move the player using A,D and space keys

        if (Keyboard::isKeyPressed(Keyboard::A))
        {
            sonic.move(-10, 0);
            view.move(-10, 0);
            animeIndicator++;
            sonic.setOrigin(sonic.getLocalBounds().width, 0);
            sonic.setScale(-1, 1);
        }
        else if (Keyboard::isKeyPressed(Keyboard::D))
        {
            sonic.move(10, 0);
            view.move(10, 0);
            animeIndicator++;
            sonic.setOrigin(0, 0);
            sonic.setScale(1, 1);
        }

        //collision with rectangle and jumping 

        if (sonic.getGlobalBounds().intersects(ground.getGlobalBounds()))
        {
            landed = true;
            velocityY = 0;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
                velocityY = 18.5f;
        }
        else
        {
            landed = false;
            velocityY -= 0.9f;
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

        //background motion
        if (sonic.getPosition().x > positioncount * 1696)
        {
            positioncount++;
            background[bgindex].setPosition(Vector2f(positioncount * 1696, 0));
            bgindex++;
            if (bgindex > 1)
                bgindex = 0;
        }

        // Draw the sprite
        view.setCenter(Vector2f(sonic.getPosition().x + 848, 540));
        window.clear();
        window.setView(view);
        window.draw(background[bgindex]);
        window.draw(sonic);
        window.draw(ground);
        sonic.move(0, -velocityY);
        if(isCoinVisible)
             window.draw(coin);
        window.display();
    }

    return 0;
}
