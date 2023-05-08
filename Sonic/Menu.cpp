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
	mainmenu[0].setCharacterSize(30);
	mainmenu[0].setPosition(865, 700);
	mainmenu[0].setOutlineColor(Color::Black);
	mainmenu[0].setOutlineThickness(3);

	//options
	mainmenu[1].setFont(font);
	mainmenu[1].setFillColor(Color::White);
	mainmenu[1].setString("Controls");
	mainmenu[1].setCharacterSize(30);
	mainmenu[1].setPosition(815, 750);
	mainmenu[1].setOutlineColor(Color::Black);
	mainmenu[1].setOutlineThickness(3);


	//About
	mainmenu[2].setFont(font);
	mainmenu[2].setFillColor(Color::White);
	mainmenu[2].setString("Leaderboard");
	mainmenu[2].setCharacterSize(30);
	mainmenu[2].setPosition(785, 800);
	mainmenu[2].setOutlineColor(Color::Black);
	mainmenu[2].setOutlineThickness(3);

	//Exit 
	mainmenu[3].setFont(font);
	mainmenu[3].setFillColor(Color::White);
	mainmenu[3].setString("Exit");
	mainmenu[3].setCharacterSize(30);
	mainmenu[3].setPosition(875, 850);
	mainmenu[3].setOutlineColor(Color::Black);
	mainmenu[3].setOutlineThickness(3);

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
		selected--;
		if (selected == -1)
		{
			selected = 2;
		}
		mainmenu[selected].setFillColor(Color::Blue);

	}

}

//Move down
void Menu::movedown()
{
	if (selected + 1 <= buttons)
	{
		mainmenu[selected].setFillColor(Color::White);
		selected++;
		if (selected == 4)
		{
			selected = 0;

		}
		mainmenu[selected].setFillColor(Color::Blue);
	}
}