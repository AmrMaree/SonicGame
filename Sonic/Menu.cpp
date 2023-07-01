#include "Menu.h"


Menu::Menu(float width, float height)
{
	if (!font.loadFromFile("Fonts/NiseSegaSonic.TTF"))
	{
		cout << "font is not here ";

	}
	// play 
	mainmenu[0].setFont(font);
	mainmenu[0].setFillColor(Color::White);
	mainmenu[0].setString("Play");
	mainmenu[0].setCharacterSize(50);
	mainmenu[0].setPosition(850, 420);
	mainmenu[0].setOutlineColor(Color::Black);
	mainmenu[0].setOutlineThickness(6);

	//options
	mainmenu[1].setFont(font);
	mainmenu[1].setFillColor(Color::White);
	mainmenu[1].setString("Options");
	mainmenu[1].setCharacterSize(50);
	mainmenu[1].setPosition(790, 521);
	mainmenu[1].setOutlineColor(Color::Black);
	mainmenu[1].setOutlineThickness(6);


	//About
	mainmenu[2].setFont(font);
	mainmenu[2].setFillColor(Color::White);
	mainmenu[2].setString("History");
	mainmenu[2].setCharacterSize(50);
	mainmenu[2].setPosition(805, 620);
	mainmenu[2].setOutlineColor(Color::Black);
	mainmenu[2].setOutlineThickness(6);

	//Exit 
	mainmenu[3].setFont(font);
	mainmenu[3].setFillColor(Color::White);
	mainmenu[3].setString("Exit");
	mainmenu[3].setCharacterSize(50);
	mainmenu[3].setPosition(865, 720);
	mainmenu[3].setOutlineColor(Color::Black);
	mainmenu[3].setOutlineThickness(6);

	selected = -1;

}

Menu::~Menu()
{


}

//draw mainmenu

void Menu::draw(RenderWindow& window)
{
	for (int i = 0; i < buttons; i++)
	{
		window.draw(mainmenu[i]);
	}
}

//Move up 

void Menu::moveup()
{
	if (selected - 1 >= 0)
	{

		mainmenu[selected].setFillColor(Color::White);
		mainmenu[selected].setScale(1, 1);
		selected--;
		if (selected == -1)
		{
			selected = 2;
			mainmenu[selected].setScale(1.3, 1.3);
		}
		mainmenu[selected].setFillColor(Color::Yellow);
		mainmenu[selected].setScale(1.3, 1.3);
	}
}

//Move down
void Menu::movedown()
{
	if (selected + 1 <= buttons)
	{
		mainmenu[selected].setFillColor(Color::White);
		mainmenu[selected].setScale(1, 1);
		selected++;
		if (selected == 4)
		{
			selected = 0;
			mainmenu[selected].setScale(1.3, 1.3);

		}
		mainmenu[selected].setFillColor(Color::Yellow);
		mainmenu[selected].setScale(1.3, 1.3);

	}

}