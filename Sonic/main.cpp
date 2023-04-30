#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
using namespace sf;
using namespace std;

const int MAP_WIDTH = 10;
const int MAP_HEIGHT = 10;

void drawMap(RenderWindow& window, Sprite& sprite, Sprite& sonic, Sprite& sprite2, int map[][MAP_HEIGHT], int mapWidth, int mapHeight, vector<Sprite>& blocks) {
    bool collided = false; // flag to indicate if a collision occurred
    for (int i = 0; i < mapWidth; i++) {
        for (int j = 0; j < mapHeight; j++) {
            if (map[i][j] == 1) {
                sprite.setPosition(i * sprite.getGlobalBounds().width, j * sprite.getGlobalBounds().height);
                blocks.push_back(sprite);
                window.draw(sprite);
            }
            else if (map[i][j] == 2) {
                sprite2.setPosition(i * sprite2.getGlobalBounds().width, j * sprite2.getGlobalBounds().height);
                blocks.push_back(sprite2);
                window.draw(sprite2);

            }
        }
    }
}

int main()
{
    RenderWindow window(sf::VideoMode(1696, 1024), "Sonic Game");
    vector <Sprite> blocks;


    //Make sonic sprite
    Texture sonictexture;
    sonictexture.loadFromFile("sonicsprite.png");
    Sprite sonic(sonictexture);
    sonic.setTextureRect(IntRect(0, 0, 102, 105));
    sonic.setPosition(0, 0);


    //setting ground
    Texture groundtexture;
    groundtexture.loadFromFile("ground1.png");
    Sprite ground(groundtexture);
    ground.setPosition(0, 980);

    //Making long and short blocks
    Texture shortBlockTexture; //short block
    shortBlockTexture.loadFromFile("shortBlock.png");
    Sprite shortBlockSprite(shortBlockTexture);
    Texture longBlockTexture; //long block
    longBlockTexture.loadFromFile("longBlock.png");
    Sprite longBlockSprite(longBlockTexture);

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
    coin.setPosition(500, 800);
    coin.setTextureRect(IntRect(0, 0, 134, 134));
    coin.setScale(0.4f, 0.4f);

    //background 
    Texture backgroundtexture;
    backgroundtexture.loadFromFile("lapper-bggreenhill1.jpg");
    Sprite background[100];

    for (int i = 0; i < 100; ++i)
    {
        background[i].setScale(1, 1);
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

    //Map array

    int map[MAP_WIDTH][MAP_HEIGHT] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 2},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    };


    while (window.isOpen())
    {
         Time deltatime = clock.restart();
         float dt = deltatime.asSeconds();

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
            if (Keyboard::isKeyPressed(Keyboard::Space))
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

    
      

        // Draw the sprite
        view.setCenter(Vector2f(sonic.getPosition().x + 848, 540));
        window.clear();
        window.setView(view);
        for(int i=0;i<100;++i)
        {
            window.draw(background[i]);
        }
            
        drawMap(window, longBlockSprite, sonic, shortBlockSprite, map, MAP_WIDTH, MAP_HEIGHT, blocks);
        window.draw(sonic);
        window.draw(ground);
        sonic.move(0, -velocityY);
        if (isCoinVisible)
            window.draw(coin);
        window.display();
    }

    return 0;
}
