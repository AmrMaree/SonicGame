#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>
#include <string>
using namespace sf;
using namespace std;

const int MAP_WIDTH = 10;
const int MAP_HEIGHT = 10;

//bullet 
struct Bullet {
    float speed;
    int moveTo;
    Sprite bulletSprite;
    int magazine;
    float cooldownUse;
};

struct player
{
    Sprite sprite;
    float currentframe;
    float move_X, move_Y;
    FloatRect rect;
    bool onground;
    int last_key_pressed;
    int health = 10;
    int index = -1;
    int droptype = -1;
    int shootCooldown;
    float speed = 1;
    float addSpeed = 0.0;
    vector<Bullet>bullet;
    bool canShoot = 0;


    void sp(Texture& sonicTexture)
    {
        sprite.setTexture(sonicTexture);
        move_X = 0;
        move_Y = 0;
        currentframe = 0;
        last_key_pressed = 1;
    }
    void update(float time,View &view)
    {
        rect.left += move_X * time;
        view.move(move_X * time,0);
        if (!onground) {
            move_Y += (0.005 * time);
        }
        rect.top += move_Y * time;
        onground = false;
        if (rect.left < 0) {
            rect.left = 0;
        }
        //I can add a limit from right
        if (rect.top > 529) {
            rect.top = 529;
            move_Y = 0;
            onground = true;
        }
        currentframe +=  0.015 * time;
        if (currentframe > 10)
            currentframe -= 10;

        if (move_X > 0) {
            sprite.setTextureRect(IntRect(int(currentframe) * 102, 0, 102, 105));
        }
        if (move_X < 0) {
            sprite.setTextureRect(IntRect(int(currentframe) * 102, 0, 102, 105));
        }
        cout << rect.left << "  " << rect.top<<"  "<<currentframe << endl;
        sprite.setPosition(rect.left, rect.top);
        move_X = 0;
    }
};

struct Help {
    Sprite dropShape; //drop
    Sprite targetShape; // block
    int type; // 0, 1, 2, 3

};
vector<Help>dropBag;
Sprite Drops[4]; // 0 pistol, 1 rifle, 2 health, 3 speed
Texture DropsTex[6];
void Setdrops() {
    DropsTex[0].loadFromFile("pistol.png");
    DropsTex[1].loadFromFile("Rifle.png");
    DropsTex[2].loadFromFile("heart.png");
    DropsTex[3].loadFromFile("thunder");
    DropsTex[4].loadFromFile("RifleBullet.png");
    DropsTex[5].loadFromFile("PistolBullet.png");
    for (int i = 0; i < 4; i++)
    {
        Drops[i].setTexture(DropsTex[i]);
    }
    Drops[0].setScale(2.5, 2.5);
    Drops[1].setScale(3, 3);
    Drops[2].setScale(0.13, 0.13);
    Drops[3].setScale(2, 2);
}

void dropADrop() {
    for (int i = 0; i < dropBag.size(); i++) {
        dropBag[i].dropShape.move(0, 5);
        if (dropBag[i].dropShape.getGlobalBounds().intersects(dropBag[i].targetShape.getGlobalBounds()))
            dropBag[i].dropShape.move(0, -5);
    }
}
void chooseDrop(Sprite ground1[], Clock& timerAdd, Clock& timerDelete) {
    if (timerAdd.getElapsedTime().asSeconds() >= 2) {
        int indexDrop = rand() % 4;
        int indexBlock = rand() % 18;
        Help help;
        help.dropShape = Drops[indexDrop];
        help.targetShape = ground1[indexBlock];
        help.dropShape.setPosition(help.targetShape.getPosition().x + help.targetShape.getScale().x * 125 / 2 - 20, -100);
        help.type = indexDrop;
        dropBag.push_back(help);
        timerAdd.restart();

    }
    if (timerDelete.getElapsedTime().asSeconds() >= 8) {
        if (!dropBag.empty()) {
            dropBag.erase(dropBag.begin());
            timerDelete.restart();
        }
    }
}
void dropCollision(player& player) {
    for (int i = 0; i < dropBag.size(); i++)
    {
        if (player.sprite.getGlobalBounds().intersects(dropBag[i].dropShape.getGlobalBounds())) {
            player.droptype = dropBag[i].type;
            dropBag.erase(dropBag.begin() + i);

        }
    }

}
void checkDrop(player& player) {
    if (player.droptype == -1) {
        return;
    }
    else {
        if (player.droptype == 0) {
            Bullet bullet;
            bullet.bulletSprite.setTexture(DropsTex[4]);
            bullet.bulletSprite.setScale(2, 2);
            bullet.speed = 5;
            bullet.cooldownUse = 50;
            bullet.magazine = 3;
            player.canShoot = 1;
            for (int i = 0; i < bullet.magazine; i++)
            {
                player.bullet.push_back(bullet);
                player.index++;
            }
        }
        else if (player.droptype == 1) {
            Bullet bullet;
            bullet.bulletSprite.setTexture(DropsTex[5]);
            bullet.bulletSprite.setScale(2, 2);
            bullet.speed = 9;
            bullet.cooldownUse = 30;
            bullet.magazine = 7;
            player.canShoot = 1;
            for (int i = 0; i < bullet.magazine; i++)
            {
                player.bullet.push_back(bullet);
                player.index++;
            }
        }
        else if (player.droptype == 2) {
            if (player.health < 10)
                player.health++;
        }
        else if (player.droptype == 3) {
            player.addSpeed = 10;
            player.speed = 1.4;
        }
        player.droptype = -1;
    }
}
void resetSpeed(player& player) {
    if (player.addSpeed <= 0) {
        player.addSpeed = 0;
        player.speed = 1;
        return;

    }
    else {
        player.addSpeed -= 0.01;
    }
}

