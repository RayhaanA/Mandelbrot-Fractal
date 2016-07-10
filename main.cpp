#include "SFML\Window.hpp"
#include "SFML\Graphics.hpp"
#include <iostream>

//Global window dimensions
const unsigned int HEIGHT = 600;
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

	//Function to create mandelbrot fractal
	sf::Image createMandelbrot(unsigned int maxIterations, sf::Color colours[], sf::Image image, double minRe, double maxRe, double minIm, double maxIm);

	//Initialize window
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Mandelbrot Fractals", sf::Style::Close);

	//Texture and sprite for texturing and drawing to window
	sf::Sprite mandelbrot;
	sf::Texture texture;

	//Text for displaying info on window
	
	// Load font
	sf::Font font;
	font.loadFromFile("./res/Forque.ttf");
	
	// Create a text
	sf::Text mouseXText("", font);
	mouseXText.setCharacterSize(16);
	mouseXText.setColor(sf::Color::White);
	mouseXText.setPosition(0, 0);
	sf::Text mouseYText("", font);
	mouseYText.setCharacterSize(16);
	mouseYText.setColor(sf::Color::White);
	mouseYText.setPosition(0, 16);

	//Sets max number of iterations, varies based on the scale value
	auto maxIterations = 60;

	//Virtual coordinate extremities
	auto minRe = -2.0;
	auto maxRe = 1.0;
	auto minIm = -1.125;
	auto maxIm = 1.125;

	//Create mandelbrot image
	sf::Image image;
	image.create(WIDTH, HEIGHT, sf::Color::Black);
	image = createMandelbrot(maxIterations, colours, image, minRe, maxRe, minIm, maxIm);

	//bool so that sprite isn't constantly updated
	bool textureSet = false;

	while (window.isOpen())
	{
		sf::Event event;

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::R)
				{
					std::cout << "Resetting image... " << std::endl;;
					textureSet = false;
					window.clear(sf::Color::Black);
					image.create(WIDTH, HEIGHT, sf::Color::Black);
					minRe = -2.0;
					maxRe = 1.0;
					minIm = -1.2;
					maxIm = 1.2;
					image = createMandelbrot(maxIterations, colours, image, minRe, maxRe, minIm, maxIm);
				}
			}

			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					std::cout << "Zooming in... " << std::endl;;
					textureSet = false;
					window.clear(sf::Color::Black);
					image.create(WIDTH, HEIGHT, sf::Color::Black);
					image = createMandelbrot(maxIterations, colours, image, minRe /= 2, maxRe /= 2, minIm /= 2, maxIm /= 2);
				}
			}
		}

		if (!textureSet)
		{
			texture.loadFromImage(image);
			mandelbrot.setTexture(texture);
			textureSet = true;
		}

		if ((sf::Mouse::getPosition(window).x < WIDTH + 1 && sf::Mouse::getPosition(window).x > -1) &&
			sf::Mouse::getPosition(window).y < HEIGHT + 1 && sf::Mouse::getPosition(window).y > -1)
		{
			sf::Vector2i mousePos = sf::Mouse::getPosition(window);
			mouseXText.setString("x: " + std::to_string(minRe + mousePos.x * (maxRe - minRe) / WIDTH));
			mouseYText.setString("y: " + std::to_string(maxIm - mousePos.y * (maxIm - minIm) / HEIGHT));
		}

		//Clear window and draw
		window.clear(sf::Color::Black);
		window.draw(mandelbrot);
		window.draw(mouseXText);
		window.draw(mouseYText);
		window.display();
	}
	return 0;
}

//function to draw image
sf::Image createMandelbrot(unsigned int maxIterations, sf::Color colours[], sf::Image image, double minRe, double maxRe, double minIm, double maxIm)
{
	std::cout << "Drawing image... " << std::endl;;
	auto reFactor = (maxRe - minRe) / (WIDTH);
	auto imFactor = (maxIm - minIm) / (HEIGHT);

	for (auto y = 0; y < HEIGHT; y++)
	{
		auto cIm = maxIm - y * imFactor;
	
		for (auto x = 0; x < WIDTH; x++)
		{
			auto cRe = minRe + x * reFactor;

			auto zRe = cRe, zIm = cIm;
			auto isInside = true;
			auto iteration = 0;
			
			for (auto i = 0; i < maxIterations; i++)
			{
				auto zRe2 = zRe*zRe, zIm2 = zIm*zIm;
				
				if (zRe2 + zIm2 > 4)
				{
					isInside = false;
					break;
				}
				
				zIm = 2 * zRe*zIm + cIm;
				zRe = zRe2 - zIm2 + cRe;
				
				iteration++;
			}
			if (isInside) 	
				image.setPixel(x, y, sf::Color::Black);
			else
				image.setPixel(x, y, colours[iteration % 16]);
		}
	}

	std::cout << "Image drawn." << std::endl << std::endl;
	return image;
}