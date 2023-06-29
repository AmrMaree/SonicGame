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
#include <Candle/Candle.hpp>
#include"Candle/LightingArea.hpp"
#include"Candle/LightSource.hpp"
#include"Candle/RadialLight.hpp"
#include"Candle/Constants.hpp"
#include"Candle/DirectedLight.hpp"

using namespace std;
using namespace sf;

bool gameover = false;
string name;
string timeString;
int score = 0;
int rings;
bool level1isfinished = true;
bool level2isfinished = true;
bool level3isfinished = true;
bool bossfightlevel = false;
bool soundison = true;
bool pause = false;
int character = -1;
bool stopFollowingSonic = false;

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
    int w, h;
    float s1, s2;

    void sp(Texture& sonicTexture)
    {
        sprite.setTexture(sonicTexture);
        currentframe = 0;
        runcurrentframe = 0;
        jumpframe = 0;
        waitingframe = 0;
        last_key_pressed = 1;
        damage = -30;
        index = -1;
        droptype = -1;
        speed = 1;
        addSpeed = 0.0;
        sprite.setOrigin(15, 0);
        if (character == 1) {
            w = 50, h = 55;
            s1 = 2.3, s2 = 2.3;
        }
        else if (character == 2) {
            w = 36, h = 41;
            s1 = -2.65, s2 = 2.65;
        }
        else if (character == 3) {
            w = 55, h = 42;
            s1 = 2.3, s2 = 2.3;
        }
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
            sprite.setScale(-s1, s2); // Flip the sprite to face left
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            if (velocity.x < 0) {
                while (velocity.x < 0) {
                    velocity.x += 0.01;
                }
            }
            else
                acceleration.x = moveSpeed;
            sprite.setScale(s1, s2); // Flip the sprite to face right
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

            }

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

    }
    void sonic_animation(float time)
    {
        ////Animation of sonic
        if (((velocity.x >= 500) || (velocity.x <= -500)) && onground == true) {
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
        else if (onground && !Keyboard::isKeyPressed(Keyboard::A) && !Keyboard::isKeyPressed(Keyboard::D) && !Keyboard::isKeyPressed(Keyboard::Space)) {
            waitingframe += 0.0038f * time;
            fullsonictexture.loadFromFile("Textures/waitingsonic.png");
            sprite.setTexture(fullsonictexture);
            sprite.setOrigin(22, 0);
            if (waitingframe > 5)
                waitingframe -= 5;
            sprite.setTextureRect(IntRect(int(waitingframe) * 45, 0, 45, 43));
        }
        else if (velocity.y < 0) {
            jumpframe += 0.015f * time;
            fullsonictexture.loadFromFile("Textures/jumpingsonic.png");
            sprite.setTexture(fullsonictexture);
            sprite.setOrigin(28, 0);
            if (jumpframe > 10)
                jumpframe -= 10;
            sprite.setTextureRect(IntRect(int(jumpframe) * 45, 0, 45, 39));
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
    void knucles_animation(float time)
    {
        ////Animation of knucles
        if (((velocity.x >= 500) || (velocity.x <= -500)) && onground == true) {
            // Running animation
            runcurrentframe += 0.015f * time;
            sprite.setOrigin(20, 0);
            if (runcurrentframe > 12) {
                runcurrentframe -= 12;
            }
            fullsonictexture.loadFromFile("Textures/fastrunningknuckles.png");
            sprite.setTexture(fullsonictexture);
            sprite.setTextureRect(IntRect(0, 0, 42, 44));
            groundHeight = 721;
            //sprite.setPosition(sprite.getPosition().x, sprite.getPosition().y);
            if (acceleration.x != 0)
                sprite.setTextureRect(IntRect((int(runcurrentframe) * 42), 0, 42, 44));
        }
        else if (velocity.y < 0) {
            jumpframe += 0.015f * time;
            fullsonictexture.loadFromFile("Textures/jumpingknuckles.png");
            sprite.setTexture(fullsonictexture);
            sprite.setOrigin(19, 0);
            if (jumpframe > 4) {
                jumpframe -= 4;
            }
            sprite.setTextureRect(IntRect(int(jumpframe) * 39, 0, 39, 59));
        }
        else if (onground && !Keyboard::isKeyPressed(Keyboard::A) && !Keyboard::isKeyPressed(Keyboard::D) && !Keyboard::isKeyPressed(Keyboard::Space))
        {
            waitingframe += 0.0054f * time;
            if (waitingframe > 16)
                waitingframe -= 16;
            fullsonictexture.loadFromFile("Textures/waitingknuckles.png");
            sprite.setTexture(fullsonictexture);
            sprite.setOrigin(18, 0);
            sprite.setTextureRect(IntRect(int(waitingframe) * 36, 0, 36, 38));
        }
        else
        {
            // Idle animation
            currentframe += (0.00523f * time);
            sprite.setOrigin(18, 0);
            if (currentframe > 6) {
                currentframe -= 6;
            }
            fullsonictexture.loadFromFile("Textures/runningknuckles1.png");
            sprite.setTexture(fullsonictexture);
            if (acceleration.x != 0)
                sprite.setTextureRect(IntRect(int(currentframe) * 36, 0, 36, 41));
        }
    }
    void tails_animation(float time)
    {
        //Animation of tails
        if (((velocity.x >= 500) || (velocity.x <= -500)) && onground == true) {
            // Running animation
            runcurrentframe += 0.015f * time;
            sprite.setOrigin(29, 0);
            if (runcurrentframe > 8) {
                runcurrentframe -= 8;
            }
            fullsonictexture.loadFromFile("Textures/fastrunningtails1.png");
            sprite.setTexture(fullsonictexture);
            sprite.setTextureRect(IntRect(0, 0, 58, 48));
            //sprite.setPosition(sprite.getPosition().x, sprite.getPosition().y + 10);
            if (acceleration.x != 0)
                sprite.setTextureRect(IntRect((int(runcurrentframe) * 58), 0, 58, 48));
        }
        else if (onground && !Keyboard::isKeyPressed(Keyboard::A) && !Keyboard::isKeyPressed(Keyboard::D) && !Keyboard::isKeyPressed(Keyboard::Space)) {
            waitingframe += 0.0038f * time;
            fullsonictexture.loadFromFile("Textures/waitingtails.png");
            sprite.setTexture(fullsonictexture);
            sprite.setOrigin(26, 0);
            if (waitingframe > 8)
                waitingframe -= 8;
            sprite.setTextureRect(IntRect(int(waitingframe) * 53, 0, 53, 38));
        }
        else if (velocity.y < 0) {
            jumpframe += 0.015f * time;
            fullsonictexture.loadFromFile("Textures/jumpingtails.png");
            sprite.setTexture(fullsonictexture);
            sprite.setOrigin(20, 0);
            if (jumpframe > 6)
                jumpframe -= 6;
            sprite.setTextureRect(IntRect(int(jumpframe) * 41, 0, 41, 60));
        }
        else
        {
            // Idle animation
            currentframe += 0.00523f * time;
            sprite.setOrigin(22, 0);
            if (currentframe > 8) {
                currentframe -= 8;
            }
            fullsonictexture.loadFromFile("Textures/runningtails.png");
            sprite.setTexture(fullsonictexture);
            if (acceleration.x != 0)
                sprite.setTextureRect(IntRect(int(currentframe) * 55, 0, 55, 42));
        }
    }

};
struct boss
{
    Sprite sprite;
    float speed;
    float x;
};
struct SoundManager {
    std::vector<sf::Sound> sounds;
    std::vector<bool> playing;

    void addSound(sf::SoundBuffer& buffer) {
        sf::Sound sound(buffer);
        sounds.push_back(sound);
        playing.push_back(false); // Initialize the playing status to false
    }

    void setVolume(float volume) {
        for (auto& sound : sounds) {
            sound.setVolume(volume);
        }
    }

    void playSound(int index) {
        if (index >= 0 && index < sounds.size()) {
            playing[index] = true; // Set the playing status to true
            sounds[index].play(); // Play the sound
        }
    }

    void stopSound(int index) {
        if (index >= 0 && index < sounds.size()) {
            playing[index] = false; // Set the playing status to false
            sounds[index].stop(); // Stop the sound
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
    if (character == 1)
    {
        DropsTex[0].loadFromFile("Textures/powerup.png");
        DropsTex[1].loadFromFile("Textures/powerup.png");
    }
    else if (character == 2)
    {
        DropsTex[0].loadFromFile("Textures/powerupk.png");
        DropsTex[1].loadFromFile("Textures/powerupk.png");
    }
    else if (character == 3)
    {
        DropsTex[0].loadFromFile("Textures/powerupt.png");
        DropsTex[1].loadFromFile("Textures/powerupt.png");
    }
   
    DropsTex[2].loadFromFile("Textures/heart (3).png");
    DropsTex[3].loadFromFile("Textures/thunder.png");
    DropsTex[4].loadFromFile("Textures/bullet lazer.png");
    DropsTex[5].loadFromFile("Textures/bullet lazer.png");
    for (int i = 0; i < 4; i++)
    {
        Drops[i].setTexture(DropsTex[i]);
    }
    Drops[0].setScale(1, 1);
    Drops[1].setScale(1, 1);
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
    
    if (bossfightlevel)
    {
        if (timerAdd.getElapsedTime().asSeconds() >= 1.9) {
            int indexDrop = rand() % 4;
            int indexBlock = rand() % 4;
            Help help;
            help.dropShape = Drops[indexDrop];
            help.targetShape = ground1[indexBlock];
            help.dropShape.setPosition(help.targetShape.getPosition().x + help.targetShape.getScale().x * 125 / 2 - 20, -100);
            help.type = indexDrop;
            dropBag.push_back(help);
            timerAdd.restart();

        }
        if (timerDelete.getElapsedTime().asSeconds() >= 5) {
            if (!dropBag.empty()) {
                dropBag.erase(dropBag.begin());
                timerDelete.restart();
            }
        }

    }
}
void dropCollision(player& player, Sound& liveSound, Sound& getthegunSound) {
    SoundManager soundManager;
    for (int i = 0; i < dropBag.size(); i++)
    {
        if (player.sprite.getGlobalBounds().intersects(dropBag[i].dropShape.getGlobalBounds())) {
            player.droptype = dropBag[i].type;
            if (player.droptype == 0 || player.droptype == 1) {
                if (soundison) {
                    soundManager.playSound(1);
                    soundManager.setVolume(20);
                }
                /*if (!soundison)
                {
                    soundManager.setVolume(0);
                    soundManager.stopSound(1);
                }*/
                   
            }
            if (player.droptype == 2) {
                if (!soundison) {
                    soundManager.playSound(2);
                    soundManager.setVolume(20);
                }
              /*  if (!soundison)
                {
                    soundManager.setVolume(0);
                    soundManager.stopSound(2);
                }*/
              
            }
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
            bullet.speed = 25;
            bullet.cooldownUse = 10;
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
            bullet.bulletSprite.setScale(0.6, 0.6);
            bullet.speed = 25;
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
            player.addSpeed = 70;
            player.moveSpeed = 350;
            player.limitSpeed = 800;
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
        if (bullet[i].bulletSprite.getPosition().x >= 15000 ||
            bullet[i].bulletSprite.getPosition().x <= -100) {
            bullet.erase(bullet.begin() + i);
        }

    }
}
void coin(Sprite coins[]) {
    coins[0].setPosition(430, 600);
    coins[1].setPosition(500, 600);
    coins[2].setPosition(600, 600);
    coins[3].setPosition(700, 600);
    coins[4].setPosition(800, 600);
    //floor 

    coins[5].setPosition(1100, 300);
    coins[6].setPosition(1150, 300);
    coins[7].setPosition(1200, 300);
    coins[8].setPosition(1250, 300);



    coins[9].setPosition(2400, 600);
    coins[10].setPosition(2500, 600);
    coins[11].setPosition(2600, 600);
    coins[12].setPosition(2700, 600);
    coins[13].setPosition(2800, 600);

    coins[14].setPosition(2900, 320);
    coins[15].setPosition(2950, 320);
    coins[16].setPosition(3000, 320);
    coins[17].setPosition(3050, 320);
    coins[18].setPosition(3100, 320);


    coins[14].setPosition(4100, 350);
    coins[15].setPosition(4150, 350);
    coins[16].setPosition(4200, 350);
    coins[17].setPosition(4250, 350);
    coins[18].setPosition(4300, 350);

    coins[19].setPosition(5300, 600);
    coins[20].setPosition(5400, 600);
    coins[21].setPosition(5500, 600);
    coins[22].setPosition(5600, 600);
    coins[23].setPosition(5700, 600);

    coins[24].setPosition(7100, 300);
    coins[25].setPosition(7150, 300);
    coins[26].setPosition(7200, 300);
    coins[27].setPosition(7250, 300);
    coins[28].setPosition(7300, 300);


    coins[29].setPosition(8500, 600);
    coins[30].setPosition(8600, 600);
    coins[31].setPosition(8700, 600);
    coins[32].setPosition(8800, 600);
    coins[33].setPosition(8900, 600);
    coins[34].setPosition(9000, 600);
    coins[35].setPosition(9100, 600);
    coins[36].setPosition(9200, 600);


    coins[36].setPosition(9800, 350);
    coins[37].setPosition(9850, 340);
    coins[38].setPosition(9900, 330);
    coins[39].setPosition(9950, 320);
    coins[40].setPosition(10000, 310);
    coins[41].setPosition(10050, 305);
    coins[42].setPosition(10100, 300);



    coins[43].setPosition(11650, 400);
    coins[44].setPosition(11650, 450);
    coins[45].setPosition(11650, 500);
    coins[46].setPosition(11650, 550);
    coins[47].setPosition(11650, 500);
    coins[48].setPosition(11650, 650);
    coins[49].setPosition(11650, 600);

    coins[50].setPosition(13100, 300);
    coins[51].setPosition(13150, 300);
    coins[52].setPosition(13200, 300);
    coins[53].setPosition(13250, 300);
    coins[54].setPosition(13300, 300);



    coins[55].setPosition(13600, 600);
    coins[56].setPosition(13650, 600);
    coins[57].setPosition(13700, 600);
    coins[58].setPosition(13750, 600);
    coins[59].setPosition(13800, 600);
    coins[60].setPosition(13850, 600);
    coins[61].setPosition(13900, 600);
    coins[62].setPosition(13950, 600);
    coins[63].setPosition(14000, 600);
    coins[64].setPosition(14050, 600);
    coins[65].setPosition(14100, 600);
    coins[66].setPosition(14150, 600);
    coins[67].setPosition(14200, 600);
    coins[68].setPosition(14250, 600);
    coins[69].setPosition(14300, 600);
    coins[70].setPosition(14350, 600);


}
void block(Sprite ground1[])
{
    ground1[0].setPosition(560, 430);
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
            //cout << lines.size() << endl;
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
void gameOver(RenderWindow& window, int score, int rings, string timeString) {


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

    Font font1;
    font1.loadFromFile("Fonts/NiseSegaSonic.TTF");
    Text GameoverF;
    GameoverF.setFont(font1);
    GameoverF.setString("Main menu ");
    GameoverF.setPosition(250, 500);
    GameoverF.setScale(1.2, 1.2);
    GameoverF.setFillColor(Color::White);
    GameoverF.setOutlineColor(Color::Black);
    GameoverF.setOutlineThickness(2);


    Text Gameover;
    Gameover.setFont(font1);
    Gameover.setString("Game Over ");
    Gameover.setPosition(100, 100);
    Gameover.setScale(2.5, 2.5);
    Gameover.setFillColor(Color::White);
    Gameover.setOutlineColor(Color::Black);
    Gameover.setOutlineThickness(4);


    Text GameoverScore;
    string ScoreString = to_string(score);
    GameoverScore.setFont(font1);
    GameoverScore.setString("Score : " + ScoreString);
    GameoverScore.setPosition(200, 235);
    GameoverScore.setScale(1.7, 1.7);
    GameoverScore.setFillColor(Color::White);
    GameoverScore.setOutlineColor(Color::Black);
    GameoverScore.setOutlineThickness(4);


    Text Gameovercoins;
    string RingString = to_string(rings);
    Gameovercoins.setFont(font1);
    Gameovercoins.setString("Rings : " + RingString);
    Gameovercoins.setPosition(200, 395);
    Gameovercoins.setScale(1.7, 1.7);
    Gameovercoins.setFillColor(Color::White);
    Gameovercoins.setOutlineColor(Color::Black);
    Gameovercoins.setOutlineThickness(4);


    Text Gameovertime;
    Gameovertime.setFont(font1);
    Gameovertime.setString("Time : " + timeString);
    Gameovertime.setPosition(200, 315);
    Gameovertime.setScale(1.7, 1.7);
    Gameovertime.setFillColor(Color::White);
    Gameovertime.setOutlineColor(Color::Black);
    Gameovertime.setOutlineThickness(4);


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
        window.draw(Gameovercoins);
        window.draw(Gameovertime);
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

    // create a vector of textures for the animation frames
   vector<Texture> frames;
    for (int i = 0; i < 8; i++) {
       Texture frame;
        frame.loadFromFile("Textures/titlescreen" +to_string(i + 1) + ".png");
        // exit the program if a frame fails to load
        frames.push_back(frame);
    }

    sf::Texture titlescreenCoverT;
    titlescreenCoverT.loadFromFile("Textures/titlescreenCover.png");
    sf::Sprite titlescreenCoverS;
    titlescreenCoverS.setTexture(titlescreenCoverT);
    titlescreenCoverS.setPosition(650, 300);
    titlescreenCoverS.setScale(2, 2);

    sf::Texture tieT;
    tieT.loadFromFile("Textures/tie.png");
    sf::Sprite tieS;
    tieS.setTexture(tieT);
    tieS.setPosition(692, 448);
    tieS.setScale(2, 2);

    // create a sprite object
    Sprite sprite1;
    sprite1.setPosition(845.f, 336.f);
    sprite1.setScale(1.7, 1.7);

    // set up the animation properties
    int currentFrame = 0;
    float animationDuration = 0.2f; // duration of each frame in seconds
    sf::Clock animationClock;
    animationClock.restart();
    bool firstLoop = true;


    Font font;
    font.loadFromFile("Fonts/NiseSegaSonic.TTF"); // load font lel window 
    Text t1;
    t1.setFont(font);
    t1.setString("Press Enter");
    t1.setCharacterSize(40);
    t1.setOutlineColor(Color::Black);
    t1.setOutlineThickness(4);
    t1.setPosition(750, 750);

    sf::Clock timer;
    bool visible = true;

    sf::RenderTexture renderTexture;
    renderTexture.create(window.getSize().x, window.getSize().y);



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
        // update the sprite texture based on the elapsed time
        if (animationClock.getElapsedTime().asSeconds() > animationDuration) {
            currentFrame = (currentFrame + 1) % frames.size();
            if (currentFrame == 0 && !firstLoop) {
                currentFrame = 6;
            }
            sprite1.setTexture(frames[currentFrame]);
            animationClock.restart();
            if (currentFrame == 6) {
                firstLoop = false;
            }
            // Update the timer and toggle the text visibility
            sf::Time elapsed = timer.getElapsedTime();
            if (elapsed.asMilliseconds() % 100 == 0)
            {
                visible = !visible;
            }

            renderTexture.clear(sf::Color::Transparent);
            if (visible)
            {
                renderTexture.draw(t1);
            }
            renderTexture.display();

            // Clear the window and draw the render texture and other sprites
            window.clear(sf::Color::Black);
            Sprite sprite(renderTexture.getTexture());
            window.draw(sprite);
            window.draw(bg);
            window.draw(titlescreenCoverS);
            window.draw(t1);
            
            window.draw(sprite1);
            window.draw(tieS);
            window.display();

        }

    }
}
void GamePlay(RenderWindow& window, bool& level1isfinished) {
    srand(static_cast<unsigned>(time(NULL)));
    Clock timerAdd, timerDelete, gametime;
    SoundManager soundManager;
    level1isfinished = false;

    Texture pauseT;
    pauseT.loadFromFile("Textures/arrow.png");
    Sprite pauseS;
    pauseS.setTexture(pauseT);
    pauseS.setScale(1, 1);

    Font font1;
    font1.loadFromFile("Fonts/NiseSegaSonic.TTF");
    Text text1;
    Text tS;
    text1.setFont(font1);
    text1.setString("Continue");
    text1.setPosition(150, 300);
    text1.setCharacterSize(70);
    text1.setFillColor(Color::White);
    text1.setOutlineColor(Color::Black);
    text1.setOutlineThickness(3);

    tS.setFont(font1);
    tS.setString("Main Menu");
    tS.setPosition(690, 300);
    tS.setCharacterSize(70);
    tS.setFillColor(Color::White);
    tS.setOutlineColor(Color::Black);
    tS.setOutlineThickness(3);


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

    //declaring sonic

    Texture sonictexture;
    sonictexture.loadFromFile("Textures/approvedsonic.png");
    player sonic;
    sonic.sprite.setTextureRect(IntRect(0, 0, 50, 55));
    sonic.sprite.setScale(sonic.s1, sonic.s2);
    sonic.sp(sonictexture);


    //ending sign
    Texture endtexture;
    endtexture.loadFromFile("Textures/ending.png");
    Sprite end(endtexture);
    end.setScale(2.8f, 2.8f);
    end.setPosition(14750, 560);

    //setting ground
    Texture groundtexture;
    groundtexture.loadFromFile("Textures/map.png");
    Sprite ground[6];
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
    rings = 0;
    bool stopFollowingSonic = false;


    //creating coins 
    Texture coinsTextures;
    coinsTextures.loadFromFile("Textures/coinsprite.png");
    Sprite coins[70];
    coin(coins);
    for (int i = 0; i < 70; i++) {
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

    // load coin sound 
    SoundBuffer coinBuffer;
    coinBuffer.loadFromFile("Sounds/soundcoin.wav");
    soundManager.addSound(coinBuffer);

    // load the bullet sound
    SoundBuffer bulletBuffer;
    bulletBuffer.loadFromFile("Sounds/lazer3.wav");
    soundManager.addSound(bulletBuffer);

    //load the sound of sonic's death
    SoundBuffer sdBuffer;
    sdBuffer.loadFromFile("Sounds/funnydeath.wav");
    soundManager.addSound(sdBuffer);

    //load heart sound
    SoundBuffer liveBuffer;
    liveBuffer.loadFromFile("Sounds/live.wav");
    Sound liveSound(liveBuffer);

    //load the sound when sonic got the gun
    SoundBuffer gotthegBuffer;
    gotthegBuffer.loadFromFile("Sounds/gotthegun.wav");
    Sound getthegunSound(gotthegBuffer);

    ////load sound of level up
    SoundBuffer levelupBuffer;
    levelupBuffer.loadFromFile("Sounds/levelup.wav");
   soundManager.addSound(levelupBuffer);


    // load soundtrack and loop it
    Music soundtrackMusic;
    if (soundtrackMusic.openFromFile("Sounds/music.ogg.opus"))

        soundtrackMusic.setLoop(true);
    if(soundison)
    soundtrackMusic.setVolume(20);
    else if(soundison==false)
        soundtrackMusic.setVolume(0);
    soundtrackMusic.play();


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

        if (soundison)
            soundManager.setVolume(20);
        if (!soundison)
        {
            soundManager.setVolume(0);
            soundManager.stopSound(0);
            //soundManager.stopSound(1);
            //soundManager.stopSound(2);
            soundManager.stopSound(5);
        }

        chooseDrop(ground1, timerAdd, timerDelete);
        dropADrop();
        dropCollision(sonic, liveSound, getthegunSound);
        checkDrop(sonic);
        resetSpeed(sonic);
        //bullet 
        bulletCooldown(sonic);
        moveBullets(sonic.bullet);
        // Move the player using A,D and space keys
        if (sonic.last_key_pressed == 1) {
            sonic.sprite.setTextureRect(IntRect(0, 0, sonic.w, sonic.h));
            sonic.sprite.setScale(sonic.s1, sonic.s2);
        }
        if (sonic.last_key_pressed == 2) {
            sonic.sprite.setTextureRect(IntRect(0, 0, sonic.w, sonic.h));
            sonic.sprite.setScale(-sonic.s1, sonic.s2);
        }
        if (Keyboard::isKeyPressed(Keyboard::A)) {
            sonic.sprite.setScale(-sonic.s1, sonic.s2);
            sonic.last_key_pressed = 2;
        }
        if (Keyboard::isKeyPressed(Keyboard::D)) {
            sonic.sprite.setScale(sonic.s1, sonic.s2);
            sonic.last_key_pressed = 1;
        }

        if (!pause) {
            if (Mouse::isButtonPressed(Mouse::Left) && sonic.index >= 0 && sonic.canShoot) {
                soundManager.playSound(1);
                sonic.bullet[sonic.index].bulletSprite.setPosition(sonic.sprite.getPosition().x, sonic.sprite.getPosition().y);
                sonic.shootCooldown = sonic.bullet[sonic.index].cooldownUse;
                sonic.bullet[sonic.index].moveTo = sonic.last_key_pressed;
                sonic.index--;
                sonic.canShoot = 0;
            }
        }
        //setting position for score
        if (!stopFollowingSonic|| !pause) {
            text.setPosition(sonic.sprite.getPosition().x - 100, 48);
            text2.setPosition(sonic.sprite.getPosition().x - 100, 155);
            timerText.setPosition(sonic.sprite.getPosition().x - 100, 105);
            scoreimage[0].setPosition(sonic.sprite.getPosition().x - 180 - 104, 25);
            scoreimage[1].setPosition(sonic.sprite.getPosition().x - 180 - 104, 880);
            scoreimage[2].setPosition(sonic.sprite.getPosition().x - 50 - 104, 867);
        }
        if (!pause)
        {
            pauseS.setPosition(sonic.sprite.getPosition().x + 500,10);
        }


        Vector2i mousePosition = Mouse::getPosition(window);
        FloatRect spriteBounds(sonic.sprite.getPosition().x + 700, pauseS.getPosition().y, pauseS.getGlobalBounds().width,pauseS.getGlobalBounds().height);
        pauseS.setScale(1, 1);

        if (spriteBounds.contains(mousePosition.x, mousePosition.y))
        {
            pauseS.setScale(1.7, 1.7);

            if (Mouse::isButtonPressed(Mouse::Left))
            {
                pause = true;
            }
        }
        Vector2i mousePosition1 = Mouse::getPosition(window);
        FloatRect spriteBounds1 = text1.getGlobalBounds();
        text1.setScale(1, 1);

        if (spriteBounds1.contains(mousePosition1.x, mousePosition1.y))
        {
            text1.setScale(1.2, 1.2);

            if (Mouse::isButtonPressed(Mouse::Left))
            {
                pause = false;
            }
        }
        Vector2i mousePosition2 = Mouse::getPosition(window);
        FloatRect spriteBounds2 = tS.getGlobalBounds();
        tS.setScale(1, 1);

        if (spriteBounds2.contains(mousePosition2.x, mousePosition2.y))
        {
            tS.setScale(1.2, 1.2);

            if (Mouse::isButtonPressed(Mouse::Left))
            {
                pause = false;
                window.close();
            }
        }


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
        if (!stopFollowingSonic||!pause)
        {
            if (sonic.sprite.getGlobalBounds().intersects(enemy.sprite.getGlobalBounds()))
            {
                sonic.damage++;
                enemy.sprite.setPosition(sonic.sprite.getPosition().x + 2500, 580); // Respawn the enemy on the right side of the window
            }
            if (enemy.sprite.getPosition().x < (sonic.sprite.getPosition().x - 1000))
            {
                enemy.sprite.setPosition(sonic.sprite.getPosition().x + 2500, 580); // Respawn the enemy on the right side of the window
            }
        }
        //collision between sonic and enemy2
        if (!stopFollowingSonic || !pause)
        {
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



        //animation of coins
        for (int i = 0; i < 70; i++) {
            coinAnimationIndicator += 0.008;
            if (coinAnimationIndicator > 9)
                coinAnimationIndicator -= 9;
            coins[i].setTextureRect(IntRect(int(coinAnimationIndicator) * 64, 0, 64, 64));
        }

        //incrementing scoring
        for (int i = 0; i < 70; i++) {
            if (sonic.sprite.getGlobalBounds().intersects(coins[i].getGlobalBounds())) {
                soundManager.playSound(0);
                coins[i].setScale(0, 0);
                rings++;
                text2.setString(to_string(rings));
            }
        }

        //Updating sonic
        if (!pause) 
        {
            sonic.update(time, 1.0f / 40.f, ground1);
        }
        if (character == 1)
        {
            sonic.sonic_animation(time);
        }
        else if (character == 2)
        {
            sonic.knucles_animation(time);
        }
        else if (character == 3)
        {
            sonic.tails_animation(time);
        }

        if (!pause)
        {
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
        }

        Time elapsedTime = gametime.getElapsedTime();
        int totalSeconds = static_cast<int>(elapsedTime.asSeconds());
        int minutes = (totalSeconds % 3600) / 60;
        int seconds = totalSeconds % 60;

        // Format the time as a string
        timeString = to_string(minutes) + "'" + to_string(seconds).substr(0, 2);
        timerText.setString(timeString); // Set the text string

        //Upating history after checking if sonic is dead or not
        if (sonic.damage < 3)
        {
            scoreimage[2].setTextureRect(IntRect(88 - (10 * sonic.damage), 0, 10, 25));
        }
        else
        {
            ofstream offile;
            offile.open("History.txt", ios::app);
            offile << name << "   " << score << "   " << timeString << "   " << rings << "*" << endl;
            soundManager.playSound(3);
            gameover = true;
            break;
        }
        if (!stopFollowingSonic) {
            view.setCenter(Vector2f(sonic.sprite.getPosition().x + 648, 540));
        }
        //checking if the level is finished
        if (sonic.sprite.getPosition().x > 13700)
            stopFollowingSonic = true;


        if (sonic.sprite.getPosition().x < 13700 && stopFollowingSonic)
            sonic.sprite.setPosition(13700, sonic.sprite.getPosition().y);

        if (sonic.sprite.getPosition().x > 14750)
            sonic.sprite.move(10, 0);

        if (sonic.sprite.getPosition().x > 15400) {
            level1isfinished = true;
            soundtrackMusic.pause();
            soundManager.playSound(5);
        }
        if (level1isfinished)
            return;
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
        for (int i = 0; i < 70; i++) {
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
        for (int i = 0; i < 6; i++)
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
        window.draw(pauseS);
        if (pause)
        {
            window.draw(text1);
            window.draw(tS);
        }
        window.display();
    }
}
void GamePlay2(RenderWindow& window, bool& level2isfinished) {
    srand(static_cast<unsigned>(time(NULL)));
    Clock timerAdd, timerDelete, gametime;
    level2isfinished = false;
    stopFollowingSonic = false;

    Texture pauseT;
    pauseT.loadFromFile("Textures/arrow.png");
    Sprite pauseS;
    pauseS.setTexture(pauseT);
    pauseS.setScale(1, 1);

    Font font1;
    font1.loadFromFile("Fonts/NiseSegaSonic.TTF");
    Text text1;
    Text tS;
    text1.setFont(font1);
    text1.setString("Continue");
    text1.setPosition(150, 300);
    text1.setCharacterSize(70);
    text1.setFillColor(Color::White);
    text1.setOutlineColor(Color::Black);
    text1.setOutlineThickness(3);

    tS.setFont(font1);
    tS.setString("Main Menu");
    tS.setPosition(690, 300);
    tS.setCharacterSize(70);
    tS.setFillColor(Color::White);
    tS.setOutlineColor(Color::Black);
    tS.setOutlineThickness(3);

    vector<Texture> frames;
    frames.emplace_back();
    frames.back().loadFromFile("Textures/fire1.png");
    frames.emplace_back();
    frames.back().loadFromFile("Textures/fire2.png");
    frames.emplace_back();
    frames.back().loadFromFile("Textures/fire3.png");

    const int numSprites = 8; // Number of sprite instances to create
    vector<Sprite> sprites(numSprites);
    // Setting fire 
    for (int i = 0; i < numSprites; i++)
    {
        sprites[i].setTexture(frames[0]);
        sf::FloatRect frameBounds = sprites[i].getLocalBounds();
        sprites[i].setOrigin(frameBounds.width / 2, frameBounds.height / 2);
        sprites[i].setPosition((i + 1) * window.getSize().x + 30 / (numSprites + 1), 690);
        sprites[i].setScale(3.5f, 7);
    }

    int currentFrame = 0;
    Clock frameClock;
    Clock timer; // Clock for measuring elapsed time
    const float timeLimit = 2.0f; // Time limit in seconds
    int loopCounter = 0;



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
    sonic.sprite.setScale(sonic.s1, sonic.s2);
    sonic.sp(sonictexture);



    //ending sign
    Texture endtexture;
    endtexture.loadFromFile("Textures/ending.png");
    Sprite end(endtexture);
    end.setScale(2.8f, 2.8f);
    end.setPosition(14750, 578);


    //declaring enemy1
    Texture enemytexture;
    enemytexture.loadFromFile("Textures/beetle.png");
    Enemy enemy[7];
    for (int i = 0; i < 7; i++) {
        enemy[i].sprite.setTexture(enemytexture);
        enemy[i].speed = 3;
        enemy[i].sprite.setTextureRect(IntRect(0, 0, 45, 35));
        enemy[i].sprite.setScale(-2.8, 2.8);
        enemy[i].sprite.setPosition((enemy[i].posX) + (i * 2000.0f), 622);
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
        backgroundimage[i].setPosition(2400 * i, 210);
        backgroundimage[i].setScale(2, 2);
        backgroundimage[i].setOrigin(500, 0);
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
    rings = 0;

    //creating coins 
    Texture coinsTextures;
    coinsTextures.loadFromFile("Textures/coinsprite.png");
    Sprite coins[70];
    coin(coins);
    for (int i = 0; i < 70; i++) {
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
    SoundManager soundManager;

    // load coin sound 
    SoundBuffer coinBuffer;
    coinBuffer.loadFromFile("Sounds/soundcoin.wav");
    soundManager.addSound(coinBuffer);

    // load the bullet sound
    SoundBuffer bulletBuffer;
    bulletBuffer.loadFromFile("Sounds/lazer3.wav");
    soundManager.addSound(bulletBuffer);

    //load the sound of sonic's death
    SoundBuffer sdBuffer;
    sdBuffer.loadFromFile("Sounds/funnydeath.wav");
    soundManager.addSound(sdBuffer);

    //load heart sound
    SoundBuffer liveBuffer;
    liveBuffer.loadFromFile("Sounds/live.wav");
    Sound liveSound(liveBuffer);

    //load the sound when sonic got the gun
    SoundBuffer gotthegBuffer;
    gotthegBuffer.loadFromFile("Sounds/gotthegun.wav");
    Sound getthegunSound(gotthegBuffer);

    ////load sound of level up
    //SoundBuffer levelupBuffer;
    //levelupBuffer.loadFromFile("Sounds/levelup.wav");
    //soundManager.addSound(levelupBuffer);


 // load soundtrack and loop it
    Music soundtrackMusic;
    if (soundtrackMusic.openFromFile("Sounds/music.ogg.opus"))

        soundtrackMusic.setLoop(true);
    if (soundison)
        soundtrackMusic.setVolume(20);
    else if (soundison == false)
        soundtrackMusic.setVolume(0);
    soundtrackMusic.play();

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
    float cooldownTime = 2.2f;
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
        dropCollision(sonic, liveSound, getthegunSound);
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

        Vector2i mousePosition = Mouse::getPosition(window);
        FloatRect spriteBounds = pauseS.getGlobalBounds();
        pauseS.setScale(1, 1);

        if (spriteBounds.contains(mousePosition.x, mousePosition.y))
        {
            pauseS.setScale(1, 1);

            if (Mouse::isButtonPressed(Mouse::Left))
            {
                pause = true;
            }
        }
        Vector2i mousePosition1 = Mouse::getPosition(window);
        FloatRect spriteBounds1 = text1.getGlobalBounds();
        text1.setScale(1, 1);

        if (spriteBounds1.contains(mousePosition1.x, mousePosition1.y))
        {
            text1.setScale(1.2, 1.2);

            if (Mouse::isButtonPressed(Mouse::Left))
            {
                pause = false;
            }
        }
        Vector2i mousePosition2 = Mouse::getPosition(window);
        FloatRect spriteBounds2 = tS.getGlobalBounds();
        tS.setScale(1, 1);

        if (spriteBounds2.contains(mousePosition2.x, mousePosition2.y))
        {
            tS.setScale(1.2, 1.2);

            if (Mouse::isButtonPressed(Mouse::Left))
            {
                pause = false;
                window.close();
            }
        }


        if (frameClock.getElapsedTime().asSeconds() > 0.1f)
        {
            currentFrame = (currentFrame + 1) % frames.size();
            for (int i = 0; i < numSprites; i++)
            {
                sprites[i].setTexture(frames[currentFrame]);
            }
            frameClock.restart();

            if (currentFrame == 0) {
                loopCounter++;
            }

            if (loopCounter >= 8) {

            }
        }
        soundManager.setVolume(20);

        if (soundison)
            soundManager.setVolume(20);
        if (!soundison)
        {
            soundManager.setVolume(0);
            soundManager.stopSound(0);
            //soundManager.stopSound(1);
            //soundManager.stopSound(2);
            soundManager.stopSound(5);
        }

        // Move the player using A,D and space keys
        if (sonic.last_key_pressed == 1) {
            sonic.sprite.setTextureRect(IntRect(0, 0, sonic.w, sonic.h));
            sonic.sprite.setScale(sonic.s1, sonic.s2);
        }
        if (sonic.last_key_pressed == 2) {
            sonic.sprite.setTextureRect(IntRect(0, 0, sonic.w, sonic.h));
            sonic.sprite.setScale(-sonic.s1, sonic.s2);
        }
        if (Keyboard::isKeyPressed(Keyboard::A)) {
            sonic.sprite.setScale(-sonic.s1, sonic.s2);
            sonic.last_key_pressed = 2;
        }
        if (Keyboard::isKeyPressed(Keyboard::D)) {
            sonic.sprite.setScale(sonic.s1, sonic.s2);
            sonic.last_key_pressed = 1;
        }
        if (!pause) {
            if (Mouse::isButtonPressed(Mouse::Left) && sonic.index >= 0 && sonic.canShoot) {
                soundManager.playSound(1);
                sonic.bullet[sonic.index].bulletSprite.setPosition(sonic.sprite.getPosition().x, sonic.sprite.getPosition().y);
                sonic.shootCooldown = sonic.bullet[sonic.index].cooldownUse;
                sonic.bullet[sonic.index].moveTo = sonic.last_key_pressed;
                sonic.index--;
                sonic.canShoot = 0;
            }
        }
        
        //setting position for score
        if (!stopFollowingSonic || !pause) {
            text.setPosition(sonic.sprite.getPosition().x - 100, 48);
            text2.setPosition(sonic.sprite.getPosition().x - 100, 155);
            timerText.setPosition(sonic.sprite.getPosition().x - 100, 105);
            scoreimage[0].setPosition(sonic.sprite.getPosition().x - 180 - 104, 25);
            scoreimage[1].setPosition(sonic.sprite.getPosition().x - 180 - 104, 880);
            scoreimage[2].setPosition(sonic.sprite.getPosition().x - 50 - 104, 867);
            pauseS.setPosition(0, 10);
        }

        //collision between sonic and fire
        for (int i = 0; i < 7; i++) {
            if (sonic.sprite.getGlobalBounds().intersects(sprites[i].getGlobalBounds()))
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
        if (!pause)
        {
            for (int i = 0; i < 7; i++) {
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
        }

        //collision between sonic and enemy2
        if (!stopFollowingSonic || !pause)
        {
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
        }
        //collision between bullets and enemy
        for (int i = 0; i < 7; i++) {
            for (int j = 0; j < sonic.bullet.size(); j++)
            {
                if (sonic.bullet[j].bulletSprite.getGlobalBounds().intersects(enemy[i].sprite.getGlobalBounds())) {
                    enemy[i].health -= 19;
                    sonic.bullet[j].bulletSprite.setScale(0, 0);
                }
                if (enemy[i].health == 0) {
                    enemy[i].sprite.setPosition(-40000, 0);
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


        //animation of coins
        for (int i = 0; i < 70; i++) {
            coinAnimationIndicator += 0.008;
            if (coinAnimationIndicator > 9)
                coinAnimationIndicator -= 9;
            coins[i].setTextureRect(IntRect(int(coinAnimationIndicator) * 64, 0, 64, 64));
        }

        //incrementing scoring
        for (int i = 0; i < 70; i++) {
            if (sonic.sprite.getGlobalBounds().intersects(coins[i].getGlobalBounds())) {
                soundManager.playSound(0);
                coins[i].setScale(0, 0);
                rings++;
                text2.setString(to_string(rings));
            }
        }


        //Updating sonic
        if (!pause)
        {
            sonic.update(time, 1.0f / 40.f, ground1);
        }
        if (character == 1)
        {
            sonic.sonic_animation(time);
        }
        else if (character == 2)
        {
            sonic.knucles_animation(time);
        }
        else if (character == 3)
        {
            sonic.tails_animation(time);
        }
        if (!pause) {
            //enemy animation
            for (int i = 0; i < 7; i++) {
                enemy[i].sprite.setTextureRect(IntRect(int(enemy[i].animation) * 45, 0, 45, 35));
                enemy[i].animation += 0.01;
                if (enemy[i].animation > 1)
                    enemy[i].animation = 0;
            }
        }

        for (int i = 0; i < 7; i++) {
            // Update the enemy's position based on movement direction
            if (enemy[i].moveRight) {
                enemy[i].sprite.move(enemy[i].moveSpeed, 0.0f);
                enemy[i].sprite.setScale(-2.8, 2.8);
            }
            else {
                enemy[i].sprite.move(-enemy[i].moveSpeed, 0.0f);
                enemy[i].sprite.setScale(2.8, 2.8);
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
        if (pause) {
            for (int i = 0; i < 2; i++) {
                enemy2[i].sprite.setTextureRect(IntRect(int(enemy2[i].animation) * 36, 0, 36, 48));
                enemy2[i].animation += 0.1;
                if (enemy2[i].animation > 3)
                    enemy2[i].animation = 0;
            }
            enemy2[0].sprite.move(-enemy2[0].speed, 0);
            enemy2[1].sprite.move(enemy2[1].speed, 0);

       }

        Time elapsedTime = gametime.getElapsedTime();
        int totalSeconds = static_cast<int>(elapsedTime.asSeconds());
        int minutes = (totalSeconds % 3600) / 60;
        int seconds = totalSeconds % 60;

        // Format the time as a string
        timeString = to_string(minutes) + "'" + std::to_string(seconds).substr(0, 2);
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
            soundManager.playSound(3);
            gameover = true;
            break;
        }
        sonic.groundHeight = 744;
        sonic.moveSpeed = 1;

        if (!stopFollowingSonic) {
            view.setCenter(Vector2f(sonic.sprite.getPosition().x + 648, 540));
        }


        //checking if the level is finished
        if (sonic.sprite.getPosition().x > 13700)
            stopFollowingSonic = true;


        if (sonic.sprite.getPosition().x < 13700 && stopFollowingSonic)
            sonic.sprite.setPosition(13700, sonic.sprite.getPosition().y);

        if (sonic.sprite.getPosition().x > 14750)
            sonic.sprite.move(10, 0);

        if (sonic.sprite.getPosition().x > 15400) {
            level1isfinished = true;
            soundtrackMusic.pause();
            soundManager.playSound(5);
        }
        if (level2isfinished)
            return;
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
        for (int i = 0; i < 70; i++) {
            window.draw(coins[i]);
        }

        for (int i = 0; i < numSprites - 1; i++)
        {
            window.draw(sprites[i]);
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
        window.draw(pauseS);
        if (pause)
        {
            window.draw(text1);
            window.draw(tS);
        }
        window.display();
    }
}
void GamePlay3(RenderWindow& window, bool& level3isfinished) {
    srand(static_cast<unsigned>(time(NULL)));
    Clock timerAdd, timerDelete, gametime;
    //level3isfinished = false;
    stopFollowingSonic = false;

    vector<Texture> frames;
    frames.emplace_back();
    frames.back().loadFromFile("Textures/flames1.png");
    frames.emplace_back();
    frames.back().loadFromFile("Textures/flames2.png");
    frames.emplace_back();
    frames.back().loadFromFile("Textures/flames3.png");

    const int numSprites = 8; // Number of sprite instances to create
    std::vector<sf::Sprite> sprites(numSprites);

    // Setting fire 

    for (int i = 0; i < numSprites; i++)
    {
        sprites[i].setTexture(frames[0]);
        sf::FloatRect frameBounds = sprites[i].getLocalBounds();
        sprites[i].setOrigin(frameBounds.width / 2, frameBounds.height / 2);
        sprites[i].setPosition((i + 1) * window.getSize().x + 30 / (numSprites + 1), 678);
        sprites[i].setScale(3.5f, 7);
    }

    int currentFrame = 0;
    Clock frameClock;
    Clock timer; // Clock for measuring elapsed time
    const float timeLimit = 2.0f; // Time limit in seconds
    int loopCounter = 0;


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
    sonic.sprite.setScale(sonic.s1, sonic.s2);
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
    groundtexture.loadFromFile("Textures/map3ground.png");
    Sprite ground[6];
    for (int i = 0; i < 3; i++)
    {
        ground[i].setTexture(groundtexture);
        ground[i].setPosition(Vector2f(i * 5408, 516));
        ground[i].setScale(2, 2);
        ground[i].setOrigin(200, 0);
    }
    //setting background image
    Texture backgroundimagetexture;
    backgroundimagetexture.loadFromFile("Textures/map3top.png");
    Sprite backgroundimage[18];
    for (int i = 0; i < 18; ++i)
    {
        backgroundimage[i].setTexture(backgroundimagetexture);
        backgroundimage[i].setPosition(5062 * i, -82);
        backgroundimage[i].setScale(2.7, 2.7);
        backgroundimage[i].setOrigin(700, 0);
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

   

    //Variables
    int coinCount = 0;
    float coinAnimationIndicator = 0;
    bool isCoinVisible = true;
    bool landed = false;
    double velocityY = 0;
    score = 0;
    rings = 0;

    //creating coins 
    Texture coinsTextures;
    coinsTextures.loadFromFile("Textures/coinsprite.png");
    Sprite coins[70];
    coin(coins);
    for (int i = 0; i < 70; i++) {
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
    SoundManager soundManager;

    // load coin sound 
    SoundBuffer coinBuffer;
    coinBuffer.loadFromFile("Sounds/soundcoin.wav");
    soundManager.addSound(coinBuffer);

    // load the bullet sound
    SoundBuffer bulletBuffer;
    bulletBuffer.loadFromFile("Sounds/lazer3.wav");
    soundManager.addSound(bulletBuffer);

    //load the sound of sonic's death
    SoundBuffer sdBuffer;
    sdBuffer.loadFromFile("Sounds/funnydeath.wav");
    soundManager.addSound(sdBuffer);

    //load heart sound
    SoundBuffer liveBuffer;
    liveBuffer.loadFromFile("Sounds/live.wav");
    Sound liveSound(liveBuffer);

    //load the sound when sonic got the gun
    SoundBuffer gotthegBuffer;
    gotthegBuffer.loadFromFile("Sounds/gotthegun.wav");
    Sound getthegunSound(gotthegBuffer);

    ////load sound of level up
    //SoundBuffer levelupBuffer;
    //levelupBuffer.loadFromFile("Sounds/levelup.wav");
    //soundManager.addSound(levelupBuffer);


   // load soundtrack and loop it
    Music soundtrackMusic;
    if (soundtrackMusic.openFromFile("Sounds/music.ogg.opus"))

        soundtrackMusic.setLoop(true);
    if (soundison)
        soundtrackMusic.setVolume(20);
    else if (soundison == false)
        soundtrackMusic.setVolume(0);
    soundtrackMusic.play();

    //colliosion cooldown
    Clock cooldowndamage;
    float cooldownTime = 5;
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
        dropCollision(sonic, liveSound, getthegunSound);
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

        soundManager.setVolume(20);
        if (soundison)
            soundManager.setVolume(20);
        if (!soundison)
        {
            soundManager.setVolume(0);
            soundManager.stopSound(0);
            //soundManager.stopSound(1);
            //soundManager.stopSound(2);
            soundManager.stopSound(5);
        }
        if (frameClock.getElapsedTime().asSeconds() > 0.1f)
        {
            currentFrame = (currentFrame + 1) % frames.size();
            for (int i = 0; i < numSprites; i++)
            {
                sprites[i].setTexture(frames[currentFrame]);
            }
            frameClock.restart();

            if (currentFrame == 0) {
                loopCounter++;
            }

            if (loopCounter >= 8) {

            }
        }


        // Move the player using A,D and space keys
        if (sonic.last_key_pressed == 1) {
            sonic.sprite.setTextureRect(IntRect(0, 0, sonic.w, sonic.h));
            sonic.sprite.setScale(sonic.s1, sonic.s2);
        }
        if (sonic.last_key_pressed == 2) {
            sonic.sprite.setTextureRect(IntRect(0, 0, sonic.w, sonic.h));
            sonic.sprite.setScale(-sonic.s1, sonic.s2);
        }
        if (Keyboard::isKeyPressed(Keyboard::A)) {
            sonic.sprite.setScale(-sonic.s1, sonic.s2);
            sonic.last_key_pressed = 2;
        }
        if (Keyboard::isKeyPressed(Keyboard::D)) {
            sonic.sprite.setScale(sonic.s1, sonic.s2);
            sonic.last_key_pressed = 1;
        }

        if (Mouse::isButtonPressed(Mouse::Left) && sonic.index >= 0 && sonic.canShoot) {
            soundManager.playSound(1);
            sonic.bullet[sonic.index].bulletSprite.setPosition(sonic.sprite.getPosition().x, sonic.sprite.getPosition().y);
            sonic.shootCooldown = sonic.bullet[sonic.index].cooldownUse;
            sonic.bullet[sonic.index].moveTo = sonic.last_key_pressed;
            sonic.index--;
            sonic.canShoot = 0;
        }

        //setting position for score
        if (!stopFollowingSonic)
        {
            text.setPosition(sonic.sprite.getPosition().x - 100, 48);
            text2.setPosition(sonic.sprite.getPosition().x - 100, 155);
            timerText.setPosition(sonic.sprite.getPosition().x - 100, 105);
            scoreimage[0].setPosition(sonic.sprite.getPosition().x - 180 - 104, 25);
            scoreimage[1].setPosition(sonic.sprite.getPosition().x - 180 - 104, 880);
            scoreimage[2].setPosition(sonic.sprite.getPosition().x - 50 - 104, 867);
        }


        //collision between sonic and spikes
        for (int i = 0; i < 8; i++) {
            if (sonic.sprite.getGlobalBounds().intersects(sprites[i].getGlobalBounds()))
            {

                if (candamage && cooldowndamage.getElapsedTime().asSeconds() >= cooldownTime)
                {
                    if (5 % 2 == 0)
                    {
                        Color colorzat = sonic.sprite.getColor(); // Get the current color of the sprite
                        colorzat.a = 128;
                        sonic.sprite.setColor(colorzat);
                    }
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
        if (!stopFollowingSonic)
        {
            if (sonic.sprite.getGlobalBounds().intersects(enemy.sprite.getGlobalBounds()))
            {
                sonic.damage++;
                enemy.sprite.setPosition(sonic.sprite.getPosition().x + 2500, 598); // Respawn the enemy on the right side of the window
            }
            if (enemy.sprite.getPosition().x < (sonic.sprite.getPosition().x - 1000))
            {
                enemy.sprite.setPosition(sonic.sprite.getPosition().x + 2500, 598); // Respawn the enemy on the right side of the window
            }
        }

        //collision between sonic and enemy2
        if (!stopFollowingSonic)
        {
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
        }


        //collision between bullets and enemy
        for (int j = 0; j < sonic.bullet.size(); j++)
        {
            if (sonic.bullet[j].bulletSprite.getGlobalBounds().intersects(enemy.sprite.getGlobalBounds())) {
                enemy.health -= 19;
                sonic.bullet[j].bulletSprite.setScale(0, 0);
            }
            if (enemy.health == 0) {
                enemy.sprite.setPosition(sonic.sprite.getPosition().x + 2500, 598);
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


        //animation of coins
        for (int i = 0; i < 70; i++) {
            coinAnimationIndicator += 0.008;
            if (coinAnimationIndicator > 9)
                coinAnimationIndicator -= 9;
            coins[i].setTextureRect(IntRect(int(coinAnimationIndicator) * 64, 0, 64, 64));
        }

        //incrementing scoring
        for (int i = 0; i < 70; i++) {
            if (sonic.sprite.getGlobalBounds().intersects(coins[i].getGlobalBounds())) {
                soundManager.playSound(0);
                coins[i].setScale(0, 0);
                rings++;
                text2.setString(to_string(rings));
            }
        }   

        //Updating sonic
        sonic.update(time, 1.0f / 40.f, ground1);
        if (character == 1)
        {
            sonic.sonic_animation(time);
        }
        else if (character == 2)
        {
            sonic.knucles_animation(time);
        }
        else if (character == 3)
        {
            sonic.tails_animation(time);
        }


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
        timeString = to_string(minutes) + "'" + std::to_string(seconds).substr(0, 2);
        timerText.setString(timeString); // Set the text string

        //Upating history after checking if sonic is dead or not
        if (sonic.damage < 3)
        {
            scoreimage[2].setTextureRect(IntRect(88 - (10 * sonic.damage), 0, 10, 25));
        }
        else
        {
            ofstream offile;
            offile.open("History.txt", ios::app);
            offile << name << "   " << score << "   " << timeString << "   " << rings << "*" << endl;
            soundManager.playSound(3);
            gameover = true;
            break;
        }

        if (!stopFollowingSonic) {
            view.setCenter(Vector2f(sonic.sprite.getPosition().x + 648, 540));
        }


        //checking if the level is finished
        if (sonic.sprite.getPosition().x > 13700)
            stopFollowingSonic = true;


        if (sonic.sprite.getPosition().x < 13700 && stopFollowingSonic)
            sonic.sprite.setPosition(13700, sonic.sprite.getPosition().y);

        if (sonic.sprite.getPosition().x > 14750)
            sonic.sprite.move(10, 0);

        if (sonic.sprite.getPosition().x > 15400) {
            level1isfinished = true;
            soundtrackMusic.pause();
            soundManager.playSound(5);
        }
        if (level3isfinished)
            return;
        window.clear();
        window.setView(view);
        for (int i = 0; i < 18; ++i)
        {
            window.draw(background[i]);
        }

        for (int i = 0; i < 6; i++)
        {
            window.draw(ground[i]);
        }
        for (int i = 0; i < 18; i++)
        {
            window.draw(backgroundimage[i]);
        }
        for (int i = 0; i < 70; i++) {
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
        for (int i = 0; i < numSprites - 1; i++)
        {
            window.draw(sprites[i]);
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
void bossfight(RenderWindow& window)
{
    bossfightlevel = true;

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
    scoreimage[0].setPosition(15, 0);
    scoreimage[1].setTextureRect(IntRect(0, 56, 40, 20));
    scoreimage[1].setPosition(15, 900);
    scoreimage[2].setTextureRect(IntRect(88, 0, 10, 25));
    scoreimage[2].setPosition(165, 885);




    Texture bossbg;
    bossbg.loadFromFile("Textures/Bbg.png");
    Sprite bossbgS;
    bossbgS.setTexture(bossbg);

    Texture bossbg1;
    bossbg1.loadFromFile("Textures/Bbg.png");
    Sprite bossbg1S;
    bossbg1S.setTexture(bossbg1);
    bossbg1S.setPosition(1120, 0);

    Texture bossground;
    bossground.loadFromFile("Textures/bossfightground.png");
    Sprite bossgroundS;
    bossgroundS.setTexture(bossground);
    bossgroundS.setPosition(0, 726);
    bossgroundS.setScale(1, 1);

    //setting blocks
    Texture ground1texture;
    ground1texture.loadFromFile("Textures/bossfightblock.png");
    Sprite ground1[23];
    block(ground1);
    for (int i = 0; i < 3; ++i)
    {
        ground1[i].setTexture(ground1texture);
    }

    //declaring sonic
    Texture sonictexture;
    sonictexture.loadFromFile("Textures/approvedsonic.png");
    player sonic;
    sonic.sprite.setTextureRect(IntRect(0, 0, 50, 55));
    sonic.sprite.setScale(sonic.s1, sonic.s2);
    sonic.sp(sonictexture);
    sonic.groundHeight = 742;

    //Variables
    int coinCount = 0;
    float coinAnimationIndicator = 0;
    bool isCoinVisible = true;
    bool landed = false;
    double velocityY = 0;
    score = 0;
    rings = 0;

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
    text.setPosition(196, 20);
    text2.setPosition(191, 133);
    text.setCharacterSize(32);
    text2.setCharacterSize(32);
    text.setScale(1.45f, 1.45f);
    text2.setScale(1.45f, 1.45f);


    // Declare a Text object for the timer
    Text timerText("", font, 50);
    timerText.setFillColor(Color::White);
    timerText.setOutlineColor(Color::Black);
    timerText.setOutlineThickness(2);
    timerText.setPosition(171, 80);
    timerText.setCharacterSize(32);
    timerText.setScale(1.45f, 1.45f);
    //powerups
    Setdrops();
    SoundManager soundManager;

    // load coin sound 
    SoundBuffer coinBuffer;
    coinBuffer.loadFromFile("Sounds/soundcoin.wav");
    soundManager.addSound(coinBuffer);

    // load the bullet sound
    SoundBuffer bulletBuffer;
    bulletBuffer.loadFromFile("Sounds/lazer3.wav");
    soundManager.addSound(bulletBuffer);

    //load the sound of sonic's death
    SoundBuffer sdBuffer;
    sdBuffer.loadFromFile("Sounds/funnydeath.wav");
    soundManager.addSound(sdBuffer);

    //load heart sound
    SoundBuffer liveBuffer;
    liveBuffer.loadFromFile("Sounds/live.wav");
    Sound liveSound(liveBuffer);

    //load the sound when sonic got the gun
    SoundBuffer gotthegBuffer;
    gotthegBuffer.loadFromFile("Sounds/gotthegun.wav");
    Sound getthegunSound(gotthegBuffer);

    //load sound of level up
    SoundBuffer levelupBuffer;
    levelupBuffer.loadFromFile("Sounds/levelup.wav");
    soundManager.addSound(levelupBuffer);


    // load soundtrack and loop it
    Music soundtrackMusic;
    if (soundtrackMusic.openFromFile("Sounds/boss.wav"))

        soundtrackMusic.setLoop(true);
    if (soundison)
        soundtrackMusic.setVolume(20);
    else if (soundison == false)
        soundtrackMusic.setVolume(0);
    soundtrackMusic.play();



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
        dropCollision(sonic, liveSound, getthegunSound);
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
        soundManager.setVolume(0);

        if (bossfightlevel)
        {
            ground1[0].setPosition(220, 430);
            ground1[0].setScale(1.5, 1);    //small
            ground1[1].setPosition(830, 520);
            ground1[1].setScale(1.5, 1);    //small
            ground1[2].setPosition(1550, 430);
            ground1[2].setScale(1.5, 1);    //small
           
        }
        // Move the player using A,D and space keys
        if (sonic.last_key_pressed == 1) {
            sonic.sprite.setTextureRect(IntRect(0, 0, sonic.w, sonic.h));
            sonic.sprite.setScale(sonic.s1, sonic.s2);
        }
        if (sonic.last_key_pressed == 2) {
            sonic.sprite.setTextureRect(IntRect(0, 0, sonic.w, sonic.h));
            sonic.sprite.setScale(-sonic.s1, sonic.s2);
        }
        if (Keyboard::isKeyPressed(Keyboard::A)) {
            sonic.sprite.setScale(-sonic.s1, sonic.s2);
            sonic.last_key_pressed = 2;
        }
        if (Keyboard::isKeyPressed(Keyboard::D)) {
            sonic.sprite.setScale(sonic.s1, sonic.s2);
            sonic.last_key_pressed = 1;
        }
        if (Mouse::isButtonPressed(Mouse::Left) && sonic.index >= 0 && sonic.canShoot) {
            for (auto& sound : soundManager.sounds) {
                sound.play();
            }
            sonic.bullet[sonic.index].bulletSprite.setPosition(sonic.sprite.getPosition().x, sonic.sprite.getPosition().y);
            sonic.shootCooldown = sonic.bullet[sonic.index].cooldownUse;
            sonic.bullet[sonic.index].moveTo = sonic.last_key_pressed;
            sonic.index--;
            sonic.canShoot = 0;
        }

        //sonic limits
        if (sonic.sprite.getPosition().x < 40) {
            sonic.sprite.setPosition(40, sonic.sprite.getPosition().y);
        }
        if (sonic.sprite.getPosition().y < 0) {
            sonic.sprite.setPosition(sonic.sprite.getPosition().x, 0);
        }
        if (sonic.sprite.getPosition().x > 1880) {
            sonic.sprite.setPosition(1880, sonic.sprite.getPosition().y);
        }

        //Updating sonic
        sonic.update(time, 1.0f / 40.f, ground1);
        if (character == 1)
        {
            sonic.sonic_animation(time);
        }
        else if (character == 2)
        {
            sonic.knucles_animation(time);
        }
        else if (character == 3)
        {
            sonic.tails_animation(time);
        }

        Time elapsedTime = gametime.getElapsedTime();
        int totalSeconds = static_cast<int>(elapsedTime.asSeconds());
        int minutes = (totalSeconds % 3600) / 60;
        int seconds = totalSeconds % 60;

        // Format the time as a string
        timeString = to_string(minutes) + "'" + std::to_string(seconds).substr(0, 2);
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
        window.clear();
        window.draw(bossbg1S);
        window.draw(bossbgS);
        for (int i = 0; i < sonic.bullet.size(); i++)
        {
            window.draw(sonic.bullet[i].bulletSprite);
        }

        for (int i = 0; i < dropBag.size(); i++) {
            window.draw(dropBag[i].dropShape);
        }

        window.draw(sonic.sprite);

        window.draw(text);
        window.draw(text2);
        window.draw(timerText);
        window.draw(bossgroundS);
        for (int i = 0; i < 3; i++) {
            window.draw(ground1[i]);
        }
        for (int i = 0; i < 3; i++) {
            window.draw(scoreimage[i]);
        }
        window.draw(sonic.sprite);
        window.display();
    }
}
void chat(RenderWindow& window)
{
    Texture sonic;
    sonic.loadFromFile("Textures/sonic.png");
    Sprite sonicS;
    sonicS.setTexture(sonic);
    sonicS.setPosition(100, 550);
    sonicS.setScale(0.1, 0.1);

    Texture knucles;
    knucles.loadFromFile("Textures/knucles.png");
    Sprite knuclesS;
    knuclesS.setTexture(knucles);
    knuclesS.setScale(0.37, 0.37);
    knuclesS.setPosition(100, 550);

    Texture tales;
    tales.loadFromFile("Textures/tales.png");
    Sprite talesS;
    talesS.setTexture(tales);
    talesS.setScale(0.315, 0.315);
    talesS.setPosition(70, 530);

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


    Texture eggman;
    eggman.loadFromFile("Textures/eggman.png");
    Sprite eggmanS;
    eggmanS.setTexture(eggman);
    eggmanS.setPosition(1480, 550);
    eggmanS.setScale(0.3, 0.3);



    Texture chat;
    chat.loadFromFile("Textures/chatbox.png");
    Sprite chatbox;
    chatbox.setTexture(chat);
    chatbox.setPosition(15, 780);
    chatbox.setScale(0.65, 0.237);

    Texture arrow;
    arrow.loadFromFile("Textures/arrow.png");
    Sprite arrow1;
    arrow1.setTexture(arrow);
    arrow1.setPosition(1650, 830);
    arrow1.setScale(0.4, 0.4);

    // Create the font
    sf::Font font;
    font.loadFromFile("Fonts/NiseSegaSonic.TTF");

    // Create the text objects
    sf::Text npcText("", font, 28);
    npcText.setPosition(100, 835);
    npcText.setOutlineColor(Color::Black);
    npcText.setOutlineThickness(3);

    sf::Text playerText("", font, 28);
    playerText.setPosition(100, 900);
    playerText.setOutlineColor(Color::Black);
    playerText.setOutlineThickness(3);


    // Set the dialogue text
    vector<string> npcDialogue = { "Ah,I see you're still trying to fail my plans.", "(smirks) I've created a robot that's stronger than you", "Let's find out then who will take over the world " };
    vector<string> playerDialogue = { "And I see you're still trying to take over the world. How original.", "(determined) Not if I stop you first, Eggman. Let's go!", "(smiling) The world is safe as long as I'm around.(runs off)" };

    // Create a variable to keep track of the dialogue state
    int currentDialogueIndex = 0;
    bool isNpcSpeaking = true;
    bool isDialogueFinished = false;

    // Create a variable to keep track of the current character index
    unsigned int characterIndex = 0;

    // Create a clock to measure the time elapsed
    sf::Clock clock;

    // Create a variable to store the condition for displaying the dialogue
    bool isSpriteClicked = false;
    Sprite* characters[] = { &sonicS, &knuclesS, &talesS };

    while (window.isOpen() && !isDialogueFinished)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                // Check if the mouse click is inside the sprite's bounding box
                if (arrow1.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                {
                    // Check if the dialogue is finished
                    if (currentDialogueIndex >= npcDialogue.size())
                    {
                        isDialogueFinished = true;
                    }
                    else
                    {
                        // Remove the old text and reset the variables
                        npcText.setString("");
                        playerText.setString("");
                        characterIndex = 0;
                        isNpcSpeaking = true;
                        isSpriteClicked = true;
                    }
                }
            }
        }
        string charactername;
        if (character == 1)
        {
            charactername = "Sonic: ";
        }
        else if (character == 2)
        {
            charactername = "Knucles: ";
        }
        else if (character == 3)
            charactername = "Tails: ";

        // Only display the dialogue if the condition is true
        if (isSpriteClicked && !isDialogueFinished)
        {
            // Get the elapsed time since the last character was displayed
            Time elapsed = clock.getElapsedTime();

            // Only display the next character if enough time has passed
            if (elapsed.asMilliseconds() > 10) // Change this value to adjust the delay time
            {
                // Display the next character in the dialogue
                if (isNpcSpeaking == true)
                {
                    if (characterIndex < npcDialogue[currentDialogueIndex].size())
                    {
                        npcText.setString("DR.EGGMAN: " + npcDialogue[currentDialogueIndex].substr(0, characterIndex + 1));
                        characterIndex++;
                    }
                    else
                    {
                        // NPC finished speaking, switch to player speaking
                        characterIndex = 0;
                        isNpcSpeaking = false;
                    }
                }
                else
                {
                    if (characterIndex < playerDialogue[currentDialogueIndex].size())
                    {
                        playerText.setString(charactername + playerDialogue[currentDialogueIndex].substr(0, characterIndex + 1));
                        characterIndex++;
                    }
                    else
                    {
                        // Player finished speaking, reset the variables
                        characterIndex = 0;
                        isNpcSpeaking = true;
                        currentDialogueIndex++;
                        isSpriteClicked = false;
                    }
                }

                // Reset the clock
                clock.restart();
            }
        }
        if (isDialogueFinished)
        {
            return;
        }



        // Clear the window
        window.clear();
        for (int i = 0; i < 18; ++i)
        {
            window.draw(background[i]);
        }
        for (int i = 0; i < 8; i++)
        {
            window.draw(ground2P[i]);
        }
        for (int i = 0; i < 2; i++) {
            window.draw(ground[i]);
        }


        // window.draw(greenhillS);
        window.draw(*characters[character - 1]);
        window.draw(eggmanS);
        window.draw(chatbox);
        // Display the NPC's dialogue
        window.draw(npcText);
        // Display the player's dialogue
        window.draw(playerText);
        window.draw(arrow1);

        // Display the window
        window.display();
    }
}
void chat2(RenderWindow& window)
{
    Texture sonic;
    sonic.loadFromFile("Textures/sonic.png");
    Sprite sonicS;
    sonicS.setTexture(sonic);
    sonicS.setPosition(100, 550);
    sonicS.setScale(0.1, 0.1);

    Texture knucles;
    knucles.loadFromFile("Textures/knucles.png");
    Sprite knuclesS;
    knuclesS.setTexture(knucles);
    knuclesS.setScale(0.37, 0.37);
    knuclesS.setPosition(100, 550);

    Texture tales;
    tales.loadFromFile("Textures/tales.png");
    Sprite talesS;
    talesS.setTexture(tales);
    talesS.setScale(0.315, 0.315);
    talesS.setPosition(70, 530);



    //setting background image
    Texture backgroundimagetexture;
    backgroundimagetexture.loadFromFile("Textures/map2top.png");
    Sprite backgroundimage[18];
    for (int i = 0; i < 18; ++i)
    {
        backgroundimage[i].setTexture(backgroundimagetexture);
        backgroundimage[i].setPosition(2400 * i, 210);
        backgroundimage[i].setScale(2, 2);
        backgroundimage[i].setOrigin(500, 0);
    }

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

    Texture eggman;
    eggman.loadFromFile("Textures/eggman.png");
    Sprite eggmanS;
    eggmanS.setTexture(eggman);
    eggmanS.setPosition(1480, 550);
    eggmanS.setScale(0.3, 0.3);



    Texture chat;
    chat.loadFromFile("Textures/chatbox.png");
    Sprite chatbox;
    chatbox.setTexture(chat);
    chatbox.setPosition(15, 780);
    chatbox.setScale(0.65, 0.237);

    Texture arrow;
    arrow.loadFromFile("Textures/arrow.png");
    Sprite arrow1;
    arrow1.setTexture(arrow);
    arrow1.setPosition(1650, 830);
    arrow1.setScale(0.4, 0.4);

    // Create the font
    sf::Font font;
    font.loadFromFile("Fonts/NiseSegaSonic.TTF");

    // Create the text objects
    sf::Text npcText("", font, 28);
    npcText.setPosition(100, 835);
    npcText.setOutlineColor(Color::Black);
    npcText.setOutlineThickness(3);

    sf::Text playerText("", font, 28);
    playerText.setPosition(100, 900);
    playerText.setOutlineColor(Color::Black);
    playerText.setOutlineThickness(3);


    // Set the dialogue text
    vector<string> npcDialogue = { "Ah,I see you're still trying to fail my plans.", "(smirks) I've created a robot that's stronger than you", "Let's find out then who will take over the world " };
    vector<string> playerDialogue = { "And I see you're still trying to take over the world. How original.", "(determined) Not if I stop you first, Eggman. Let's go!", "(smiling) The world is safe as long as I'm around.(runs off)" };

    // Create a variable to keep track of the dialogue state
    int currentDialogueIndex = 0;
    bool isNpcSpeaking = true;
    bool isDialogueFinished = false;

    // Create a variable to keep track of the current character index
    unsigned int characterIndex = 0;

    // Create a clock to measure the time elapsed
    sf::Clock clock;

    // Create a variable to store the condition for displaying the dialogue
    bool isSpriteClicked = false;
    Sprite* characters[] = { &sonicS, &knuclesS, &talesS };

    while (window.isOpen() && !isDialogueFinished)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                // Check if the mouse click is inside the sprite's bounding box
                if (arrow1.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                {
                    // Check if the dialogue is finished
                    if (currentDialogueIndex >= npcDialogue.size())
                    {
                        isDialogueFinished = true;
                    }
                    else
                    {
                        // Remove the old text and reset the variables
                        npcText.setString("");
                        playerText.setString("");
                        characterIndex = 0;
                        isNpcSpeaking = true;
                        isSpriteClicked = true;
                    }
                }
            }
        }
        string charactername;
        if (character == 1)
        {
            charactername = "Sonic: ";
        }
        else if (character == 2)
        {
            charactername = "Knucles: ";
        }
        else if (character == 3)
            charactername = "Tails: ";

        // Only display the dialogue if the condition is true
        if (isSpriteClicked && !isDialogueFinished)
        {
            // Get the elapsed time since the last character was displayed
            Time elapsed = clock.getElapsedTime();

            // Only display the next character if enough time has passed
            if (elapsed.asMilliseconds() > 10) // Change this value to adjust the delay time
            {
                // Display the next character in the dialogue
                if (isNpcSpeaking == true)
                {
                    if (characterIndex < npcDialogue[currentDialogueIndex].size())
                    {
                        npcText.setString("DR.EGGMAN: " + npcDialogue[currentDialogueIndex].substr(0, characterIndex + 1));
                        characterIndex++;
                    }
                    else
                    {
                        // NPC finished speaking, switch to player speaking
                        characterIndex = 0;
                        isNpcSpeaking = false;
                    }
                }
                else
                {
                    if (characterIndex < playerDialogue[currentDialogueIndex].size())
                    {
                        playerText.setString(charactername + playerDialogue[currentDialogueIndex].substr(0, characterIndex + 1));
                        characterIndex++;
                    }
                    else
                    {
                        // Player finished speaking, reset the variables
                        characterIndex = 0;
                        isNpcSpeaking = true;
                        currentDialogueIndex++;
                        isSpriteClicked = false;
                    }
                }

                // Reset the clock
                clock.restart();
            }
        }
        if (isDialogueFinished)
        {
            return;
        }



        // Clear the window
        window.clear();

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
        for (int i = 0; i < 23; i++) {
            window.draw(ground1[i]);
        }

        // window.draw(greenhillS);
        window.draw(*characters[character - 1]);
        window.draw(eggmanS);
        window.draw(chatbox);
        // Display the NPC's dialogue
        window.draw(npcText);
        // Display the player's dialogue
        window.draw(playerText);
        window.draw(arrow1);

        // Display the window
        window.display();
    }
}
void chat3(RenderWindow& window)
{
    Texture sonic;
    sonic.loadFromFile("Textures/sonic.png");
    Sprite sonicS;
    sonicS.setTexture(sonic);
    sonicS.setPosition(100, 550);
    sonicS.setScale(0.1, 0.1);

    Texture knucles;
    knucles.loadFromFile("Textures/knucles.png");
    Sprite knuclesS;
    knuclesS.setTexture(knucles);
    knuclesS.setScale(0.37, 0.37);
    knuclesS.setPosition(100, 550);

    Texture tales;
    tales.loadFromFile("Textures/tales.png");
    Sprite talesS;
    talesS.setTexture(tales);
    talesS.setScale(0.315, 0.315);
    talesS.setPosition(70, 530);

    //setting ground
    Texture groundtexture;
    groundtexture.loadFromFile("Textures/map3ground.png");
    Sprite ground[6];
    for (int i = 0; i < 3; i++)
    {
        ground[i].setTexture(groundtexture);
        ground[i].setPosition(Vector2f(i * 5408, 486));
        ground[i].setScale(2.4f, 2.4f);
        ground[i].setOrigin(200, 0);
    }
    //setting background image
    Texture backgroundimagetexture;
    backgroundimagetexture.loadFromFile("Textures/map3top.png");
    Sprite backgroundimage[18];
    for (int i = 0; i < 18; ++i)
    {
        backgroundimage[i].setTexture(backgroundimagetexture);
        backgroundimage[i].setPosition(5062 * i, -82);
        backgroundimage[i].setScale(2.7, 2.7);
        backgroundimage[i].setOrigin(700, 0);
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


    Texture eggman;
    eggman.loadFromFile("Textures/eggman.png");
    Sprite eggmanS;
    eggmanS.setTexture(eggman);
    eggmanS.setPosition(1480, 550);
    eggmanS.setScale(0.3, 0.3);



    Texture chat;
    chat.loadFromFile("Textures/chatbox.png");
    Sprite chatbox;
    chatbox.setTexture(chat);
    chatbox.setPosition(15, 780);
    chatbox.setScale(0.65, 0.237);

    Texture arrow;
    arrow.loadFromFile("Textures/arrow.png");
    Sprite arrow1;
    arrow1.setTexture(arrow);
    arrow1.setPosition(1650, 830);
    arrow1.setScale(0.4, 0.4);

    // Create the font
    sf::Font font;
    font.loadFromFile("Fonts/NiseSegaSonic.TTF");

    // Create the text objects
    sf::Text npcText("", font, 28);
    npcText.setPosition(100, 835);
    npcText.setOutlineColor(Color::Black);
    npcText.setOutlineThickness(3);

    sf::Text playerText("", font, 28);
    playerText.setPosition(100, 900);
    playerText.setOutlineColor(Color::Black);
    playerText.setOutlineThickness(3);


    // Set the dialogue text
    vector<string> npcDialogue = { "Ah,I see you're still trying to fail my plans.", "(smirks) I've created a robot that's stronger than you", "Let's find out then who will take over the world " };
    vector<string> playerDialogue = { "And I see you're still trying to take over the world. How original.", "(determined) Not if I stop you first, Eggman. Let's go!", "(smiling) The world is safe as long as I'm around.(runs off)" };

    // Create a variable to keep track of the dialogue state
    int currentDialogueIndex = 0;
    bool isNpcSpeaking = true;
    bool isDialogueFinished = false;

    // Create a variable to keep track of the current character index
    unsigned int characterIndex = 0;

    // Create a clock to measure the time elapsed
    sf::Clock clock;

    // Create a variable to store the condition for displaying the dialogue
    bool isSpriteClicked = false;
    Sprite* characters[] = { &sonicS, &knuclesS, &talesS };

    while (window.isOpen() && !isDialogueFinished)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                // Check if the mouse click is inside the sprite's bounding box
                if (arrow1.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                {
                    // Check if the dialogue is finished
                    if (currentDialogueIndex >= npcDialogue.size())
                    {
                        isDialogueFinished = true;
                    }
                    else
                    {
                        // Remove the old text and reset the variables
                        npcText.setString("");
                        playerText.setString("");
                        characterIndex = 0;
                        isNpcSpeaking = true;
                        isSpriteClicked = true;
                    }
                }
            }
        }
        string charactername;
        if (character == 1)
        {
            charactername = "Sonic: ";
        }
        else if (character == 2)
        {
            charactername = "Knucles: ";
        }
        else if (character == 3)
            charactername = "Tails: ";

        // Only display the dialogue if the condition is true
        if (isSpriteClicked && !isDialogueFinished)
        {
            // Get the elapsed time since the last character was displayed
            Time elapsed = clock.getElapsedTime();

            // Only display the next character if enough time has passed
            if (elapsed.asMilliseconds() > 10) // Change this value to adjust the delay time
            {
                // Display the next character in the dialogue
                if (isNpcSpeaking == true)
                {
                    if (characterIndex < npcDialogue[currentDialogueIndex].size())
                    {
                        npcText.setString("DR.EGGMAN: " + npcDialogue[currentDialogueIndex].substr(0, characterIndex + 1));
                        characterIndex++;
                    }
                    else
                    {
                        // NPC finished speaking, switch to player speaking
                        characterIndex = 0;
                        isNpcSpeaking = false;
                    }
                }
                else
                {
                    if (characterIndex < playerDialogue[currentDialogueIndex].size())
                    {
                        playerText.setString(charactername + playerDialogue[currentDialogueIndex].substr(0, characterIndex + 1));
                        characterIndex++;
                    }
                    else
                    {
                        // Player finished speaking, reset the variables
                        characterIndex = 0;
                        isNpcSpeaking = true;
                        currentDialogueIndex++;
                        isSpriteClicked = false;
                    }
                }

                // Reset the clock
                clock.restart();
            }
        }
        if (isDialogueFinished)
        {
            return;
        }



        // Clear the window
        window.clear();
        for (int i = 0; i < 18; ++i)
        {
            window.draw(background[i]);
        }
        for (int i = 0; i < 6; i++)
        {
            window.draw(ground[i]);
        }
        for (int i = 0; i < 18; i++)
        {
            window.draw(backgroundimage[i]);
        }
        for (int i = 0; i < 23; i++) {
            window.draw(ground1[i]);
        }


        // window.draw(greenhillS);
        window.draw(*characters[character - 1]);
        window.draw(eggmanS);
        window.draw(chatbox);
        // Display the NPC's dialogue
        window.draw(npcText);
        // Display the player's dialogue
        window.draw(playerText);
        window.draw(arrow1);

        // Display the window
        window.display();
    }
}
void chatboss(RenderWindow& window)
{
    Texture sonic;
    sonic.loadFromFile("Textures/sonic.png");
    Sprite sonicS;
    sonicS.setTexture(sonic);
    sonicS.setPosition(100, 550);
    sonicS.setScale(0.1, 0.1);

    Texture knucles;
    knucles.loadFromFile("Textures/knucles.png");
    Sprite knuclesS;
    knuclesS.setTexture(knucles);
    knuclesS.setScale(0.37, 0.37);
    knuclesS.setPosition(100, 550);

    Texture tales;
    tales.loadFromFile("Textures/tales.png");
    Sprite talesS;
    talesS.setTexture(tales);
    talesS.setScale(0.315, 0.315);
    talesS.setPosition(70, 530);

    Texture bossbg1;
    bossbg1.loadFromFile("Textures/Bbg.png");
    Sprite bossbg1S;
    bossbg1S.setTexture(bossbg1);
    bossbg1S.setPosition(1120, 0);

    Texture bossbg;
    bossbg.loadFromFile("Textures/Bbg.png");
    Sprite bossbgS;
    bossbgS.setTexture(bossbg);

    Texture bossground;
    bossground.loadFromFile("Textures/bossfightground.png");
    Sprite bossgroundS;
    bossgroundS.setTexture(bossground);
    bossgroundS.setPosition(0, 726);
    bossgroundS.setScale(1, 1);

    //setting blocks
    Texture ground1texture;
    ground1texture.loadFromFile("Textures/bossfightblock.png");
    Sprite ground1[23];
    block(ground1);
    for (int i = 0; i < 3; ++i)
    {
        ground1[i].setTexture(ground1texture);
    }
    Texture eggman;
    eggman.loadFromFile("Textures/eggman.png");
    Sprite eggmanS;
    eggmanS.setTexture(eggman);
    eggmanS.setPosition(1480, 550);
    eggmanS.setScale(0.3, 0.3);



    Texture chat;
    chat.loadFromFile("Textures/chatbox.png");
    Sprite chatbox;
    chatbox.setTexture(chat);
    chatbox.setPosition(15, 780);
    chatbox.setScale(0.65, 0.237);

    Texture arrow;
    arrow.loadFromFile("Textures/arrow.png");
    Sprite arrow1;
    arrow1.setTexture(arrow);
    arrow1.setPosition(1650, 830);
    arrow1.setScale(0.4, 0.4);

    // Create the font
    sf::Font font;
    font.loadFromFile("Fonts/NiseSegaSonic.TTF");

    // Create the text objects
    sf::Text npcText("", font, 28);
    npcText.setPosition(100, 835);
    npcText.setOutlineColor(Color::Black);
    npcText.setOutlineThickness(3);

    sf::Text playerText("", font, 28);
    playerText.setPosition(100, 900);
    playerText.setOutlineColor(Color::Black);
    playerText.setOutlineThickness(3);


    // Set the dialogue text
    vector<string> npcDialogue = { "Ah,I see you're still trying to fail my plans.", "(smirks) I've created a robot that's stronger than you", "Let's find out then who will take over the world " };
    vector<string> playerDialogue = { "And I see you're still trying to take over the world. How original.", "(determined) Not if I stop you first, Eggman. Let's go!", "(smiling) The world is safe as long as I'm around.(runs off)" };

    // Create a variable to keep track of the dialogue state
    int currentDialogueIndex = 0;
    bool isNpcSpeaking = true;
    bool isDialogueFinished = false;

    // Create a variable to keep track of the current character index
    unsigned int characterIndex = 0;

    // Create a clock to measure the time elapsed
    sf::Clock clock;

    // Create a variable to store the condition for displaying the dialogue
    bool isSpriteClicked = false;
    Sprite* characters[] = { &sonicS, &knuclesS, &talesS };

    while (window.isOpen() && !isDialogueFinished)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                // Check if the mouse click is inside the sprite's bounding box
                if (arrow1.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                {
                    // Check if the dialogue is finished
                    if (currentDialogueIndex >= npcDialogue.size())
                    {
                        isDialogueFinished = true;
                    }
                    else
                    {
                        // Remove the old text and reset the variables
                        npcText.setString("");
                        playerText.setString("");
                        characterIndex = 0;
                        isNpcSpeaking = true;
                        isSpriteClicked = true;
                    }
                }
            }
        }
        string charactername;
        if (character == 1)
        {
            charactername = "Sonic: ";
        }
        else if (character == 2)
        {
            charactername = "Knucles: ";
        }
        else if (character == 3)
            charactername = "Tails: ";

        // Only display the dialogue if the condition is true
        if (isSpriteClicked && !isDialogueFinished)
        {
            // Get the elapsed time since the last character was displayed
            Time elapsed = clock.getElapsedTime();

            // Only display the next character if enough time has passed
            if (elapsed.asMilliseconds() > 10) // Change this value to adjust the delay time
            {
                // Display the next character in the dialogue
                if (isNpcSpeaking == true)
                {
                    if (characterIndex < npcDialogue[currentDialogueIndex].size())
                    {
                        npcText.setString("DR.EGGMAN: " + npcDialogue[currentDialogueIndex].substr(0, characterIndex + 1));
                        characterIndex++;
                    }
                    else
                    {
                        // NPC finished speaking, switch to player speaking
                        characterIndex = 0;
                        isNpcSpeaking = false;
                    }
                }
                else
                {
                    if (characterIndex < playerDialogue[currentDialogueIndex].size())
                    {
                        playerText.setString(charactername + playerDialogue[currentDialogueIndex].substr(0, characterIndex + 1));
                        characterIndex++;
                    }
                    else
                    {
                        // Player finished speaking, reset the variables
                        characterIndex = 0;
                        isNpcSpeaking = true;
                        currentDialogueIndex++;
                        isSpriteClicked = false;
                    }
                }

                // Reset the clock
                clock.restart();
            }
        }
        if (isDialogueFinished)
        {
            return;
        }



        // Clear the window
        window.clear();
        window.draw(bossbg1S);
        window.draw(bossbgS);
        window.draw(bossgroundS);
        for (int i = 0; i < 3; i++) {
            window.draw(ground1[i]);
        }
        // window.draw(greenhillS);
        window.draw(*characters[character - 1]);
        window.draw(eggmanS);
        window.draw(chatbox);
        // Display the NPC's dialogue
        window.draw(npcText);
        // Display the player's dialogue
        window.draw(playerText);
        window.draw(arrow1);

        // Display the window
        window.display();
    }
}
void selectlevel(RenderWindow& window)
{
    Texture level;
    level.loadFromFile("Textures/main2.jpg");
    Sprite levelz;
    levelz.setTexture(level);

    Texture lock;
    lock.loadFromFile("Textures/locks.png");
    Sprite locks;
    locks.setTexture(lock);
    locks.setPosition(720, 320);
    locks.setScale(0.85, 0.4);


    Texture lock1;
    lock1.loadFromFile("Textures/locks.png");
    Sprite locks1;
    locks1.setTexture(lock1);
    locks1.setPosition(1320, 320);
    locks1.setScale(0.85, 0.4);

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
                     chat(window);
                    GamePlay(window, level1isfinished);
                    if (level1isfinished)
                    {
                        chat2(window);
                        GamePlay2(window, level2isfinished);
                    }
                    if (level2isfinished)
                    {
                        chat3(window);
                        GamePlay3(window, level3isfinished);
                    }
                    if (level3isfinished)
                    {
                        //chatboss(window);
                        //bossfight(window);
                    }
                    window.close();
                    return;
                }
            }
            Vector2i mousePosition1 = sf::Mouse::getPosition(window);
            FloatRect spriteBounds1 = levelz2.getGlobalBounds();
            levelz2.setScale(1, 1);
            locks.setScale(0.85, 0.35);
            if (spriteBounds1.contains(mousePosition1.x, mousePosition1.y))
            {
                levelz2.setScale(1.2, 1.2);
                locks.setScale(1.0, 0.45);

                if (Mouse::isButtonPressed(Mouse::Left)) {
                    chat2(window);
                    GamePlay2(window, level2isfinished);
                    if (level2isfinished)
                    {
                        chat3(window);
                        GamePlay3(window, level3isfinished);
                    }
                    if (level3isfinished) {
                       // chatboss(window);
                        //bossfight(window);
                    }
                    window.close();
                    return;
                }
            }

            Vector2i mousePosition2 = Mouse::getPosition(window);
            FloatRect spriteBounds2 = levelz3.getGlobalBounds();
            levelz3.setScale(1, 1);
            locks1.setScale(0.85, 0.35);
            if (spriteBounds2.contains(mousePosition2.x, mousePosition2.y))
            {
                levelz3.setScale(1.2, 1.2);
                locks1.setScale(1.0, 0.45);
                if (Mouse::isButtonPressed(Mouse::Left) && level2isfinished) {
                    chat3(window);
                    GamePlay3(window, level3isfinished);
                    if (level3isfinished) {
                      //  chatboss(window);
                      //  bossfight(window);
                    }
                    window.close();
                    return;
                }
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
        if (!level1isfinished)
            window.draw(locks);
        if (!level2isfinished)
            window.draw(locks1);
        window.draw(Gameover);
        window.display();
    }
}
void playerSelection(RenderWindow& window, int& character)
{
    bool sonicC = true;
    bool knuclesC = false;
    bool talesC = false;
    Font font;
    font.loadFromFile("Fonts/NiseSegaSonic.TTF"); // load font lel window 
    Text t1;
    t1.setFont(font);
    t1.setString("Select Your Character");
    t1.setCharacterSize(50);
    t1.setOutlineColor(Color::Black);
    t1.setOutlineThickness(4);
    t1.setPosition(550, 100);

    Texture sonic;
    sonic.loadFromFile("Textures/sonic.png");
    Sprite sonicS;
    sonicS.setTexture(sonic);
    sonicS.setScale(0.13, 0.13);
    sonicS.setPosition(815, 300);

    Texture knucles;
    knucles.loadFromFile("Textures/knucles.png");
    Sprite knuclesS;
    knuclesS.setTexture(knucles);
    knuclesS.setScale(0.37, 0.37);
    knuclesS.setPosition(820, 340);

    Texture tales;
    tales.loadFromFile("Textures/tales.png");
    Sprite talesS;
    talesS.setTexture(tales);
    talesS.setScale(0.365, 0.365);
    talesS.setPosition(750, 310);

    Texture level;
    level.loadFromFile("Textures/main2.jpg");
    Sprite levelz;
    levelz.setTexture(level);

    Texture arrow;
    arrow.loadFromFile("Textures/arrow.png");
    Sprite arrow1;
    arrow1.setTexture(arrow);
    arrow1.setPosition(750, 550);
    arrow1.setScale(-0.4, 0.4);

    Sprite arrow2;
    arrow2.setTexture(arrow);
    arrow2.setPosition(1100, 550);
    arrow2.setScale(0.4, 0.4);

    Texture choose;
    choose.loadFromFile("Textures/choose.png");
    Sprite chooseS;
    chooseS.setTexture(choose);
    chooseS.setPosition(745, 800);
    chooseS.setScale(0.8, 0.8);

    int currentCharacterIndex = 0;
    Sprite* characters[] = { &sonicS, &knuclesS, &talesS };

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {

                    if (arrow2.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                        currentCharacterIndex = (currentCharacterIndex + 1) % 3;
                    if (arrow1.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                        currentCharacterIndex = (currentCharacterIndex + 2) % 3;
                }
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    if (chooseS.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                    {
                        character = currentCharacterIndex + 1;
                        selectlevel(window);
                    }

                }
            }
        }
        window.clear();
        window.draw(levelz);
        window.draw(t1);
        window.draw(chooseS);
        window.draw(*characters[currentCharacterIndex]);
        window.draw(arrow1);
        window.draw(arrow2);
        window.display();
    }
}
void Controls()
{
RenderWindow window(sf::VideoMode(1920, 1080), "Controls");
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



    while (window.isOpen())
    {
        Event aevent;
        while (window.pollEvent(aevent))
        {
            if (aevent.type == Event::Closed)
            {
                window.close();

            }
            if (aevent.key.code == Keyboard::Escape)
            {
                window.close();

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
        i++;
        if (i > 2) {
            i = 0;
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
        window.clear();
        window.draw(controlbg);
        window.draw(bluebg);
        window.draw(rectangle1);
        window.draw(rectangle);
        window.draw(text1);
        window.draw(keys2);
        window.draw(keys4);
        window.draw(keys5);
        window.draw(keysR);
        window.draw(keysblock1);
        window.draw(keysblock2);
        window.draw(keysblock3);
        window.draw(keysS);
        window.draw(keysL);
        window.display();

    }
}
void SoundOption()
{
    RenderWindow window(sf::VideoMode(1920, 1080), "Sounds");

    Font font1;
    font1.loadFromFile("Fonts/NiseSegaSonic.TTF");
    Text text1,t,t1;
    text1.setFont(font1);
    text1.setString("SOUNDS");
    text1.setPosition(757, 170);
    text1.setCharacterSize(70);
    text1.setFillColor(Color::White);
    text1.setOutlineColor(Color::Black);
    text1.setOutlineThickness(5);

    t.setFont(font1);
    t.setString("Sound Off");
    t.setPosition(757, 550);
    t.setCharacterSize(40);
    t.setFillColor(Color::White);
    t.setOutlineColor(Color::Black);
    t.setOutlineThickness(3);

    t1.setFont(font1);
    t1.setString("Sound On");
    t1.setPosition(757, 350);
    t1.setCharacterSize(40);
    t1.setFillColor(Color::White);
    t1.setOutlineColor(Color::Black);
    t1.setOutlineThickness(3);

    Texture rounded;
    rounded.loadFromFile("Textures/roundedsquare.png");
    Sprite square(rounded);
    square.setPosition(600,250);
    square.setScale(5, 5);

    Texture Son;
    Son.loadFromFile("Textures/soundon.png");
    Sprite SonS(Son);
    SonS.setPosition(1100, 340);
    SonS.setScale(4, 4);

    Texture Soff;
    Soff.loadFromFile("Textures/soundoff.png");
    Sprite SoffS(Soff);
    SoffS.setPosition(1100, 540);
    SoffS.setScale(4, 4);

    Texture backbg;
    backbg.loadFromFile("Textures/main2.jpg");
    Sprite main(backbg);

    Texture tieT;
    tieT.loadFromFile("Textures/tie.png");
    Sprite tieS;
    tieS.setTexture(tieT);
    tieS.setPosition(692, 248);
    tieS.setScale(2, 2);


    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            Vector2i mousePosition = Mouse::getPosition(window);
            FloatRect spriteBounds = SoffS.getGlobalBounds();
            SoffS.setScale(4, 4);

            if (spriteBounds.contains(mousePosition.x, mousePosition.y))
            {
                SoffS.setScale(3.5, 3.5);

                if (Mouse::isButtonPressed(Mouse::Left))
                {
                    soundison = false;
                }
            }
            Vector2i mousePosition1 = Mouse::getPosition(window);
            FloatRect spriteBounds1 = SonS.getGlobalBounds();
            SonS.setScale(4, 4);

            if (spriteBounds1.contains(mousePosition1.x, mousePosition1.y))
            {
                SonS.setScale(3.5, 3.5);
                if (Mouse::isButtonPressed(Mouse::Left)) 
                {
                    soundison = true;
                }
            }
              


        }
        window.clear();
        window.draw(main);
        window.draw(square);
        window.draw(text1);
        window.draw(t);
        window.draw(t1);
        window.draw(SonS);
        window.draw(SoffS);
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

    Font fontmain;
    fontmain.loadFromFile("Fonts/NiseSegaSonic.TTF");
    Text tP, tO, tH, tE;

    tP.setFont(fontmain);
    tP.setFillColor(Color::White);
    tP.setString("Play");
    tP.setCharacterSize(50);
    tP.setPosition(360, 200);
    tP.setOutlineColor(Color::Black);
    tP.setOutlineThickness(6);

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
                    RenderWindow Options(VideoMode(1920, 1080), "Options");
                    RenderWindow About(VideoMode(1920, 1080), "History");


                    int x = mainmenu.pressed();
                    if (x == 0)
                    {
                        Options.close();
                        About.close();
                        playername(entername, window, name);
                        playerSelection(window, character);
                        selectlevel(window);
                        if (level3isfinished)
                            chatboss(window);
                            bossfight(window);

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

                        Font font1;
                        font1.loadFromFile("Fonts/NiseSegaSonic.TTF");
                        Text text1;
                        Text tS;
                        text1.setFont(font1);
                        text1.setString("CONTROLS");
                        text1.setPosition(450, 100);
                        text1.setCharacterSize(70);
                        text1.setFillColor(Color::White);
                        text1.setOutlineColor(Color::Black);
                        text1.setOutlineThickness(3);

                        tS.setFont(font1);
                        tS.setString("SOUNDS");
                        tS.setPosition(1150, 100);
                        tS.setCharacterSize(70);
                        tS.setFillColor(Color::White);
                        tS.setOutlineColor(Color::Black);
                        tS.setOutlineThickness(3);

                      

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

                                Vector2i mousePosition = Mouse::getPosition(Options);
                                FloatRect spriteBounds = text1.getGlobalBounds();
                                text1.setScale(1, 1);

                                if (spriteBounds.contains(mousePosition.x, mousePosition.y))
                                {
                                    text1.setScale(1.2, 1.2);

                                    if (Mouse::isButtonPressed(Mouse::Left)) {
                                        Controls();
                                    }

                                }
                                    Vector2i mousePosition1 = Mouse::getPosition(Options);
                                    FloatRect spriteBounds1 = tS.getGlobalBounds();
                                    tS.setScale(1, 1);

                                    if (spriteBounds1.contains(mousePosition1.x, mousePosition1.y))
                                    {
                                        tS.setScale(1.2, 1.2);

                                        if (Mouse::isButtonPressed(Mouse::Left)) {
                                            SoundOption();
                                        }
                                    }
                                
                            }

                            
                            window.close();
                            About.close();
                            entername.close();
                            Options.clear();
                            Options.draw(text1);
                            Options.draw(tS);
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
                        gameOver(gameover, score, rings, timeString);
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