void bulletCooldown(player& player) {
    if (player.shootCooldown > 0) {
        player.shootCooldown -= 0.00001;
    }
    if (player.shootCooldown <= 0 && player.canShoot == 0) {
        player.canShoot = 1;
    }
}

void moveBullets(vector<Bullet>& bullet) {
    for (int i = 0; i < bullet.size(); i++) {
        if (bullet[i].moveTo == 2) {
            bullet[i].bulletSprite.move(-1 * bullet[i].speed, 0);
        }
        if (bullet[i].moveTo == 1) {
            bullet[i].bulletSprite.move(1 * bullet[i].speed, 0);

        }
        if (bullet[i].bulletSprite.getPosition().x >= 1750 ||
            bullet[i].bulletSprite.getPosition().x <= -100) {
            bullet.erase(bullet.begin() + i);
        }

    }
}


void drawCoins(RenderWindow& window,player& sonic, Sprite& coin, int map[][MAP_HEIGHT], int mapWidth, int mapHeight, int coinAnimationIndicator, vector<Sprite>& coinslist,int& score,Text& text) {
    
    for (int i = 0; i < mapWidth; i++) {
        for (int j = 0; j < mapHeight; j++) {
            if (map[i][j] == 1) {
                coin.setPosition(i * coin.getGlobalBounds().width*10, j * coin.getGlobalBounds().height*10);
                if (sonic.sprite.getGlobalBounds().intersects(coin.getGlobalBounds())) {
                    map[i][j] = 0;
                    score++;
                    text.setString(" Score " + to_string(score));
                } 
                window.draw(coin);
                coin.setTextureRect(IntRect(coinAnimationIndicator * 134, 0, 134, 134));
                coinAnimationIndicator++;
                coinAnimationIndicator %= 9;
               // coinslist.push_back(coin);
            }
        }
    }
}

