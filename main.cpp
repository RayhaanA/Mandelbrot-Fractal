#include "SFML\Window.hpp"
#include "SFML\Graphics.hpp"
#include <vector>
#include <iostream>

//Global window dimensions
const unsigned int HEIGHT = 600;
const unsigned int WIDTH = 800;

//Image structure
struct Image {
	sf::Image image_;
	double minRe_, maxRe_, minIm_, maxIm_;
	Image(sf::Image image, double minRe, double maxRe, double minIm, double maxIm)
	{
		image_ = image;
		minRe_ = minRe;
		maxRe_ = maxRe;
		minIm_ = minIm;
		maxIm_ = maxIm;
	}
};

//Function to create mandelbrot fractal
sf::Image createMandelbrot(unsigned int maxIterations, sf::Image image, double minRe, double maxRe, double minIm, double maxIm);

//Function to calculate new bounds
void changeBounds(sf::Vector2f mousePos, double & minRe, double & maxRe, double & minIm, double & maxIm, int zoom);

//Colouring function
sf::Color smoothRGB(int iter, int maxIter);

int main()
{
	//Container for holding previous fractal zoom images for quicker unzooming
	std::vector<Image> images;

	//Initialize window
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Mandelbrot Fractals", sf::Style::Close);

	//Texture and sprite for texturing and drawing to window
	sf::Sprite mandelbrot;
	sf::Texture texture;

	//Text for displaying info on window
	
	// Load font
	sf::Font font;
	font.loadFromFile("./res/Forque.ttf");
	
	// Create text fields for x and y mouse pos
	sf::Text mouseXText("", font);
	mouseXText.setCharacterSize(16);
	mouseXText.setColor(sf::Color::White);
	mouseXText.setPosition(0, 0);
	sf::Text mouseYText("", font);
	mouseYText.setCharacterSize(16);
	mouseYText.setColor(sf::Color::White);
	mouseYText.setPosition(0, 16);

	//Sets max number of iterations, varies based on the scale value
	auto maxIterations = 500;

	//Virtual coordinate extremities
	auto minRe = -2.0;
	auto maxRe = 1.0;
	auto maxIm = HEIGHT * (maxRe - minRe) / WIDTH  / 2;
	auto minIm = -maxIm;

	//Zoom factor
	auto zoom = 2;
	
	//Scaled mouse position
	sf::Vector2f relMousePos;

	//Create mandelbrot image
	sf::Image image;
	image.create(WIDTH, HEIGHT, sf::Color::Black);
	image = createMandelbrot(maxIterations, image, minRe, maxRe, minIm, maxIm);

	images.push_back(Image(image, minRe, maxRe, minIm, maxIm));

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
					std::cout << "Resetting image... " << std::endl;
					textureSet = false;
					window.clear(sf::Color::Black);
					minRe = -2.0, maxRe = 1.0, minIm = -1.2, maxIm = 1.2;
					for (auto i = images.size() - 1; i > 0; i--)
						images.pop_back();
					std::cout << "Images reset\n\n";
				}
			}

			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					std::cout << "Zooming in... " << std::endl;
					textureSet = false;
					changeBounds(relMousePos, minRe, maxRe, minIm, maxIm, zoom);
					image = createMandelbrot(maxIterations, image, minRe, maxRe, minIm, maxIm);
					images.push_back(Image(image, minRe, maxRe, minIm, maxIm));
					std::cout << relMousePos.x << "  " << relMousePos.y << "\n\n";
				}

				if (event.mouseButton.button == sf::Mouse::Right && images.size() > 1)
				{
					std::cout << "Zooming out.\n\n";
					textureSet = false;
					images.pop_back();
					minRe = images.back().minRe_;
					maxRe = images.back().maxRe_;
					minIm = images.back().minIm_;
					maxIm = images.back().maxIm_;
				}

				std::cout << images.size() << " images stacked.\n\n";
			}
		}

		if (!textureSet)
		{
			texture.loadFromImage((images.back()).image_);
			mandelbrot.setTexture(texture);
			textureSet = true;
		}

		if ((sf::Mouse::getPosition(window).x < WIDTH + 1 && sf::Mouse::getPosition(window).x > -1) &&
			sf::Mouse::getPosition(window).y < HEIGHT + 1 && sf::Mouse::getPosition(window).y > -1)
		{
			sf::Vector2i mousePos = sf::Mouse::getPosition(window);
			relMousePos.x = images.back().minRe_ + mousePos.x * (images.back().maxRe_ - images.back().minRe_) / WIDTH;
			relMousePos.y = images.back().maxIm_ - mousePos.y * (images.back().maxIm_ - images.back().minIm_) / HEIGHT;
			mouseXText.setString("x: " + std::to_string(relMousePos.x));
			mouseYText.setString("y: " + std::to_string(relMousePos.y));
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

sf::Image createMandelbrot(unsigned int maxIterations, sf::Image image, double minRe, double maxRe, double minIm, double maxIm)
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

			//Colouring variable
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
				image.setPixel(x, y, smoothRGB(iteration, maxIterations));
		}
	}

	std::cout << "Image drawn.\n\n";
	return image;
}

void changeBounds(sf::Vector2f mousePos, double & minRe, double & maxRe, double & minIm, double & maxIm, int zoom)
{
	//Current plane dimensions
	auto width = maxRe - minRe;
	auto height = maxIm - minIm;

	//New plane dimension (makes it smaller by a factor of zoom)
	width /= zoom;
	height /= zoom;

	//Set new bounds on complex plane
	minRe = mousePos.x - width / 2;
	maxRe = mousePos.x + width / 2;
	minIm = mousePos.y - height / 2;
	maxIm = mousePos.y + height / 2;
}

sf::Color smoothRGB(int iter, int maxIter)
{
	double x = (double)iter / (double)maxIter;

 	// Polynomials for calculating rgb
	// Polynomials from https://solarianprogrammer.com/2013/02/28/mandelbrot-set-cpp-11/
	int r = (int)(9 * (1 - x) * x * x * x * 255);
	int g = (int)(15 * (1 - x) * (1 - x) * x * x * 255);
	int b = (int)(8.5 * (1 - x) * (1 - x)  *(1 - x) * x * 255);
	
	return sf::Color(r, g, b);
}