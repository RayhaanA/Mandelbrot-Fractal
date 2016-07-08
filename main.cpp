#include "stdafx.h"
#include "SFML\Window.hpp"
#include "SFML\Graphics.hpp"
#include <iostream>

//Global window dimensions
const unsigned int HEIGHT = 750;
const unsigned int WIDTH = 800;

int main()
{
	//Colour map gotten from user "5chdn" on stackoverflow from this thread:
	//http://stackoverflow.com/questions/16500656/which-color-gradient-is-used-to-color-mandelbrot-in-wikipedia
	sf::Color colours[16];
	colours[0] = sf::Color(66, 30, 15);
	colours[1] = sf::Color(25, 7, 26);
	colours[2] = sf::Color(9, 1, 47);
	colours[3] = sf::Color(4, 4, 73);
	colours[4] = sf::Color(0, 7, 100);
	colours[5] = sf::Color(12, 44, 138);
	colours[6] = sf::Color(24, 82, 177);
	colours[7] = sf::Color(57, 125, 209);
	colours[8] = sf::Color(134, 181, 229);
	colours[9] = sf::Color(211, 236, 248);
	colours[10] = sf::Color(241, 233, 191);
	colours[11] = sf::Color(248, 201, 95);
	colours[12] = sf::Color(255, 170, 0);
	colours[13] = sf::Color(204, 128, 0);
	colours[14] = sf::Color(153, 87, 0);
	colours[15] = sf::Color(106, 52, 3);

	//Var to store final colour index
	int * selection = new int(0);

	//Function to create image
	sf::Image createImage(unsigned int maxIterations, sf::Color colours[], sf::Image image, int * selection, float scale);

	//Initialize window
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Mandelbrot Fractals", sf::Style::Close);

	//Texture and sprite for texturing and drawing to window
	sf::Sprite mandelbrot;
	sf::Texture texture;

	//Modifiable window view, plus variable for scale based on zoom -- affects translation speed
	float centerX = window.getDefaultView().getCenter().x, centerY = window.getDefaultView().getCenter().y;
	sf::View view(window.getDefaultView());
	float scale = 1.0f;

	//Sets max number of iterations, varies based on the scale value
	unsigned int maxIterations = 50;

	//Create mandelbrot image
	sf::Image image;
	image.create(WIDTH, HEIGHT, sf::Color::Black);
	image = createImage(maxIterations, colours, image, selection, scale);

	//bool so that sprite isn't constantly updated
	bool texturesSet = false;

	while (window.isOpen())
	{
		sf::Event event;

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (!texturesSet)
		{
			texture.loadFromImage(image);
			mandelbrot.setTexture(texture);
			texturesSet = true;
		}

		//Clear window
		window.clear(colours[*selection]);

		window.setView(view);
		window.draw(mandelbrot);
		window.display();

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Dash))
			view.zoom(1.0005f);
	

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Equal))
			view.zoom(0.9995f);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			if(view.getCenter().x >= 0)
				centerX -= 0.25;
			view.setCenter(sf::Vector2f(centerX, centerY));
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			if(view.getCenter().x <= WIDTH)
				centerX += 0.25;
			view.setCenter(sf::Vector2f(centerX, centerY));
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			if (view.getCenter().y >= 0)
				centerY -= 0.25;
			view.setCenter(sf::Vector2f(centerX, centerY));
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			if (view.getCenter().y <= HEIGHT)
				centerY += 0.25;
			view.setCenter(sf::Vector2f(centerX, centerY));
		}
		
	}
	delete selection;
	selection = nullptr;
	return 0;
}

//Function to create image was learned from Joni Salonen at http://jonisalonen.com/2013/lets-draw-the-mandelbrot-set/
sf::Image createImage(unsigned int maxIterations, sf::Color colours[], sf::Image image, int * selection, float scale)
{
	*selection = 0;
	//Function creates the image pixel by pixel hence the nested for loop for the entire image
	for (int row = 0; row < HEIGHT; row++)
	{
		for (int col = 0; col < WIDTH; col++)
		{
			//Calculate the c value within the mandelbrot set based on the pixel location
			double rC = (col - WIDTH / 2.0) * 4.0 / WIDTH / scale;
			double iC = (row - HEIGHT / 2.0) * 4.0 / WIDTH / scale;

			//Initialize the values for the next iteration of the set
			double x = 0, y = 0;
			int iteration = 0;

			//Constraint is that the plotted values are within a circle of radius 2
			//As defined in the mandelbrot set
			while (x*x + y*y <= 4 && iteration < maxIterations)
			{
				//Find the next set of values for the set 
				double newX = x*x - y*y + rC;
				y = 2 * x*y + iC;
				x = newX;

				iteration++;
			}

			//Colour selection, uses the iteration value to choose the colour index
			*selection = iteration % 16;

			if (iteration < maxIterations)
				image.setPixel(col, row, colours[*selection]);
			else
				image.setPixel(col, row, sf::Color::Black);
		}
	}
	return image;
}