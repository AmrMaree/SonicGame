#include <SFML/Graphics.hpp>
#include "Menu.h"
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <string>
#include <fstream>
#include<iostream>
#include<SFML/Audio.hpp>
#include<sstream>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<ctime>
#include<random>
#include<iomanip>

using namespace std;
using namespace sf;

bool gameover = false;
string name;
int score = 0;
bool level1isfinished = false;
bool level2isfinished = false;

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
    Texture fullsonictexture;
    float currentframe;
    float runcurrentframe;
    float waitingframe;
    float jumpframe;
    Vector2f velocity;
    Vector2f acceleration;
    bool onground;
    const float gravity = 700.0f;
    float moveSpeed = 270.0f;
    float limitSpeed = 550;
    const float jumpHeight = 350.0f;
    float groundHeight = 726.0f;
    int last_key_pressed;
    int damage;
    int index;
    int droptype;
    int shootCooldown;
    float speed;
    float addSpeed;
    vector<Bullet>bullet;
    bool canShoot;
    void sp(Texture& sonicTexture)
    {
        sprite.setTexture(sonicTexture);
        currentframe = 0;
        runcurrentframe = 0;
        last_key_pressed = 1;
        damage = 0;
        index = -1;
        droptype = -1;
        speed = 1;
        addSpeed = 0.0;
        sprite.setOrigin(15, 0);
    }
    void update(float time, float deltaTime, Sprite block[])
    {
        acceleration = Vector2f(0.0f, 0.0f);

        // Update velocity and acceleration based on player input
        if (Keyboard::isKeyPressed(Keyboard::Space) && onground) {
            velocity.y = -sqrtf(2.0f * jumpHeight * gravity);
            onground = false;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            if (velocity.x > 0) {
                while (velocity.x > 0) {
                    velocity.x -= 0.01;
                }
            }
            else
                acceleration.x = -moveSpeed;
            sprite.setScale(-2.3f, 2.3f); // Flip the sprite to face left
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            if (velocity.x < 0) {
                while (velocity.x < 0) {
                    velocity.x += 0.01;
                }
            }
            else
                acceleration.x = moveSpeed;
            sprite.setScale(2.3f, 2.3f); // Flip the sprite to face right
        }
        else {
            acceleration.x = 0.0f;
            if (onground) {
                if (velocity.x > 0) {
                    while (velocity.x > 0) {
                        velocity.x -= 0.0001;
                    }
                }
                else {
                    while (velocity.x < 0) {
                        velocity.x += 0.0001;
                    }
                }
                /*while (velocity.y < 0) {
                    velocity.y += 20;
                }*/
            }
            // sprite.setTextureRect(IntRect(0, 0, 50, 55));
        }

       // cout << velocity.x << "   " << velocity.y << endl;
        // Apply gravity to the character
        velocity.y += gravity * deltaTime;

        // Apply acceleration to the velocity
        if (velocity.x <= limitSpeed && velocity.x >= -limitSpeed)
            velocity.x += acceleration.x * deltaTime;

        // Update the character's position
        sprite.setPosition(sprite.getPosition().x + velocity.x * deltaTime, sprite.getPosition().y + velocity.y * deltaTime);

        // Check for collisions with the blocks
        for (int i = 0; i < 23; i++) {
            FloatRect blockBounds(block[i].getGlobalBounds().left, block[i].getGlobalBounds().top, block[i].getGlobalBounds().width, block[i].getGlobalBounds().height);
            FloatRect characterBounds(sprite.getGlobalBounds().left, sprite.getGlobalBounds().top, 60, 103);

            if (characterBounds.intersects(blockBounds)) {
                // Collision detected
                Vector2f characterPosition = sprite.getPosition();
                Vector2f blockPosition = block[i].getPosition();

                if (characterPosition.y + characterBounds.height <= blockPosition.y + 27.0f) {
                    // Character is on top of the block
                    sprite.setPosition(characterPosition.x, blockPosition.y - characterBounds.height);
                    velocity.y = 0.0f;
                    onground = true;
                }
                else if (characterPosition.x + characterBounds.width <= blockPosition.x + 20.0f) {
                    // Character is colliding from the left
                    sprite.setPosition(blockPosition.x - characterBounds.width, characterPosition.y);
                    velocity.x = 0.0f;
                }
                else if (characterPosition.x >= blockPosition.x + blockBounds.width - 10.0f) {
                    // Character is colliding from the right
                    sprite.setPosition(blockPosition.x + blockBounds.width, characterPosition.y);
                    velocity.x = 0.0f;
                }
                else if (characterPosition.y <= blockPosition.y + blockBounds.height) {
                    // Character is colliding from the bottom
                    sprite.setPosition(characterPosition.x, blockPosition.y + blockBounds.height);
                    velocity.y = 0.0f;
                }
            }
        }
        // Check for collisions with the ground
        if (sprite.getPosition().y + sprite.getGlobalBounds().height >= groundHeight) {
            sprite.setPosition(sprite.getPosition().x, groundHeight - sprite.getGlobalBounds().height);
            velocity.y = 0.0f;
            onground = true;
        }

        //Animation of sonic
        if (((velocity.x >= 500) || (velocity.x <= -500))&&onground ==true) {
            // Running animation
            runcurrentframe += 0.015f * time;
            sprite.setOrigin(27, 0);
            if (runcurrentframe > 4) {
                runcurrentframe -= 4;
            }
            fullsonictexture.loadFromFile("Textures/fullapprovedsonic.png");
            sprite.setTexture(fullsonictexture);
            sprite.setTextureRect(IntRect(0, 0, 54, 39));
            sprite.setPosition(sprite.getPosition().x, sprite.getPosition().y + 10);
            if (acceleration.x != 0)
                sprite.setTextureRect(IntRect((int(runcurrentframe) * 54), 0, 54, 39));
        }
        /*else if (onground && !Keyboard::isKeyPressed(Keyboard::A) && !Keyboard::isKeyPressed(Keyboard::D) && !Keyboard::isKeyPressed(Keyboard::Space)) {
            waitingframe += 0.0015f * time;
            if (waitingframe > 5)
                waitingframe = 0;
            fullsonictexture.loadFromFile("Textures/waitingsonic.png");
            sprite.setTexture(fullsonictexture);
            sprite.setTextureRect(IntRect(int(waitingframe) * 45, 0, 45, 43));
        }*/
         else if (velocity.y < 0||!onground){
            fullsonictexture.loadFromFile("Textures/jumpingsonic.png");
            sprite.setTexture(fullsonictexture);
            sprite.setOrigin(28, 0);
            if (jumpframe > 10) {
                sprite.setTextureRect(IntRect(int(jumpframe) * 45, 0, 45, 39));
            }
            else {
                sprite.setTextureRect(IntRect(405, 0, 44, 39));
            }
            jumpframe += 1;
        }
        else
        {
            // Idle animation
            currentframe += 0.00523f * time;
            sprite.setOrigin(15, 0);
            if (currentframe > 12) {
                currentframe -= 12;
            }
            fullsonictexture.loadFromFile("Textures/approvedsonic.png");
            sprite.setTexture(fullsonictexture);
            if (acceleration.x != 0)
                sprite.setTextureRect(IntRect(int(currentframe) * 50, 0, 50, 55));
        }

    }
};
struct Enemy {
    Sprite sprite;
    float speed;
    float x;
    float animation = 0;
    int health = 3 * 19;
    float posX = 700.0f;
    float moveSpeed = 4.0f;
    float moveRange = 700.0f;
    bool moveRight = true;
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
    DropsTex[0].loadFromFile("Textures/pistol3.png");
    DropsTex[1].loadFromFile("Textures/Rifle2.png");
    DropsTex[2].loadFromFile("Textures/heart (3).png");
    DropsTex[3].loadFromFile("Textures/thunder.png");
    DropsTex[4].loadFromFile("Textures/bullet lazer.png");
    DropsTex[5].loadFromFile("Textures/longshot laser.png");
    for (int i = 0; i < 4; i++)
    {
        Drops[i].setTexture(DropsTex[i]);
    }
    Drops[0].setScale(0.15, 0.15);
    Drops[1].setScale(0.15, 0.15);
    Drops[2].setScale(2.6, 2.6);
    Drops[3].setScale(2.5, 2.5);
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
        int indexBlock = rand() % 22;
        Help help;
        help.dropShape = Drops[indexDrop];
        help.targetShape = ground1[indexBlock];
        help.dropShape.setPosition(help.targetShape.getPosition().x + help.targetShape.getScale().x * 125 / 2 - 20, -100);
        help.type = indexDrop;
        dropBag.push_back(help);
        timerAdd.restart();

    }
    if (timerDelete.getElapsedTime().asSeconds() >= 6) {
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
        if (player.droptype == 0) { //pistol
            Bullet bullet;
            bullet.bulletSprite.setTexture(DropsTex[4]);
            bullet.bulletSprite.setScale(0.6, 0.6);
            bullet.speed = 8;
            bullet.cooldownUse = 20;
            bullet.magazine = 5;
            player.canShoot = 1;
            for (int i = 0; i < bullet.magazine; i++)
            {
                player.bullet.push_back(bullet);
                player.index++;
            }
        }
        else if (player.droptype == 1) {// rifle
            Bullet bullet;
            bullet.bulletSprite.setTexture(DropsTex[5]);
            bullet.bulletSprite.setScale(0.35, 0.35);
            bullet.speed = 12;
            bullet.cooldownUse = 10;
            bullet.magazine = 10;
            player.canShoot = 1;
            for (int i = 0; i < bullet.magazine; i++)
            {
                player.bullet.push_back(bullet);
                player.index++;
            }
        }
        else if (player.droptype == 2) {
            if (player.damage > 0)
                player.damage--;
        }
        else if (player.droptype == 3) {
            player.addSpeed = 40;
            player.moveSpeed = 310;
            player.limitSpeed = 650;
        }
        player.droptype = -1;
    }
}
void resetSpeed(player& player) {
    if (player.addSpeed <= 0) {
        player.addSpeed = 0;
        player.moveSpeed = 270;
        player.limitSpeed = 550;
        return;

    }
    else {
        player.addSpeed -= 1;
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
        if (bullet[i].bulletSprite.getPosition().x >= 12500 ||
            bullet[i].bulletSprite.getPosition().x <= -100) {
            bullet.erase(bullet.begin() + i);
        }

    }
}
void coin(Sprite coins[]) {
    coins[0].setPosition(430, 570);
    coins[1].setPosition(630, 570);
    coins[2].setPosition(830, 570);
    coins[3].setPosition(1030, 570);
    coins[4].setPosition(1230, 570);
    //floor 

    coins[5].setPosition(560, 280);
    coins[6].setPosition(570, 280);
    coins[7].setPosition(1830, 570);
    coins[8].setPosition(2030, 570);
    coins[9].setPosition(2230, 570);
    coins[10].setPosition(2430, 570);
    coins[11].setPosition(2630, 570);
    coins[12].setPosition(2830, 570);
    coins[13].setPosition(3030, 570);
    coins[14].setPosition(3230, 570);
    coins[15].setPosition(3430, 570);
    coins[16].setPosition(3630, 570);
    coins[17].setPosition(3830, 570);
    coins[18].setPosition(4030, 570);
    coins[19].setPosition(4230, 570);
    coins[20].setPosition(4430, 570);
    coins[21].setPosition(4630, 570);
    coins[22].setPosition(4830, 570);
    coins[23].setPosition(5030, 570);
    coins[24].setPosition(5230, 570);
    coins[25].setPosition(430, 570);
    coins[26].setPosition(630, 570);
    coins[27].setPosition(830, 570);
    coins[28].setPosition(1030, 570);
    coins[29].setPosition(1230, 570);
    coins[30].setPosition(1430, 570);
    coins[31].setPosition(1630, 570);
    coins[32].setPosition(1830, 570);
    coins[33].setPosition(2030, 570);
    coins[34].setPosition(2230, 570);
    coins[35].setPosition(2430, 570);
    coins[36].setPosition(2630, 570);
    coins[37].setPosition(2830, 570);
    coins[38].setPosition(3030, 570);
    coins[39].setPosition(3230, 570);
    coins[40].setPosition(3430, 570);
    coins[41].setPosition(3630, 570);
    coins[42].setPosition(3830, 570);
    coins[43].setPosition(4030, 570);
    coins[44].setPosition(4230, 570);
    coins[45].setPosition(4430, 570);
    coins[46].setPosition(4630, 570);
    coins[47].setPosition(4830, 570);
    coins[48].setPosition(5030, 570);
    coins[49].setPosition(5230, 570);
    coins[50].setPosition(430, 570);
    coins[51].setPosition(630, 570);
    coins[52].setPosition(830, 570);
    coins[53].setPosition(1030, 570);
    coins[54].setPosition(1230, 570);
    coins[55].setPosition(1430, 570);
    coins[56].setPosition(1630, 570);
    coins[57].setPosition(1830, 570);
    coins[58].setPosition(2030, 570);
    coins[59].setPosition(2230, 570);
    coins[60].setPosition(2430, 570);
    coins[61].setPosition(2630, 570);
    coins[62].setPosition(2830, 570);
    coins[63].setPosition(3030, 570);
    coins[64].setPosition(3230, 570);
    coins[65].setPosition(3430, 570);
    coins[66].setPosition(3630, 570);
    coins[67].setPosition(3830, 570);
    coins[68].setPosition(4030, 570);
    coins[69].setPosition(4230, 570);
    coins[70].setPosition(4430, 570);
    coins[71].setPosition(4630, 570);
    coins[72].setPosition(4830, 570);
    coins[73].setPosition(5030, 570);
    coins[74].setPosition(5230, 570);
    coins[75].setPosition(430, 570);
    coins[76].setPosition(630, 570);
    coins[77].setPosition(830, 570);
    coins[78].setPosition(1030, 570);
    coins[79].setPosition(1230, 570);
    coins[80].setPosition(1430, 570);
    coins[81].setPosition(1630, 570);
    coins[82].setPosition(1830, 570);
    coins[83].setPosition(2030, 570);
    coins[84].setPosition(2230, 570);
    coins[85].setPosition(2430, 570);
    coins[86].setPosition(2630, 570);
    coins[87].setPosition(2830, 570);
    coins[88].setPosition(3030, 570);
    coins[89].setPosition(3230, 570);
    coins[90].setPosition(3430, 570);
    coins[91].setPosition(3630, 570);
    coins[92].setPosition(3830, 570);
    coins[93].setPosition(4030, 570);
    coins[94].setPosition(4230, 570);
    coins[95].setPosition(4430, 570);
    coins[96].setPosition(4630, 570);
    coins[97].setPosition(4830, 570);
    coins[98].setPosition(5030, 570);
    coins[99].setPosition(5230, 570);
}
void block(Sprite ground1[])
{
    ground1[0].setPosition(560, 300);
    ground1[0].setScale(1.2f, 1.5f);    //small
    ground1[1].setPosition(1100, 350);
    ground1[1].setScale(1.2f, 1.5f);    //small
    ground1[2].setPosition(1700, 420);
    ground1[2].setScale(1.2f, 1.5f);    //small
    ground1[3].setPosition(2300, 300);
    ground1[3].setScale(1.5f, 1.5f);
    ground1[4].setPosition(2900, 370);
    ground1[4].setScale(1.5f, 1.5f);
    ground1[5].setPosition(3500, 300);
    ground1[5].setScale(1.2f, 1.5f);    //small
    ground1[6].setPosition(4100, 400);
    ground1[6].setScale(1.5f, 1.5f);
    ground1[7].setPosition(4700, 300);
    ground1[7].setScale(1.5f, 1.5f);
    ground1[8].setPosition(5300, 350);
    ground1[8].setScale(1.5f, 1.5f);
    ground1[9].setPosition(5900, 400);
    ground1[9].setScale(1.5f, 1.5f);
    ground1[10].setPosition(6500, 300);
    ground1[10].setScale(1.5f, 1.5f);
    ground1[11].setPosition(7100, 350);
    ground1[11].setScale(1.5f, 1.5f);
    ground1[12].setPosition(7700, 400);
    ground1[12].setScale(1.2f, 1.5f);     //small
    ground1[13].setPosition(8300, 315);
    ground1[13].setScale(1.5f, 1.5f);
    ground1[14].setPosition(8900, 350);
    ground1[14].setScale(1.5f, 1.5f);
    ground1[15].setPosition(9500, 400);
    ground1[15].setScale(1.5f, 1.5f);
    ground1[16].setPosition(10100, 350);
    ground1[16].setScale(1.2f, 1.5f);     //small
    ground1[17].setPosition(10700, 380);
    ground1[17].setScale(1.5f, 1.5f);
    ground1[18].setPosition(11300, 420);
    ground1[18].setScale(1.5f, 1.5f);
    ground1[19].setPosition(11900, 300);
    ground1[19].setScale(1.5f, 1.5f);
    ground1[20].setPosition(12500, 400);
    ground1[20].setScale(1.2f, 1.5f);         //small
    ground1[21].setPosition(13100, 350);
    ground1[21].setScale(1.5f, 1.5f);
    ground1[22].setPosition(13700, 300);
    ground1[22].setScale(1.2f, 1.5f);   //small
}
void playername(RenderWindow& window, RenderWindow& gameplay, string& name)
{
    if (!name.empty())
    {
        name.clear();
    }


    Texture bacgroundmenu;
    bacgroundmenu.loadFromFile("Textures/name.jpg");  // load soora lel window
    Sprite bgmenu;
    bgmenu.setTexture(bacgroundmenu);
    Font font;
    font.loadFromFile("Fonts/NiseSegaSonic.TTF"); // load font lel window 
    Text t1;
    Text t2;    // name 
    t1.setFont(font);
    t2.setFont(font);
    t1.setString("Enter your name : ");
    t1.setCharacterSize(70);
    t1.setOutlineColor(Color::Black);
    t1.setOutlineThickness(4);
    t2.setCharacterSize(70);
    t2.setOutlineColor(Color::Black);
    t2.setOutlineThickness(3);
    t1.setPosition(50, 250);
    t2.setPosition(350, 350);
    t1.setFillColor(Color::White);
    t2.setFillColor(Color::White);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            if (event.type == Event::TextEntered)

            {
                name += static_cast<char>(event.text.unicode);  //hatzawd 3ala el stirng el letter eli ana bada5alo
            }
            if (Keyboard::isKeyPressed(Keyboard::BackSpace) && name.size() > 0)
            {
                name.resize(name.size() - 1);

            }
            if (Keyboard::isKeyPressed(Keyboard::Return) && name.size() > 1)
            {
                window.close();
                return;
            }
            if (Keyboard::isKeyPressed(Keyboard::Escape))
            {
                window.close();
                gameplay.close();
                return;
            }

        }
        t2.setString(name);
        window.clear();
        window.draw(bgmenu);
        window.draw(t1);
        window.draw(t2);
        window.display();

    }


}
void History(RenderWindow& window) {

    Texture HistoryTex;
    HistoryTex.loadFromFile("Textures/history.png");
    Sprite HistorySprite[5];
    for (int i = 0; i < 5; ++i)
    {
        HistorySprite[i].setTexture(HistoryTex);
        HistorySprite[i].setPosition(0, 1080 * i);
    }



    Font font1;
    font1.loadFromFile("Fonts/NiseSegaSonic.TTF");

    Text text[100];
    for (int i = 0; i < 90; i++)
    {
        text[i].setFont(font1);
        text[i].setCharacterSize(50);
        text[i].setFillColor(Color::White);
    }

    ifstream infile;
    infile.open("History.txt", ios::in);

    vector<string> lines;
    string line;
    while (getline(infile, line, '*')) {
        lines.push_back(line);
    }
    for (int i = 0; i < lines.size(); i++)
    {
        text[i].setString(lines[i]);
        text[i].setPosition(50, 100 * i);
    }

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();

            }
            if (Keyboard::isKeyPressed(Keyboard::Key::Up))
            {
                if (text[0].getPosition().y <= 5) {
                    for (int i = 0; i < lines.size() + 10; i++) {
                        text[i].move(0, 20);
                    }
                }

            }
            if (Keyboard::isKeyPressed(Keyboard::Key::Down)) {
                if (text[(lines.size()) - 1].getPosition().y >= window.getPosition().y + 1000) {
                    for (int i = 0; i < lines.size() + 10; i++) {
                        text[i].move(0, -20);
                    }
                }
            }
            cout << lines.size() << endl;
        }
        window.clear();
        for (int i = 0; i < 5; i++)
        {
            window.draw(HistorySprite[i]);
        }

        for (int i = 0; i < lines.size(); i++) {
            window.draw(text[i]);
        }

        window.display();
    }


}
void gameOver(RenderWindow& window, int score) {


    Texture HistoryTex;
    HistoryTex.loadFromFile("Textures/gameover.jpg");
    Sprite HistorySprit;
    HistorySprit.setTexture(HistoryTex);
    HistorySprit.setPosition(0, 0);

    Texture Button;
    Button.loadFromFile("Textures/Button.png");
    Sprite Button1;
    Button1.setTexture(Button);
    Button1.setPosition(175, 485);
    Button1.setScale(5, 5);

    Font font22;
    font22.loadFromFile("Fonts/NiseSegaSonic.TTF");
    Text GameoverF;
    GameoverF.setFont(font22);
    GameoverF.setString("Main menu ");
    GameoverF.setPosition(250, 500);
    GameoverF.setScale(1.2, 1.2);
    GameoverF.setFillColor(Color::White);
    GameoverF.setOutlineColor(Color::Black);
    GameoverF.setOutlineThickness(2);

    Font font1;
    font1.loadFromFile("Fonts/NiseSegaSonic.TTF");
    Text Gameover;
    Gameover.setFont(font1);
    Gameover.setString("Game Over ");
    Gameover.setPosition(100, 100);
    Gameover.setScale(2.5, 2.5);
    Gameover.setFillColor(Color::White);
    Gameover.setOutlineColor(Color::Black);
    Gameover.setOutlineThickness(4);

    Font font2;
    font2.loadFromFile("Fonts/NiseSegaSonic.TTF");
    Text GameoverScore;
    string ScoreString = to_string(score);

    GameoverScore.setFont(font2);
    GameoverScore.setString("Score : " + ScoreString);
    GameoverScore.setPosition(200, 235);
    GameoverScore.setScale(1.7, 1.7);
    GameoverScore.setFillColor(Color::White);
    GameoverScore.setOutlineColor(Color::Black);
    GameoverScore.setOutlineThickness(4);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed()) {
                window.close();

            }
            if (Keyboard::isKeyPressed(Keyboard::Escape)) {

                return;
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
            {
                return;
            }
        }
        window.clear();
        window.draw(HistorySprit);
        window.draw(GameoverScore);
        window.draw(Gameover);
        window.draw(Button1);
        window.draw(GameoverF);
        window.display();
    }
}
void pressEnter(RenderWindow& window)
{
    RectangleShape bg;
    bg.setSize(Vector2f(1920, 1080));
    bg.setScale(1, 1);
    bg.setPosition(0, 0);
    Texture mainbg;
    mainbg.loadFromFile("Textures/lapper-bggreenhill1.jpg");
    bg.setTexture(&mainbg);


    RectangleShape sonicabg;
    sonicabg.setSize(Vector2f(795, 475));
    sonicabg.setScale(1, 1);
    sonicabg.setPosition(505, 190);
    Texture Smainbg;
    Smainbg.loadFromFile("Textures/bgsonic.png");
    sonicabg.setTexture(&Smainbg);

    Font font;
    font.loadFromFile("Fonts/NiseSegaSonic.TTF"); // load font lel window 
    Text t1;
    t1.setFont(font);
    t1.setString("Press Enter");
    t1.setCharacterSize(40);
    t1.setOutlineColor(Color::Black);
    t1.setOutlineThickness(4);
    t1.setPosition(765, 750);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {

            if (Keyboard::isKeyPressed(Keyboard::Return))
            {
                window.close();
                return;
            }


        }
        window.clear();
        window.draw(bg);
        window.draw(sonicabg);
        window.draw(t1);
        window.display();

    }

}
void GamePlay(RenderWindow& window, bool& level1isfinished) {
    srand(static_cast<unsigned>(time(NULL)));
    Clock timerAdd, timerDelete, gametime;

    //adding score,time,rings
    Texture scoreimagetexture;
    scoreimagetexture.loadFromFile("Textures/scoreimage.png");
    Sprite scoreimage[5];
    for (int i = 0; i < 3; i++) {
        scoreimage[i].setTexture(scoreimagetexture);
        scoreimage[i].setScale(3.5f, 3.5f);
    }
    scoreimage[0].setTextureRect(IntRect(0, 0, 50, 55));
    scoreimage[1].setTextureRect(IntRect(0, 56, 40, 20));
    scoreimage[2].setTextureRect(IntRect(88, 0, 10, 25));


    //declaring sonic
    Texture sonictexture;
    sonictexture.loadFromFile("Textures/approvedsonic.png");
    player sonic;
    sonic.sprite.setTextureRect(IntRect(0, 0, 50, 55));
    sonic.sprite.setScale(2.3, 2.3);
    sonic.sp(sonictexture);


    //declaring enemy1
    Texture enemytexture;
    enemytexture.loadFromFile("Textures/crabenemy.png");
    Enemy enemy;
    enemy.sprite.setTexture(enemytexture);
    enemy.speed = 3;
    enemy.sprite.setPosition(1700, 580);
    enemy.sprite.setTextureRect(IntRect(0, 0, 51, 40));
    enemy.sprite.setScale(-3.3, 3.3);


    //declaring enemy2
    Texture enemytexture2;
    enemytexture2.loadFromFile("Textures/batenemy.png");
    Enemy enemy2[2];
    for (int i = 0; i < 2; i++) {
        enemy2[i].sprite.setTexture(enemytexture2);
        enemy2[i].speed = 7.5;
        enemy2[i].sprite.setTextureRect(IntRect(0, 0, 36, 48));
    }
    enemy2[0].sprite.setPosition(1700, 80);
    enemy2[1].sprite.setPosition(-400, 120);
    enemy2[0].sprite.setScale(-2.9, 2.9);
    enemy2[1].sprite.setScale(2.9, 2.9);

    
    //setting ground plants
    Texture groundtextureP;
    groundtextureP.loadFromFile("Textures/maplevel1.png");
    Sprite ground2P[18];
    for (int i = 0; i < 9; i++)
    {
        ground2P[i].setTexture(groundtextureP);
        ground2P[i].setPosition(Vector2f((i * 1800), 385));
        ground2P[i].setScale(1.8, 1.5);
        ground2P[i].setOrigin(200, 0);
    }


    //setting spikes
    Texture spikes;
    spikes.loadFromFile("Textures/spikes2.png");
    Sprite spike[18];
    for (int i = 0; i < 8; i++)
    {
        spike[i].setTexture(spikes);
        spike[i].setPosition(Vector2f(((i * 1600)), 597));
        spike[i].setScale(2.7, 2.7);
        spike[i].setOrigin(-500, 0);
    }

    //ending sign
    Texture endtexture;
    endtexture.loadFromFile("Textures/ending.png");
    Sprite end(endtexture);
    end.setScale(2.8f, 2.8f);
    end.setPosition(14200,560);

    //setting ground
    Texture groundtexture;
    groundtexture.loadFromFile("Textures/map.png");
    Sprite ground[5];
    for (int i = 0; i < 2; i++)
    {
        ground[i].setTexture(groundtexture);
        ground[i].setPosition(Vector2f(i * 5408, 700));
        ground[i].setScale(2, 2);
        ground[i].setOrigin(200, 0);
    }

    
    //setting blocks
    Texture ground1texture;
    ground1texture.loadFromFile("Textures/block89.png");
    Sprite ground1[23];
    block(ground1);
    for (int i = 0; i < 23; ++i)
    {
        ground1[i].setTexture(ground1texture);
    }


    //Variables
    int coinCount = 0;
    float coinAnimationIndicator = 0;
    bool isCoinVisible = true;
    bool landed = false;
    double velocityY = 0;
    score = 0;
    int rings = 0;


    //creating coins 
    Texture coinsTextures;
    coinsTextures.loadFromFile("Textures/coinsprite.png");
    Sprite coins[100];
    coin(coins);
    for (int i = 0; i < 100; i++) {
        coins[i].setTexture(coinsTextures);
        coins[i].setTextureRect(IntRect(0, 0, 64, 64));
        coins[i].setScale(0.8f, 0.8f);
    }

    //background 1
    Texture backgroundtexture;
    backgroundtexture.loadFromFile("Textures/lapper-bggreenhill1.jpg");
    Sprite background[18];
    for (int i = 0; i < 18; ++i)
    {
        background[i].setTexture(backgroundtexture);
        background[i].setPosition(Vector2f((i * 1920), -50));
        background[i].setOrigin(400, 0);
        background[i].setScale(1, 1.17);
    }

    //Score
    Font font;
    font.loadFromFile("Fonts/sonic-hud-font.ttf");
    Text text;
    Text text2;
    text.setFont(font);
    text2.setFont(font);
    text.setString(to_string(score));
    text2.setString(to_string(rings));
    text.setFillColor(Color::White);
    text2.setFillColor(Color::White);
    text.setOutlineColor(Color::Black);
    text2.setOutlineColor(Color::Black);
    text.setOutlineThickness(2);
    text2.setOutlineThickness(2);
    text.setPosition(70, 65);
    text.setCharacterSize(32);
    text2.setCharacterSize(32);
    text.setScale(1.45f, 1.45f);
    text2.setScale(1.45f, 1.45f);

    //Declare a Text object for the timer
    Text timerText("", font, 50);
    timerText.setFillColor(Color::White);
    timerText.setOutlineColor(Color::Black);
    timerText.setOutlineThickness(2);
    timerText.setPosition(350, 250);
    timerText.setCharacterSize(32);
    timerText.setScale(1.45f, 1.45f);

    //2D camera
    View view(Vector2f(0, 0), Vector2f(1920, 1080));
    view.setCenter(sonic.sprite.getPosition() - Vector2f(200.0f, 0)); //update
    window.setView(view);

    //powerups
    Setdrops();

    // load the sound 
    SoundBuffer coinBuffer;
    coinBuffer.loadFromFile("Sounds/coinsound.wav");
    Sound coinSound(coinBuffer);

    //background 2 
    Texture backgroundtexture2;
    backgroundtexture2.loadFromFile("Textures/lapper-bggreenhill2.jpg");
    Sprite background2[18];
    for (int i = 0; i < 18; ++i)
    {
        background2[i].setTexture(backgroundtexture2);
        background2[i].setPosition(Vector2f((i * 1920), -50));
        background2[i].setOrigin(200, 0);
        background2[i].setScale(1, 1.17);
    }

    //colliosion cooldown
    Clock cooldowndamage;
    float cooldownTime = 5.5f;
    bool candamage = true;

    while (window.isOpen())
    {
        //setting time 
        Clock clock, cooldown;
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

        Event aevent;
        while (window.pollEvent(aevent))
        {
            if (aevent.type == Event::Closed)
            {
                window.close();
            }
            if (aevent.type == Event::KeyPressed)
            {
                if (aevent.key.code == Keyboard::Escape)
                {
                    window.close();
                }
            }
        }

        // Move the player using A,D and space keys
        if (sonic.last_key_pressed == 1) {
            sonic.sprite.setTextureRect(IntRect(0, 0, 50, 55));
            sonic.sprite.setScale(2.3, 2.3);
        }
        if (sonic.last_key_pressed == 2) {
            sonic.sprite.setTextureRect(IntRect(0, 0, 50, 55));
            sonic.sprite.setScale(-2.3, 2.3);
        }
        if (Keyboard::isKeyPressed(Keyboard::A)) {
            sonic.sprite.setScale(-2.3, 2.3);
            sonic.last_key_pressed = 2;
        }
        if (Keyboard::isKeyPressed(Keyboard::D)) {
            sonic.sprite.setScale(2.3, 2.3);
            sonic.last_key_pressed = 1;
        }
        if (Mouse::isButtonPressed(Mouse::Left) && sonic.index >= 0 && sonic.canShoot) {
            sonic.bullet[sonic.index].bulletSprite.setPosition(sonic.sprite.getPosition().x, sonic.sprite.getPosition().y);
            sonic.shootCooldown = sonic.bullet[sonic.index].cooldownUse;
            sonic.bullet[sonic.index].moveTo = sonic.last_key_pressed;
            sonic.index--;
            sonic.canShoot = 0;

        }
        //setting position for score
        text.setPosition(sonic.sprite.getPosition().x - 100, 48);
        text2.setPosition(sonic.sprite.getPosition().x - 100, 155);
        timerText.setPosition(sonic.sprite.getPosition().x - 100, 105);
        scoreimage[0].setPosition(sonic.sprite.getPosition().x - 180 - 104, 25);
        scoreimage[1].setPosition(sonic.sprite.getPosition().x - 180 - 104, 880);
        scoreimage[2].setPosition(sonic.sprite.getPosition().x - 50 - 104, 867);


        //collision between sonic and spikes
        for (int i = 0; i < 18; i++) {
            if (sonic.sprite.getGlobalBounds().intersects(spike[i].getGlobalBounds()))
            {

                if (candamage && cooldowndamage.getElapsedTime().asSeconds() >= cooldownTime)
                {

                    sonic.damage++;
                    sonic.sprite.move(-450, -150);

                    cooldowndamage.restart();
                    candamage = false;
                }

                if (!candamage && cooldowndamage.getElapsedTime().asSeconds() >= cooldownTime)

                {
                    candamage = true;
                }


            }
        }

        //collision between sonic and enemy
        if (sonic.sprite.getGlobalBounds().intersects(enemy.sprite.getGlobalBounds()))
        {
            sonic.damage++;
            enemy.sprite.setPosition(sonic.sprite.getPosition().x + 2500, 580); // Respawn the enemy on the right side of the window
        }
        if (enemy.sprite.getPosition().x < (sonic.sprite.getPosition().x - 1000))
        {
            enemy.sprite.setPosition(sonic.sprite.getPosition().x + 2500, 580); // Respawn the enemy on the right side of the window
        }

        //collision between sonic and enemy2
        if (sonic.sprite.getGlobalBounds().intersects(enemy2[0].sprite.getGlobalBounds()))
        {
            enemy2[0].sprite.setPosition(sonic.sprite.getPosition().x + 2500, 80); // Respawn the enemy2 on the right side of the window
            sonic.damage++;
        }
        if (enemy2[0].sprite.getPosition().x < (sonic.sprite.getPosition().x - 1000))
        {
            enemy2[0].sprite.setPosition(sonic.sprite.getPosition().x + 2500, 80); // Respawn the enemy2 on the right side of the window
        }
        if (sonic.sprite.getGlobalBounds().intersects(enemy2[1].sprite.getGlobalBounds()))
        {
            enemy2[1].sprite.setPosition(sonic.sprite.getPosition().x + 2500, 120); // Respawn the enemy2 on the right side of the window
            sonic.damage++;
        }
        if (enemy2[1].sprite.getPosition().x > (sonic.sprite.getPosition().x + 1650))
        {
            enemy2[1].sprite.setPosition(sonic.sprite.getPosition().x - 1000, 120); // Respawn the enemy2 on the right side of the window
        }

        //collision between bullets and enemy
        for (int j = 0; j < sonic.bullet.size(); j++)
        {
            if (sonic.bullet[j].bulletSprite.getGlobalBounds().intersects(enemy.sprite.getGlobalBounds())) {
                enemy.health -= 19;
                sonic.bullet[j].bulletSprite.setScale(0, 0);
            }
            if (enemy.health == 0) {
                enemy.sprite.setPosition(sonic.sprite.getPosition().x + 2500, 580);
                score += 5;
                text.setString(to_string(score));
                enemy.health = 3 * 19;
            }
        }

        //sonic limits
        if (sonic.sprite.getPosition().x < 0) {
            sonic.sprite.setPosition(0, sonic.sprite.getPosition().y);
        }
        if (sonic.sprite.getPosition().y < 0) {
            sonic.sprite.setPosition(sonic.sprite.getPosition().x, 0);
        }
        if (sonic.sprite.getPosition().x > 14200) {
            sonic.sprite.setPosition(14200, sonic.sprite.getPosition().y);
        }

        //animation of coins
        for (int i = 0; i < 100; i++) {
            coinAnimationIndicator += 0.018;
            if (coinAnimationIndicator > 9)
                coinAnimationIndicator -= 9;
            coins[i].setTextureRect(IntRect(int(coinAnimationIndicator) * 64, 0, 64, 64));
        }

        //incrementing scoring
        for (int i = 0; i < 100; i++) {
            if (sonic.sprite.getGlobalBounds().intersects(coins[i].getGlobalBounds())) {
                coinSound.play(); // Play the sound effect
                coins[i].setScale(0, 0);
                rings++;
                text2.setString(to_string(rings));
            }
        }

        //Updating sonic
        view.setCenter(Vector2f(sonic.sprite.getPosition().x + 648, 540));
        sonic.update(time, 1.0f / 40.f, ground1);

        //enemy animation
        enemy.sprite.move(-enemy.speed, 0);
        enemy.sprite.setTextureRect(IntRect(int(enemy.animation) * 52, 0, 52, 40));
        enemy.animation += 0.1;
        if (enemy.animation > 4)
            enemy.animation = 0;

        //enemy2 animation
        for (int i = 0; i < 2; i++) {
            enemy2[i].sprite.setTextureRect(IntRect(int(enemy.animation) * 36, 0, 36, 48));
            enemy2[i].animation += 0.1;
        }
        enemy2[0].sprite.move(-enemy2[0].speed, 0);
        enemy2[1].sprite.move(enemy2[1].speed, 0);
        if (enemy.animation > 3)
            enemy.animation = 0;

        Time elapsedTime = gametime.getElapsedTime();
        int totalSeconds = static_cast<int>(elapsedTime.asSeconds());
        int minutes = (totalSeconds % 3600) / 60;
        int seconds = totalSeconds % 60;

        // Format the time as a string
        string timeString = to_string(minutes) + "'" + to_string(seconds).substr(0, 2);
        timerText.setString(timeString); // Set the text string

        //Upating history after checking if sonic is dead or not
        if (sonic.damage < 3)
        {
            scoreimage[2].setTextureRect(IntRect(88 - (10 * sonic.damage), 0, 10, 25));
        }
        else
        {
            ofstream offile;
            offile.open("history.txt", ios::app);
            offile << name << "   " << score << "   " << timeString << "   " << rings << "*" << endl;
            gameover = true;
            break;
        }

        //checking if the level is finished
        if (sonic.sprite.getGlobalBounds().intersects(end.getGlobalBounds())) {
            sonic.sprite.move(-30, 0);
            sonic.velocity.x = 0;
            level1isfinished = true;
            break;
        }

        window.clear();
        window.setView(view);
        for (int i = 0; i < 18; ++i)
        {
            window.draw(background[i]);
        }
        for (int i = 0; i < 8; i++)
        {
            window.draw(ground2P[i]);
        }

        for (int i = 0; i < 10; i++) {
            window.draw(coins[i]);
        }

        for (int i = 0; i < 8; i++)
        {
            window.draw(spike[i]);
        }
        window.draw(enemy.sprite);
        for (int i = 0; i < 2; i++) {
            window.draw(enemy2[i].sprite);
        }
        for (int i = 0; i < 5; i++)
        {
            window.draw(ground[i]);
        }
        for (int i = 0; i < sonic.bullet.size(); i++)
        {
            window.draw(sonic.bullet[i].bulletSprite);
        }

        for (int i = 0; i < dropBag.size(); i++) {
            window.draw(dropBag[i].dropShape);
        }

        window.draw(sonic.sprite);
        for (int i = 0; i < 23; i++) {
            window.draw(ground1[i]);
        }
        window.draw(end);
        window.draw(text);
        window.draw(text2);
        window.draw(timerText);
        for (int i = 0; i < 3; i++) {
            window.draw(scoreimage[i]);
        }
        window.display();
    }
}
void GamePlay2(RenderWindow& window) {
    srand(static_cast<unsigned>(time(NULL)));
    Clock timerAdd, timerDelete, gametime;

    //adding score,time,rings
    Texture scoreimagetexture;
    scoreimagetexture.loadFromFile("Textures/scoreimage.png");
    Sprite scoreimage[5];
    for (int i = 0; i < 3; i++) {
        scoreimage[i].setTexture(scoreimagetexture);
        scoreimage[i].setScale(3.5f, 3.5f);
    }
    scoreimage[0].setTextureRect(IntRect(0, 0, 50, 55));
    scoreimage[1].setTextureRect(IntRect(0, 56, 40, 20));
    scoreimage[2].setTextureRect(IntRect(88, 0, 10, 25));


    //declaring sonic
    Texture sonictexture;
    sonictexture.loadFromFile("Textures/approvedsonic.png");
    player sonic;
    sonic.sprite.setTextureRect(IntRect(0, 0, 50, 55));
    sonic.sprite.setScale(2.3, 2.3);
    sonic.sp(sonictexture);

    //ending sign
    Texture endtexture;
    endtexture.loadFromFile("Textures/ending.png");
    Sprite end(endtexture);
    end.setScale(2.8f, 2.8f);
    end.setPosition(14200, 560);


    //declaring enemy1
    Texture enemytexture;
    enemytexture.loadFromFile("Textures/beetle.png");
    Enemy enemy[7];
    for (int i = 0; i < 7; i++){
    enemy[i].sprite.setTexture(enemytexture);
    enemy[i].speed = 3;
    enemy[i].sprite.setTextureRect(IntRect(0, 0, 45, 35));
    enemy[i].sprite.setScale(-3.3, 3.3);
    enemy[i].sprite.setPosition((enemy[i].posX) + (i * 2000.0f), 608);
    enemy[i].sprite.setOrigin(21, 0);
    }
    enemy[1].posX = 2700;
    enemy[2].posX = 4700;
    enemy[3].posX = 6700;
    enemy[4].posX = 8700;
    enemy[5].posX = 10700;
    enemy[6].posX = 12700;
    
    //declaring enemy2
    Texture enemytexture2;
    enemytexture2.loadFromFile("Textures/batenemy.png");
    Enemy enemy2[2];
    for (int i = 0; i < 2; i++) {
        enemy2[i].sprite.setTexture(enemytexture2);
        enemy2[i].speed = 7.5;
        enemy2[i].sprite.setTextureRect(IntRect(0, 0, 36, 48));
    }
    enemy2[0].sprite.setPosition(1700, 80);
    enemy2[1].sprite.setPosition(-400, 120);
    enemy2[0].sprite.setScale(-2.9, 2.9);
    enemy2[1].sprite.setScale(2.9, 2.9);




    //setting background image
    Texture backgroundimagetexture;
    backgroundimagetexture.loadFromFile("Textures/map2top.png");
    Sprite backgroundimage[18];
    for (int i = 0; i < 18; ++i)
    {
        backgroundimage[i].setTexture(backgroundimagetexture);
        backgroundimage[i].setPosition(2400*i, 210);
        backgroundimage[i].setScale(2, 2);
        backgroundimage[i].setOrigin(500, 0);
    }

   

    //setting fire
    Texture fires;
    fires.loadFromFile("Textures/fire.png");
    Sprite fire[18];

    for (int i = 0; i < 18; i++)
    {
        fire[i].setTexture(fires);
        fire[i].setPosition(Vector2f(((i * 1600)), 637));
        fire[i].setScale(2.7, 2.7);
        fire[i].setOrigin(-500, 0);
    }


    //setting ground2
    Texture groundtexture2;
    groundtexture2.loadFromFile("Textures/map2.png");
    Sprite ground2[20];
    for (int i = 0; i < 14; i++)
    {
        ground2[i].setTexture(groundtexture2);
        ground2[i].setPosition(Vector2f((i * 1088), 695));
        ground2[i].setScale(2, 3.1);
        ground2[i].setOrigin(200, 0);
    }

    //setting blocks
    Texture ground1texture2;
    ground1texture2.loadFromFile("Textures/oakground.png");
    Sprite ground1[23];
    block(ground1);
    for (int i = 0; i < 23; ++i)
    {
        ground1[i].setTexture(ground1texture2);
    }

    //Variables
    int coinCount = 0;
    float coinAnimationIndicator = 0;
    bool isCoinVisible = true;
    bool landed = false;
    double velocityY = 0;
    score = 0;
    int rings = 0;

    //creating coins 
    Texture coinsTextures;
    coinsTextures.loadFromFile("Textures/coinsprite.png");
    Sprite coins[100];
    coin(coins);
    for (int i = 0; i < 100; i++) {
        coins[i].setTexture(coinsTextures);
        coins[i].setTextureRect(IntRect(0, 0, 64, 64));
        coins[i].setScale(0.8f, 0.8f);
    }

    //Score
    Font font;
    font.loadFromFile("Fonts/sonic-hud-font.ttf");      
    Text text;
    Text text2;
    text.setFont(font);
    text2.setFont(font);
    text.setString(to_string(score));
    text2.setString(to_string(rings));
    text.setFillColor(Color::White);
    text2.setFillColor(Color::White);
    text.setOutlineColor(Color::Black);
    text2.setOutlineColor(Color::Black);
    text.setOutlineThickness(2);
    text2.setOutlineThickness(2);
    text.setPosition(70, 65);
    text.setCharacterSize(32);
    text2.setCharacterSize(32);
    text.setScale(1.45f, 1.45f);
    text2.setScale(1.45f, 1.45f);

    // Declare a Text object for the timer
    Text timerText("", font, 50);
    timerText.setFillColor(Color::White);
    timerText.setOutlineColor(Color::Black);
    timerText.setOutlineThickness(2);
    timerText.setPosition(350, 250);
    timerText.setCharacterSize(32);
    timerText.setScale(1.45f, 1.45f);

    //2D camera
    View view(Vector2f(0, 0), Vector2f(1920, 1080));
    view.setCenter(sonic.sprite.getPosition() - Vector2f(200.0f, 0)); //update
    window.setView(view);

    //powerups
    Setdrops();

    // load the sound 
    SoundBuffer coinBuffer;
    coinBuffer.loadFromFile("Sounds/coinsound.wav");
    Sound coinSound(coinBuffer);

    //background 2 
    Texture backgroundtexture2;
    backgroundtexture2.loadFromFile("Textures/lapper-bggreenhill2.jpg");
    Sprite background2[18];
    for (int i = 0; i < 18; ++i)
    {
        background2[i].setTexture(backgroundtexture2);
        background2[i].setPosition(Vector2f((i * 1920) - 200, -50));
        background2[i].setOrigin(200, 0);
        background2[i].setScale(1, 1.17);
    }

    //colliosion cooldown
    Clock cooldowndamage;
    float cooldownTime = 5.5f;
    bool candamage = true;

    while (window.isOpen())
    {
        //setting time 
        Clock clock, cooldown;
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

        Event aevent;
        while (window.pollEvent(aevent))
        {
            if (aevent.type == Event::Closed)
            {
                window.close();
            }
            if (aevent.type == Event::KeyPressed)
            {
                if (aevent.key.code == Keyboard::Escape)
                {
                    window.close();
                }
            }
        }

        // Move the player using A,D and space keys
        if (sonic.last_key_pressed == 1) {
            sonic.sprite.setTextureRect(IntRect(0, 0, 50, 55));
            sonic.sprite.setScale(2.3, 2.3);
        }
        if (sonic.last_key_pressed == 2) {
            sonic.sprite.setTextureRect(IntRect(0, 0, 50, 55));
            sonic.sprite.setScale(-2.3, 2.3);
        }
        if (Keyboard::isKeyPressed(Keyboard::A)) {
            sonic.last_key_pressed = 2;
            sonic.sprite.setScale(-2.3, 2.3);
        }
        if (Keyboard::isKeyPressed(Keyboard::D)) {
            sonic.last_key_pressed = 1;
            sonic.sprite.setScale(2.3, 2.3);
        }
        if (Mouse::isButtonPressed(Mouse::Left) && sonic.index >= 0 && sonic.canShoot) {
            sonic.bullet[sonic.index].bulletSprite.setPosition(sonic.sprite.getPosition().x, sonic.sprite.getPosition().y);
            sonic.shootCooldown = sonic.bullet[sonic.index].cooldownUse;
            sonic.bullet[sonic.index].moveTo = sonic.last_key_pressed;
            sonic.index--;
            sonic.canShoot = 0;
        }
        //setting position for score
        text.setPosition(sonic.sprite.getPosition().x - 100, 48);
        text2.setPosition(sonic.sprite.getPosition().x - 100, 155);
        timerText.setPosition(sonic.sprite.getPosition().x - 100, 105);
        scoreimage[0].setPosition(sonic.sprite.getPosition().x - 180 - 104, 25);
        scoreimage[1].setPosition(sonic.sprite.getPosition().x - 180 - 104, 880);
        scoreimage[2].setPosition(sonic.sprite.getPosition().x - 50 - 104, 867);


        //collision between sonic and fire
        for (int i = 0; i < 18; i++) {
            if (sonic.sprite.getGlobalBounds().intersects(fire[i].getGlobalBounds()))
            {
                if (candamage && cooldowndamage.getElapsedTime().asSeconds() >= cooldownTime)
                {
                    sonic.damage++;
                    sonic.sprite.move(-450, -150);
                    cooldowndamage.restart();
                    candamage = false;
                }
                if (!candamage && cooldowndamage.getElapsedTime().asSeconds() >= cooldownTime)
                {
                    candamage = true;
                }
            }
        }

        //collision between sonic and enemy
        for (int i = 0; i < 5; i++) {
            if (sonic.sprite.getGlobalBounds().intersects(enemy[i].sprite.getGlobalBounds()))
            {
                if (candamage && cooldowndamage.getElapsedTime().asSeconds() >= cooldownTime)
                {
                    sonic.damage++;
                    sonic.sprite.move(-100, -150);

                    cooldowndamage.restart();
                    candamage = false;
                }

                if (!candamage && cooldowndamage.getElapsedTime().asSeconds() >= cooldownTime)

                {
                    candamage = true;
                }
            }

        }


        //collision between sonic and enemy2
        if (sonic.sprite.getGlobalBounds().intersects(enemy2[0].sprite.getGlobalBounds()))
        {
            enemy2[0].sprite.setPosition(sonic.sprite.getPosition().x + 2500, 80); // Respawn the enemy2 on the right side of the window
            sonic.damage++;
        }
        if (enemy2[0].sprite.getPosition().x < (sonic.sprite.getPosition().x - 1000))
        {
            enemy2[0].sprite.setPosition(sonic.sprite.getPosition().x + 2500, 80); // Respawn the enemy2 on the right side of the window
        }
        if (sonic.sprite.getGlobalBounds().intersects(enemy2[1].sprite.getGlobalBounds()))
        {
            enemy2[1].sprite.setPosition(sonic.sprite.getPosition().x + 2500, 120); // Respawn the enemy2 on the right side of the window
            sonic.damage++;
        }
        if (enemy2[1].sprite.getPosition().x > (sonic.sprite.getPosition().x + 1650))
        {
            enemy2[1].sprite.setPosition(sonic.sprite.getPosition().x - 1000, 120); // Respawn the enemy2 on the right side of the window
        }

        //collision between bullets and enemy
        for (int i = 0; i < 5; i++){
            for (int j = 0; j < sonic.bullet.size(); j++)
            {
                if (sonic.bullet[j].bulletSprite.getGlobalBounds().intersects(enemy[i].sprite.getGlobalBounds())) {
                    enemy[i].health -= 19;
                    sonic.bullet[j].bulletSprite.setScale(0, 0);
                }
                if (enemy[i].health == 0) {
                    enemy[i].sprite.setPosition(-10000,0);
                    score += 5;
                    text.setString(to_string(score));
                    enemy[i].health = 3 * 19;
                }
            }
        }

        //sonic limits
        if (sonic.sprite.getPosition().x < 0) {
            sonic.sprite.setPosition(0, sonic.sprite.getPosition().y);
        }
        if (sonic.sprite.getPosition().y < 0) {
            sonic.sprite.setPosition(sonic.sprite.getPosition().x, 0);
        }
        if (sonic.sprite.getPosition().x > 14200) {
            sonic.sprite.setPosition(14200, sonic.sprite.getPosition().y);
        }

        //animation of coins
        for (int i = 0; i < 100; i++) {
            coinAnimationIndicator += 0.018;
            if (coinAnimationIndicator > 9)
                coinAnimationIndicator -= 9;
            coins[i].setTextureRect(IntRect(int(coinAnimationIndicator) * 64, 0, 64, 64));
        }

        //incrementing scoring
        for (int i = 0; i < 100; i++){
            if (sonic.sprite.getGlobalBounds().intersects(coins[i].getGlobalBounds())) {
                coinSound.play(); // Play the sound effect
                coins[i].setScale(0, 0);
                rings++;
                text2.setString(to_string(rings));
            }
        }

        //Updating sonic
        view.setCenter(Vector2f(sonic.sprite.getPosition().x + 648, 540));
        sonic.update(time, 1.0f / 40.f, ground1);

        //enemy animation
        for (int i = 0; i < 5; i++) {
        enemy[i].sprite.setTextureRect(IntRect(int(enemy[i].animation) * 45, 0, 45, 35));
        enemy[i].animation += 0.1;
        if (enemy[i].animation > 1)
            enemy[i].animation = 0;
        }

        for (int i = 0; i < 5; i++) {
            // Update the enemy's position based on movement direction
            if (enemy[i].moveRight) {
                enemy[i].sprite.move(enemy[i].moveSpeed, 0.0f);     
                enemy[i].sprite.setScale(-3.3, 3.3);
            }
            else {
                enemy[i].sprite.move(-enemy[i].moveSpeed, 0.0f);
                enemy[i].sprite.setScale(3.3, 3.3);
            }

            // Check if the enemy exceeds the movement range
            if (enemy[i].sprite.getPosition().x >= enemy[i].posX + enemy[i].moveRange) {
                enemy[i].moveRight = false;
            }
            else if (enemy[i].sprite.getPosition().x <= enemy[i].posX) {
                enemy[i].moveRight = true;
            }
        }

        //enemy2 animation
        for (int i = 0; i < 2; i++) {
            enemy2[i].sprite.setTextureRect(IntRect(int(enemy2[i].animation) * 36, 0, 36, 48));
            enemy2[i].animation += 0.1;
            if (enemy2[i].animation > 3)
                enemy2[i].animation = 0;
        }
        enemy2[0].sprite.move(-enemy2[0].speed, 0);
        enemy2[1].sprite.move(enemy2[1].speed, 0);
        

        Time elapsedTime = gametime.getElapsedTime();
        int totalSeconds = static_cast<int>(elapsedTime.asSeconds());
        int minutes = (totalSeconds % 3600) / 60;
        int seconds = totalSeconds % 60;

        // Format the time as a string
        string timeString = to_string(minutes) + "'" + std::to_string(seconds).substr(0, 2);
        timerText.setString(timeString); // Set the text string

        //Upating history after checking if sonic is dead or not
        if (sonic.damage < 3)
        {
            scoreimage[2].setTextureRect(IntRect(88 - (10 * sonic.damage), 0, 10, 25));
        }
        else
        {
            ofstream offile;
            offile.open("history.txt", ios::app);
            offile << name << "   " << score << "   " << timeString << "   " << rings << "*" << endl;
            gameover = true;
            break;
        }
        sonic.groundHeight = 744;
        sonic.update(time, 1.0f / 40.f, ground1);
        
        //checking if the level is finished
        if (sonic.sprite.getGlobalBounds().intersects(end.getGlobalBounds())) {
            sonic.sprite.move(-30, 0);
            sonic.velocity.x = 0;
            level2isfinished = true;
            break;
        }
        window.clear();
        window.setView(view);
        for (int i = 0; i < 18; i++)
        {
            window.draw(background2[i]);
        }
        for (int i = 0; i < 14; i++)
        {
            window.draw(ground2[i]);
        }
        for (int i = 0; i < 18; i++)
        {
            window.draw(backgroundimage[i]);
        }
        for (int i = 0; i < sonic.bullet.size(); i++)
        {
            window.draw(sonic.bullet[i].bulletSprite);
        }

        for (int i = 0; i < dropBag.size(); i++) {
            window.draw(dropBag[i].dropShape);
        }
        window.draw(sonic.sprite);
        for (int i = 0; i < 23; i++) {
            window.draw(ground1[i]);
        }
        for (int i = 0; i < 100; i++) {
            window.draw(coins[i]);
        }
        for (int i = 0; i < 18; i++)
        {
            window.draw(fire[i]);
        }
       
        for (int i = 0; i < 2; i++) {
            window.draw(enemy2[i].sprite);
        }
        for (int i = 0; i < 7; i++)
        {
            window.draw(enemy[i].sprite);
        }
        window.draw(text);
        window.draw(text2);
        window.draw(timerText);
        window.draw(end);
        for (int i = 0; i < 3; i++) {
            window.draw(scoreimage[i]);
        }
        window.display();
    }
}
void GamePlay3(RenderWindow& window) {
    srand(static_cast<unsigned>(time(NULL)));
    Clock timerAdd, timerDelete, gametime;

    //adding score,time,rings
    Texture scoreimagetexture;
    scoreimagetexture.loadFromFile("Textures/scoreimage.png");
    Sprite scoreimage[5];
    for (int i = 0; i < 3; i++) {
        scoreimage[i].setTexture(scoreimagetexture);
        scoreimage[i].setScale(3.5f, 3.5f);
    }
    scoreimage[0].setTextureRect(IntRect(0, 0, 50, 55));
    scoreimage[1].setTextureRect(IntRect(0, 56, 40, 20));
    scoreimage[2].setTextureRect(IntRect(88, 0, 10, 25));

    //declaring sonic
    Texture sonictexture;
    sonictexture.loadFromFile("Textures/approvedsonic.png");
    player sonic;
    sonic.sprite.setTextureRect(IntRect(0, 0, 50, 55));
    sonic.sprite.setScale(2.3, 2.3);
    sonic.sp(sonictexture);
    sonic.groundHeight = 742;
    
    //ending sign
    Texture endtexture;
    endtexture.loadFromFile("Textures/ending.png");
    Sprite end(endtexture);
    end.setScale(2.8f, 2.8f);
    end.setPosition(14800, 568);


    //declaring enemy1
    Texture enemytexture;
    enemytexture.loadFromFile("Textures/crabenemy.png");
    Enemy enemy;
    enemy.sprite.setTexture(enemytexture);
    enemy.speed = 3;
    enemy.sprite.setPosition(1700, 598);
    enemy.sprite.setTextureRect(IntRect(0, 0, 51, 40));
    enemy.sprite.setScale(-3.3, 3.3);


    //declaring enemy2
    Texture enemytexture2;
    enemytexture2.loadFromFile("Textures/batenemy.png");
    Enemy enemy2[2];
    for (int i = 0; i < 2; i++) {
        enemy2[i].sprite.setTexture(enemytexture2);
        enemy2[i].speed = 7.5;
        enemy2[i].sprite.setTextureRect(IntRect(0, 0, 36, 48));
    }
    enemy2[0].sprite.setPosition(1700, 80);
    enemy2[1].sprite.setPosition(-400, 120);
    enemy2[0].sprite.setScale(-2.9, 2.9);
    enemy2[1].sprite.setScale(2.9, 2.9);


    //setting ground
    Texture groundtexture;
    groundtexture.loadFromFile("Textures/level3.png");
    Sprite ground[6];
    for (int i = 0; i < 3; i++)
    {
        ground[i].setTexture(groundtexture);
        ground[i].setPosition(Vector2f(i * 5408, 486));
        ground[i].setScale(2.4f, 2.4f);
        ground[i].setOrigin(200, 0);
    }


    //setting blocks
    Texture ground1texture;
    ground1texture.loadFromFile("Textures/blockat.png");
    Sprite ground1[23];
    block(ground1);
    for (int i = 0; i < 23; ++i)
    {
        ground1[i].setTexture(ground1texture);
    }
   // Texture ground1toptexture;
   // ground1toptexture.loadFromFile("Textures/blocattop.png");
   // Sprite ground1top[23];
   //// block(ground1top);
   // for (int i = 0; i < 23; ++i)
   // {
   //     ground1top[i].setTexture(ground1toptexture);
   // }

    //setting greenfires
    Texture greenfires;
    greenfires.loadFromFile("Textures/greenfire.png");
    Sprite green[18];
    for (int i = 0; i < 18; i++)
    {
        green[i].setTexture(greenfires);
        green[i].setPosition(Vector2f(((i * 1600)), 638));
        green[i].setScale(2.7, 3.5);
        green[i].setOrigin(-700, 0);
    }   

    //Variables
    int coinCount = 0;
    float coinAnimationIndicator = 0;
    bool isCoinVisible = true;
    bool landed = false;
    double velocityY = 0;
    score = 0;
    int rings = 0;

    //creating coins 
    Texture coinsTextures;
    coinsTextures.loadFromFile("Textures/coinsprite.png");
    Sprite coins[100];
    coin(coins);
    for (int i = 0; i < 100; i++) {
        coins[i].setTexture(coinsTextures);
        coins[i].setTextureRect(IntRect(0, 0, 64, 64));
        coins[i].setScale(0.8f, 0.8f);
   
    }

    //background 3
    Texture backgroundtexture;
    backgroundtexture.loadFromFile("Textures/lapper-bggreenhill3.jpg");
    Sprite background[18];
    for (int i = 0; i < 18; ++i)
    {
        background[i].setTexture(backgroundtexture);
        background[i].setPosition(Vector2f((i * 1920), -50));
        background[i].setOrigin(400, 0);
        background[i].setScale(1, 1.17);
    }

    //Score
    Font font;
    font.loadFromFile("Fonts/sonic-hud-font.ttf");
    Text text;
    Text text2;
    text.setFont(font);
    text2.setFont(font);
    text.setString(to_string(score));
    text2.setString(to_string(rings));
    text.setFillColor(Color::White);
    text2.setFillColor(Color::White);
    text.setOutlineColor(Color::Black);
    text2.setOutlineColor(Color::Black);
    text.setOutlineThickness(2);
    text2.setOutlineThickness(2);
    text.setPosition(70, 65);
    text.setCharacterSize(32);
    text2.setCharacterSize(32);
    text.setScale(1.45f, 1.45f);
    text2.setScale(1.45f, 1.45f);

    // Declare a Text object for the timer
    Text timerText("", font, 50);
    timerText.setFillColor(Color::White);
    timerText.setOutlineColor(Color::Black);
    timerText.setOutlineThickness(2);
    timerText.setPosition(350, 250);
    timerText.setCharacterSize(32);
    timerText.setScale(1.45f, 1.45f);

    //2D camera
    View view(Vector2f(0, 0), Vector2f(1920, 1080));
    view.setCenter(sonic.sprite.getPosition() - Vector2f(200.0f, 0)); //update
    window.setView(view);

    //powerups
    Setdrops();

    // load the sound 
    SoundBuffer coinBuffer;
    coinBuffer.loadFromFile("Sounds/coinsound.wav");
    Sound coinSound(coinBuffer);


    //colliosion cooldown
    Clock cooldowndamage;
    float cooldownTime = 5.5f;
    bool candamage = true;

    while (window.isOpen())
    {
        //setting time 
        Clock clock, cooldown;
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

        Event aevent;
        while (window.pollEvent(aevent))
        {
            if (aevent.type == Event::Closed)
            {
                window.close();
            }
            if (aevent.type == Event::KeyPressed)
            {
                if (aevent.key.code == Keyboard::Escape)
                {
                    window.close();
                }
            }
        }

        // Move the player using A,D and space keys
        if (sonic.last_key_pressed == 1) {
            sonic.sprite.setTextureRect(IntRect(0, 0, 50, 55));

        }
        if (sonic.last_key_pressed == 2) {
            sonic.sprite.setTextureRect(IntRect(0, 0, 50, 55));
            sonic.sprite.setScale(-2.3, 2.3);
        }
        if (Keyboard::isKeyPressed(Keyboard::A)) {
            sonic.sprite.setScale(-2.3, 2.3);
        }
        if (Mouse::isButtonPressed(Mouse::Left) && sonic.index >= 0 && sonic.canShoot) {
            sonic.bullet[sonic.index].bulletSprite.setPosition(sonic.sprite.getPosition().x, sonic.sprite.getPosition().y);
            sonic.shootCooldown = sonic.bullet[sonic.index].cooldownUse;
            sonic.bullet[sonic.index].moveTo = sonic.last_key_pressed;
            sonic.index--;
            sonic.canShoot = 0;
        }
        //setting position for score
        text.setPosition(sonic.sprite.getPosition().x - 100, 48);
        text2.setPosition(sonic.sprite.getPosition().x - 100, 155);
        timerText.setPosition(sonic.sprite.getPosition().x - 100, 105);
        scoreimage[0].setPosition(sonic.sprite.getPosition().x - 180 - 104, 25);
        scoreimage[1].setPosition(sonic.sprite.getPosition().x - 180 - 104, 880);
        scoreimage[2].setPosition(sonic.sprite.getPosition().x - 50 - 104, 867);


        //collision between sonic and spikes
        for (int i = 0; i < 18; i++) {
            if (sonic.sprite.getGlobalBounds().intersects(green[i].getGlobalBounds()))
            {

                if (candamage && cooldowndamage.getElapsedTime().asSeconds() >= cooldownTime)
                {

                    sonic.damage++;
                    sonic.sprite.move(-450, -150);

                    cooldowndamage.restart();
                    candamage = false;
                }

                if (!candamage && cooldowndamage.getElapsedTime().asSeconds() >= cooldownTime)

                {
                    candamage = true;
                }


            }
        }

        //collision between sonic and enemy
        if (sonic.sprite.getGlobalBounds().intersects(enemy.sprite.getGlobalBounds()))
        {
            sonic.damage++;
            enemy.sprite.setPosition(sonic.sprite.getPosition().x + 2500, 598); // Respawn the enemy on the right side of the window
        }
        if (enemy.sprite.getPosition().x < (sonic.sprite.getPosition().x - 1000))
        {
            enemy.sprite.setPosition(sonic.sprite.getPosition().x + 2500, 598); // Respawn the enemy on the right side of the window
        }

        //collision between sonic and enemy2
        if (sonic.sprite.getGlobalBounds().intersects(enemy2[0].sprite.getGlobalBounds()))
        {
            enemy2[0].sprite.setPosition(sonic.sprite.getPosition().x + 2500, 80); // Respawn the enemy2 on the right side of the window
            sonic.damage++;
        }
        if (enemy2[0].sprite.getPosition().x < (sonic.sprite.getPosition().x - 1000))
        {
            enemy2[0].sprite.setPosition(sonic.sprite.getPosition().x + 2500, 80); // Respawn the enemy2 on the right side of the window
        }
        if (sonic.sprite.getGlobalBounds().intersects(enemy2[1].sprite.getGlobalBounds()))
        {
            enemy2[1].sprite.setPosition(sonic.sprite.getPosition().x + 2500, 120); // Respawn the enemy2 on the right side of the window
            sonic.damage++;
        }
        if (enemy2[1].sprite.getPosition().x > (sonic.sprite.getPosition().x + 1650))
        {
            enemy2[1].sprite.setPosition(sonic.sprite.getPosition().x - 1000, 120); // Respawn the enemy2 on the right side of the window
        }


        //collision between bullets and enemy
        for (int j = 0; j < sonic.bullet.size(); j++)
        {
            if (sonic.bullet[j].bulletSprite.getGlobalBounds().intersects(enemy.sprite.getGlobalBounds())) {
                enemy.health -= 19;
                sonic.bullet[j].bulletSprite.setScale(0, 0);
            }
            if (enemy.health == 0) {
                enemy.sprite.setPosition(sonic.sprite.getPosition().x + 2500, 580);
                score += 5;
                text.setString(to_string(score));
                enemy.health = 3 * 19;
            }
        }

        //sonic limits
        if (sonic.sprite.getPosition().x < 0) {
            sonic.sprite.setPosition(0, sonic.sprite.getPosition().y);
        }
        if (sonic.sprite.getPosition().y < 0) {
            sonic.sprite.setPosition(sonic.sprite.getPosition().x, 0);
        }
        if (sonic.sprite.getPosition().x > 14800) {
            sonic.sprite.setPosition(14800, sonic.sprite.getPosition().y);
        }

        //animation of coins
        for (int i = 0; i < 100; i++) {
            coinAnimationIndicator += 0.018 ;
            if (coinAnimationIndicator > 9)
                coinAnimationIndicator -= 9;
            coins[i].setTextureRect(IntRect(int(coinAnimationIndicator) * 64, 0, 64, 64));
        }

        //incrementing scoring
        for (int i = 0; i < 100; i++) {
            if (sonic.sprite.getGlobalBounds().intersects(coins[i].getGlobalBounds())) {
                coinSound.play(); // Play the sound effect
                coins[i].setScale(0, 0);
                rings++;
                text2.setString(to_string(rings));
            }
        }

        //Updating sonic
        view.setCenter(Vector2f(sonic.sprite.getPosition().x + 648, 540));
        sonic.update(time, 1.0f / 40.f, ground1);


        //enemy animation
        enemy.sprite.move(-enemy.speed, 0);
        enemy.sprite.setTextureRect(IntRect(int(enemy.animation) * 52, 0, 52, 40));
        enemy.animation += 0.1;
        if (enemy.animation > 4)
            enemy.animation = 0;


        //enemy2 animation
        for (int i = 0; i < 2; i++) {
            enemy2[i].sprite.setTextureRect(IntRect(int(enemy.animation) * 36, 0, 36, 48));
            enemy2[i].animation += 0.1;
        }
        enemy2[0].sprite.move(-enemy2[0].speed, 0);
        enemy2[1].sprite.move(enemy2[1].speed, 0);
        if (enemy.animation > 3)
            enemy.animation = 0;

        Time elapsedTime = gametime.getElapsedTime();
        int totalSeconds = static_cast<int>(elapsedTime.asSeconds());
        int minutes = (totalSeconds % 3600) / 60;
        int seconds = totalSeconds % 60;

        // Format the time as a string
        string timeString = to_string(minutes) + "'" + std::to_string(seconds).substr(0, 2);
        timerText.setString(timeString); // Set the text string

        //Upating history after checking if sonic is dead or not
        if (sonic.damage < 3)
        {
            scoreimage[2].setTextureRect(IntRect(88 - (10 * sonic.damage), 0, 10, 25));
        }
        else
        {
            ofstream offile;
            offile.open("history.txt", ios::app);
            offile << name << "   " << score << "   " << timeString << "   " << rings << "*" << endl;
            gameover = true;
            break;
        }

        //checking if the level is finished
        if (sonic.sprite.getGlobalBounds().intersects(end.getGlobalBounds())) {
            sonic.sprite.move(-30, 0);
            sonic.velocity.x = 0;
            break;
        }

        window.clear();
        window.setView(view);
        for (int i = 0; i < 18; ++i)
        {
            window.draw(background[i]);
        }
        for (int i = 0; i < 18; i++)
        {
            window.draw(green[i]);
        }
        for (int i = 0; i < 6; i++)
        {
            window.draw(ground[i]);
        }
        for (int i = 0; i < 100; i++) {
            window.draw(coins[i]);
        }
        window.draw(enemy.sprite);
        for (int i = 0; i < 2; i++) {
            window.draw(enemy2[i].sprite);
        }
        for (int i = 0; i < sonic.bullet.size(); i++)
        {
            window.draw(sonic.bullet[i].bulletSprite);
        }

        for (int i = 0; i < dropBag.size(); i++) {
            window.draw(dropBag[i].dropShape);
        }

        window.draw(sonic.sprite);
        
        for (int i = 0; i < 23; i++) {
            window.draw(ground1[i]);
        }
        /*for (int i = 0; i < 23; i++) {
            window.draw(ground1top[i]);
        }*/
        window.draw(text);
        window.draw(text2);
        window.draw(timerText);
        window.draw(end);
        for (int i = 0; i < 3; i++) {
            window.draw(scoreimage[i]);
        }
        window.display();
    }
}
void selectlevel(RenderWindow& window)
{
    Texture level;
    level.loadFromFile("Textures/main2.jpg");
    Sprite levelz;
    levelz.setTexture(level);

    Texture level1;
    level1.loadFromFile("Textures/levels1.png");
    Sprite levelz1;
    levelz1.setTexture(level1);
    levelz1.setPosition(100, 320);

    Texture level2;
    level2.loadFromFile("Textures/level2.png");
    Sprite levelz2;
    levelz2.setTexture(level2);
    levelz2.setPosition(700, 320);

    Texture level3;
    level3.loadFromFile("Textures/level3m.png");
    Sprite levelz3;
    levelz3.setTexture(level3);
    levelz3.setPosition(1300, 320);

    Font font;
    font.loadFromFile("Fonts/NiseSegaSonic.TTF"); // load font lel window 
    Text ts;
    ts.setFont(font);
    ts.setString("Level 1");
    ts.setCharacterSize(30);
    ts.setOutlineColor(Color::Black);
    ts.setOutlineThickness(3);
    ts.setPosition(200, 661);


    Text ts1;
    ts1.setFont(font);
    ts1.setString("Level 2");
    ts1.setCharacterSize(30);
    ts1.setOutlineColor(Color::Black);
    ts1.setOutlineThickness(3);
    ts1.setPosition(750, 661);

    Text ts2;
    ts2.setFont(font);
    ts2.setString("Level 3");
    ts2.setCharacterSize(30);
    ts2.setOutlineColor(Color::Black);
    ts2.setOutlineThickness(3);
    ts2.setPosition(1370, 661);


    Texture Stars;
    Stars.loadFromFile("Textures/stars.png");
    Sprite star2;
    star2.setTexture(Stars);
    star2.setPosition(329, 575);

    Sprite star3;
    star3.setTexture(Stars);
    star3.setPosition(850, 575);

    Sprite star4;
    star4.setTexture(Stars);
    star4.setPosition(900, 575);

    Sprite star5;
    star5.setTexture(Stars);
    star5.setPosition(1550, 575);

    Sprite star6;
    star6.setTexture(Stars);
    star6.setPosition(1450, 575);

    Sprite star1;
    star1.setTexture(Stars);
    star1.setPosition(1500, 575);

    Font font1;
    font1.loadFromFile("Fonts/NiseSegaSonic.TTF");
    Text Gameover;
    Gameover.setFont(font1);
    Gameover.setString("Levels");
    Gameover.setPosition(805, 100);
    Gameover.setScale(1.7, 1.7);
    Gameover.setFillColor(Color::White);
    Gameover.setOutlineColor(Color::Black);
    Gameover.setOutlineThickness(3);


    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }

            if (Keyboard::isKeyPressed(Keyboard::Escape))
            {
                window.close();
                return;
            }
            Vector2i mousePosition = Mouse::getPosition(window);
            FloatRect spriteBounds = levelz1.getGlobalBounds();
            levelz1.setScale(1, 1);
            if (spriteBounds.contains(mousePosition.x, mousePosition.y))
            {
                levelz1.setScale(1.2, 1.2);
                if (Mouse::isButtonPressed(Mouse::Left)) {
                    GamePlay(window, level1isfinished);
                    if (level1isfinished)
                        GamePlay2(window);
                    if (level2isfinished)
                        GamePlay3(window);
                    window.close();
                    return;
                }
            }

            Vector2i mousePosition1 = sf::Mouse::getPosition(window);
            FloatRect spriteBounds1 = levelz2.getGlobalBounds();
            levelz2.setScale(1, 1);
            if (spriteBounds1.contains(mousePosition1.x, mousePosition1.y))
            {
                levelz2.setScale(1.2, 1.2);
                if (Mouse::isButtonPressed(Mouse::Left)) {
                    GamePlay2(window);
                    if (level2isfinished)
                        GamePlay3(window);
                    window.close();
                    return;
                }
            }

            Vector2i mousePosition2 = Mouse::getPosition(window);
            FloatRect spriteBounds2 = levelz3.getGlobalBounds();
            levelz3.setScale(1, 1);
            if (spriteBounds2.contains(mousePosition2.x, mousePosition2.y))
            {
                if (Mouse::isButtonPressed(Mouse::Left)) {
                    GamePlay3(window);
                    window.close();
                    return;
                }
                levelz3.setScale(1.2, 1.2);
            }

        }
        window.clear();
        window.draw(levelz);
        window.draw(levelz1);
        window.draw(levelz2);
        window.draw(levelz3);
        window.draw(ts);
        window.draw(star1);
        window.draw(ts1);
        window.draw(star2);
        window.draw(ts2);
        window.draw(star3);
        window.draw(star4);
        window.draw(star5);
        window.draw(star6);
        window.draw(Gameover);
        window.display();
    }
}
void main()
{
    RenderWindow pressenter(VideoMode(1920, 1080), "enter game");
    pressEnter(pressenter);
    // make a Main window
    RenderWindow MainMenu(VideoMode(1920, 1080), "game");
    Menu mainmenu(MainMenu.getSize().x, MainMenu.getSize().y);

    Texture duos;
    duos.loadFromFile("Textures/duo.png");
    Sprite duo;
    duo.setTexture(duos);
    duo.setPosition(850, 110);

    Texture hedge;
    hedge.loadFromFile("Textures/headgehog.png");
    Sprite hog;
    hog.setTexture(hedge);
    hog.setPosition(1050, 590);
    hog.setScale(0.55, 0.55);

    Texture mainbutton;
    mainbutton.loadFromFile("Textures/mainmenubutton.png");
    Sprite mainbutton1;
    mainbutton1.setTexture(mainbutton);
    mainbutton1.setPosition(665, 70);
    mainbutton1.setScale(2.4, 2.4);


    Texture mainbg;
    mainbg.loadFromFile("Textures/main1.jpg");
    Sprite bg;
    bg.setTexture(mainbg);


    Texture box;
    box.loadFromFile("Textures/parallelogram.png");
    Sprite box1;
    box1.setTexture(box);
    box1.setPosition(641, 125);
    box1.setScale(-0.8, 0.46);

    Texture box2;
    box2.loadFromFile("Textures/parallelogram.png");
    Sprite box3;
    box3.setTexture(box2);
    box3.setPosition(799, 325);
    box3.setScale(-1.18, 0.5);

    Texture box4;
    box4.loadFromFile("Textures/parallelogram.png");
    Sprite box5;
    box5.setTexture(box4);
    box5.setPosition(805, 500);
    box5.setScale(-1.18, 0.55);

    Texture box6;
    box6.loadFromFile("Textures/parallelogram.png");
    Sprite box7;
    box7.setTexture(box6);
    box7.setPosition(718, 780);
    box7.setScale(-0.7, 0.3);


    while (MainMenu.isOpen())
    {
        Event event;
        while (MainMenu.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                MainMenu.close();
            }
            if (event.type == Event::KeyReleased)
            {
                if (event.key.code == Keyboard::Up)
                {


                    mainmenu.moveup();

                    break;
                }

                if (event.key.code == Keyboard::Down)
                {

                    mainmenu.movedown();

                    break;
                }
                if (event.key.code == Keyboard::Return)
                {
                    RenderWindow window(sf::VideoMode(1920, 1080), "Sonic Game");
                    window.setFramerateLimit(60);
                    RenderWindow entername(VideoMode(1920, 1080), "Enter Name");
                    RenderWindow Options(VideoMode(1920, 1080), "Controls");
                    RenderWindow About(VideoMode(1920, 1080), "Leaderboard");


                    int x = mainmenu.pressed();
                    if (x == 0)
                    {

                        Options.close();
                        About.close();
                        playername(entername, window, name);
                        selectlevel(window);

                    }
                    if (x == 1)
                    {
                        int j = 0;
                        int i = 0;

                        //setting time 
                        Clock clock, cooldown;
                        window.setFramerateLimit(60);
                        float time = clock.getElapsedTime().asMicroseconds();
                        clock.restart();
                        time *= 27.5;

                        float currentframe1 = 0;
                        currentframe1 += 4 * time;

                        Texture keysD;
                        keysD.loadFromFile("Textures/D.png");
                        Sprite keys2(keysD);
                        keys2.setPosition(200, 300);
                        keys2.setScale(3, 3);
                        keys2.setTextureRect(IntRect(0, 0, 19, 21));

                        Texture keysA;
                        keysA.loadFromFile("Textures/A.png");
                        Sprite keys4(keysA);
                        keys4.setPosition(200, 500);
                        keys4.setScale(3, 3);
                        keys4.setTextureRect(IntRect(0, 0, 19, 21));

                        Texture keysSpace;
                        keysSpace.loadFromFile("Textures/SPACE.png");
                        Sprite keys5(keysSpace);
                        keys5.setPosition(130, 730);
                        keys5.setScale(3, 3);
                        keys5.setTextureRect(IntRect(98, 0, 98, 21));

                        Texture keysDmove;
                        keysDmove.loadFromFile("Textures/approvedsonic.png");
                        Sprite keysR(keysDmove);
                        keysR.setPosition(550, 238);
                        keysR.setScale(2, 2);
                        keysR.setTextureRect(IntRect(0, 0, 50, 55));

                        Texture keysblock;
                        keysblock.loadFromFile("Textures/block.png");
                        Sprite keysblock1(keysblock);
                        keysblock1.setPosition(575, 320);
                        keysblock1.setScale(0.5, 0.3);

                        Texture keysAmove;
                        keysAmove.loadFromFile("Textures/approvedsonic.png");
                        Sprite keysL(keysAmove);
                        keysL.setPosition(550, 238);
                        keysL.setScale(-2, 2);
                        keysL.setTextureRect(IntRect(0, 0, 50, 55));

                        Texture keysSmove;
                        keysSmove.loadFromFile("Textures/approvedsonicjump.png");
                        Sprite keysS(keysSmove);
                        keysS.setPosition(750, 665);
                        keysS.setScale(-2, 2);


                        Sprite keysblock3(keysblock);
                        keysblock3.setPosition(575, 740);
                        keysblock3.setScale(0.5, 0.3);


                        Sprite keysblock2(keysblock);
                        keysblock2.setPosition(575, 520);
                        keysblock2.setScale(0.5, 0.3);



                        while (Options.isOpen())
                        {
                            Event aevent;
                            while (Options.pollEvent(aevent))
                            {
                                if (aevent.type == Event::Closed)
                                {
                                    Options.close();

                                }
                                if (aevent.key.code == Keyboard::Escape)
                                {
                                    Options.close();

                                }


                            }


                            RectangleShape controlbg;
                            controlbg.setSize(Vector2f(1920, 1080));
                            controlbg.setScale(1, 1);
                            controlbg.setPosition(0, 0);
                            Texture ctrlbg;
                            ctrlbg.loadFromFile("Textures/ylow.jpg");
                            controlbg.setTexture(&ctrlbg);


                            RectangleShape bluebg;
                            bluebg.setSize(Vector2f(1300, 500));
                            bluebg.setScale(1, 1.48);
                            bluebg.setPosition(0, 150);
                            Texture bmainbg;
                            bmainbg.loadFromFile("Textures/bluebg.png");
                            bluebg.setTexture(&bmainbg);


                            Font font1;
                            font1.loadFromFile("Fonts/NiseSegaSonic.TTF");
                            Text text1;
                            text1.setFont(font1);
                            text1.setString("CONTROLS");
                            text1.setPosition(450, 100);
                            text1.setCharacterSize(70);
                            text1.setFillColor(Color::White);
                            text1.setOutlineColor(Color::Black);
                            text1.setOutlineThickness(3);


                            RectangleShape rectangle1(Vector2f(460.f, 60.f));
                            rectangle1.setPosition(450, 150);
                            rectangle1.setFillColor(Color{ 255, 71, 76 });

                            RectangleShape rectangle(Vector2f(460.f, 60.f));
                            rectangle.setPosition(450, 90);
                            rectangle.setFillColor(Color{ 135, 206, 235 });

                            keysS.move(0, -20);
                            if (keysS.getPosition().y < 580)
                            {
                                keysS.setPosition(750, 665);
                            }


                            keys2.setTextureRect(IntRect(i * 19, 0, 19, 21));
                            keys4.setTextureRect(IntRect(i * 19, 0, 19, 21));
                            keys5.setTextureRect(IntRect(i * 98, 0, 98, 21));
                            keysR.setTextureRect(IntRect(int(currentframe1) * 50, 0, 50, 55));
                            keysL.setTextureRect(IntRect(int(currentframe1) * 50, 0, 50, 55));

                            currentframe1 += 0.0523f * time;
                            if (currentframe1 > 12) {
                                currentframe1 -= 12;
                            }

                            j += 1;
                            if (j > 9)
                            {
                                j -= 9;
                            }
                            keysR.move(20, 0);
                            if (keysR.getPosition().x > 815)
                            {
                                keysR.setPosition(550, 240);
                            }

                            keysL.move(-20, 0);

                            if (keysL.getPosition().x < 630)
                            {
                                keysL.setPosition(890, 440);
                            }


                            window.close();
                            Options.clear();
                            About.close();
                            Options.draw(controlbg);
                            Options.draw(bluebg);
                            Options.draw(rectangle1);
                            Options.draw(rectangle);
                            Options.draw(text1);
                            Options.draw(keys2);
                            Options.draw(keys4);
                            Options.draw(keys5);
                            Options.draw(keysR);
                            Options.draw(keysblock1);
                            Options.draw(keysblock2);
                            Options.draw(keysblock3);
                            Options.draw(keysS);
                            Options.draw(keysL);
                            Options.display();

                        }


                    }
                    if (x == 2)
                    {

                        window.close();
                        Options.close();
                        History(About);

                    }
                    if (x == 3) {

                        MainMenu.close();
                        break;
                    }
                    if (gameover) {
                        RenderWindow gameover(VideoMode(1920, 1080), "Game Over");
                        gameOver(gameover, score);
                    }
                    gameover = false;

                }
            }

        }
        MainMenu.clear();
        MainMenu.draw(bg);
        MainMenu.draw(mainbutton1);
        MainMenu.draw(duo);
        MainMenu.draw(hog);
        MainMenu.draw(box1);
        MainMenu.draw(box3);
        MainMenu.draw(box5);
        MainMenu.draw(box7);
        mainmenu.draw(MainMenu);
        MainMenu.display();
    }
}
