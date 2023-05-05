#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;
using namespace sf;

#define buttons 4
class Menu
{
public:

	Menu(float width, float height);
	void draw(RenderWindow& window);
	void moveup();
	void movedown();

	int pressed()
	{
		return selected;

	}
	~Menu();


private:
	int selected;
	Font font;
	Text mainmenu[buttons];
};