int main()
{
    srand(static_cast<unsigned>(time(NULL)));
    RenderWindow window(sf::VideoMode(1696, 1024), "Sonic Game");
    vector <Sprite> coinslist(50);
    Clock timerAdd, timerDelete;

    //Make sonic texture
    Texture sonictexture;
    sonictexture.loadFromFile("sonicsprite.png");

    //declaring sonic
    player sonic;
    sonic.sprite.setTextureRect(IntRect(0, 0, 102, 105));
    sonic.sp(sonictexture);
    sonic.rect.left = 50;
    sonic.rect.top = 529;

    //setting ground
    Texture groundtexture;
    groundtexture.loadFromFile("map.png");
    Sprite ground(groundtexture);
    ground.setPosition(0, 633);
    ground.setScale(2.3f, 2.3f);

    //setting ground
    Texture ground1texture;
    ground1texture.loadFromFile("groundTextures/block89.png");
    Sprite ground1[3];
    for (int i = 0; i < 3; ++i)
    {
        ground1[i].setScale(2.3f, 2.3f);
        ground1[i].setTexture(ground1texture);
    }
    ground1[0].setPosition(333, 430);
    ground1[1].setPosition(833, 230);
    ground1[2].setPosition(1283, 211);

    //Variables
    int coinCount = 0;
    int coinAnimationIndicator = 0;
    bool isCoinVisible = true;
    bool landed = false;
    double velocityY = 0;
    int score = 0;

    //creating coins 
    Texture coinsTextures;
    coinsTextures.loadFromFile("sonicRingsprite.png");
    Sprite coins;
    coins.setTexture(coinsTextures);
    coins.setTextureRect(IntRect(0, 0, 134, 134));
    coins.setScale(0.4f, 0.4f);

    //background 
    Texture backgroundtexture;
    backgroundtexture.loadFromFile("lapper-bggreenhill1.jpg");
    Sprite background[100];
    for (int i = 0; i < 100; ++i)
    {
        background[i].setTexture(backgroundtexture);
        background[i].setPosition(Vector2f(i * 1696, 0));
    }

    //Score
    Font font;
    font.loadFromFile("NiseSegaSonic.TTF");
    Text text;
    text.setFont(font);
    text.setString(" Score " + to_string(score));
    text.setFillColor(Color(50, 205, 50, 255));
    text.setPosition(10, 65);
    text.setCharacterSize(32);

    //2D camera
    View view(Vector2f(0, 0), Vector2f(1696, 1024));
    view.setCenter(sonic.sprite.getPosition()); //update
    window.setView(view);



    //Map array
    int map[MAP_WIDTH][MAP_HEIGHT] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    };

    //powerups
    Setdrops();

    while (window.isOpen())
    {
        //setting time 
        Clock clock;
        window.setFramerateLimit(60);
        float time = clock.getElapsedTime().asMicroseconds();
        clock.restart();
        time *= 27.5;

        //powerups 
        chooseDrop(ground1, timerAdd, timerDelete);
        dropADrop();
        dropCollision(sonic);
        checkDrop(sonic);
        resetSpeed(sonic);

        //bullet 
        bulletCooldown(sonic);
        moveBullets(sonic.bullet);

        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        // Move the player using A,D and space keys
        if (sonic.last_key_pressed == 1) {
            sonic.sprite.setTextureRect(IntRect(0, 0, 102, 105));
        }
        if (sonic.last_key_pressed == 2) {
            sonic.sprite.setTextureRect(IntRect(0, 0, 102, 105));
            sonic.sprite.setScale(-1, 1);
        }
        if (Keyboard::isKeyPressed(Keyboard::A))
        {
            sonic.move_X = -0.73;
            sonic.last_key_pressed = 2;
            sonic.sprite.setOrigin(sonic.sprite.getLocalBounds().width, 0);
            sonic.sprite.setScale(-1, 1);
        }
        if (Keyboard::isKeyPressed(Keyboard::D))
        {
            sonic.move_X = 0.73;
            sonic.last_key_pressed = 1;
            sonic.sprite.setOrigin(0, 0);
            sonic.sprite.setScale(1, 1);
        }
        if (Keyboard::isKeyPressed(Keyboard::Space))
        {
            if (sonic.onground)
            {
                sonic.move_Y = -1.4;
                sonic.onground = false;
            }
        }
         if (Keyboard::isKeyPressed(Keyboard::G) && sonic.index >= 0 && sonic.canShoot) {
            sonic.bullet[sonic.index].bulletSprite.setPosition(sonic.sprite.getPosition().x, sonic.sprite.getPosition().y);
            sonic.shootCooldown = sonic.bullet[sonic.index].cooldownUse;
            sonic.bullet[sonic.index].moveTo = sonic.last_key_pressed;
            sonic.index--;
            sonic.canShoot = 0;
        }

       // text.setPosition(sonic.sprite.getPosition().x + 20, 65);

        //animation of coins
        for (int i = 0; i < coinslist.size(); i++) {
            coinslist[i].setTextureRect(IntRect(coinAnimationIndicator * 134, 0, 134, 134));
            coinAnimationIndicator++;
            coinAnimationIndicator %= 9;
        }

        //player collision
      // Player_Collision(blocks, sonic);


        // Draw the sprite
        view.setCenter(Vector2f(sonic.sprite.getPosition().x + 848, 540));
        sonic.update(time, view);
        window.clear();
        window.setView(view);
        for (int i = 0; i < 100; ++i)
        {
            window.draw(background[i]);
        }

        //To draw the coins and increase the score 
        drawCoins(window,sonic, coins, map, MAP_WIDTH, MAP_HEIGHT, coinAnimationIndicator, coinslist,score,text);
        window.draw(ground);
        for (int i = 0; i < sonic.bullet.size(); i++)
        {
            window.draw(sonic.bullet[i].bulletSprite);
        }

        for (int i = 0; i < dropBag.size(); i++) {
            window.draw(dropBag[i].dropShape);
        }

        window.draw(sonic.sprite);
        for (int i = 0; i < 3; i++) {
            window.draw(ground1[i]);
        }
        window.draw(text);
        window.display();
    }
    return 0;
}