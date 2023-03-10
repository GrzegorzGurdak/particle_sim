// particle_sim.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <string>
#include <iostream>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <chrono>

#include "MVec.h"
#include "PhysicBody2d.h"
#include "PhysicSolver.h"
#include "PhysicExamples.h"
#include "ColorConv.h"
#include "ImageGenerator.h"
#include "GUI_elements.h"

int main()
{
	sf::Clock clock;
	sf::RenderWindow window(sf::VideoMode(770, 730), "Orbiting", sf::Style::Titlebar | sf::Style::Close); //730
	sf::Event event;
	window.setFramerateLimit(60);

	sf::Font font; font.loadFromFile("arial.ttf");
	
	StatElement statElement(font);
	
	PhysicSolver sandbox(ChunkGrid(15, window.getSize().x, window.getSize().y)); //30,33,36
	//PhysicSolver sandbox = *PhysicExamples::Sandbox::cloth(window.getSize(), {100,100},10,10);
	PhysicDrawer sandbox_draw(sandbox);
	//sandbox.add(PhysicBody2d(Vec2(150, 180),5)).add(PhysicBody2d(Vec2(450, 180),5));

	bool mousePressed = false;
	Vec2 mousePosition;
	
	bool paused = false;

	std::pair<bool, PhysicBody2d*> dragBuffer;

	//srand(time(NULL));
	srand(5);
	sf::err().rdbuf(NULL);

	sandbox.set_acceleration(Vec2(0, 100));
	//sandbox.set_acceleration(PhysicExamples::Acceleration::centreAcceleration({ 350,350 }, 10000));
	/*sandbox.set_acceleration([](const PhysicBody2d* obj, const std::vector<PhysicBody2d*>&) {
		return (obj->getOldPos() - obj->getPos()) * 600 * 0.9;
	});*/
	/*sandbox.set_acceleration([](const PhysicBody2d* obj, const std::vector<PhysicBody2d*>&) {
		float r = obj->getRadius();
		return Vec2(0, 1) * r * r * r;
		});*/
	//sandbox.set_constraints_def();
	//sandbox.set_constraints(PhysicExamples::Constrains::boxRestrain({ 30,30 }, { 730,690 }));
	//sandbox.getChunkGrid().set_collision(PhysicExamples::Collisions::squishy_collision);
	//sandbox.set_constraints(PhysicExamples::Constrains::defaultConstrain);

	ColorMap colormap("pic2.jpg", "result", { 30,30 }, { 730,690 });

	bool dragEnable = false;
	bool shiftPressed = false;
	bool kinematicStateBefore;

	while (window.isOpen()) {
		window.clear();
		if (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
				window.close();

			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
				ImageGenerator::exportResult(sandbox, "result");
			}
			else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
				paused = !paused;
			}
			else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::LShift) {
				shiftPressed = true;
			}
			else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::LShift) {
				shiftPressed = false;
			}

			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
				mousePressed = true;
				mousePosition.set(event.mouseButton.x, event.mouseButton.y);
			}
			else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
				mousePressed = false;
				mousePosition.set(event.mouseButton.x, event.mouseButton.y);
			}
			else if (event.type == sf::Event::MouseMoved)
				mousePosition.set(event.mouseMove.x, event.mouseMove.y);

			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right && shiftPressed) {
				dragBuffer = sandbox.pop_from_position({ event.mouseButton.x,event.mouseButton.y });
				if (dragBuffer.second != &PhysicBody2d::nullPB) {
					dragEnable = true;
					kinematicStateBefore = dragBuffer.second->isKinematic;
					dragBuffer.second->isKinematic = false;
				}
			}
			else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
				dragBuffer = sandbox.pop_from_position({ event.mouseButton.x,event.mouseButton.y });
			}
			else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right) {
				if (dragBuffer.first && !dragEnable) {
					auto pr = sandbox.pop_from_position({ event.mouseButton.x,event.mouseButton.y });
					if (pr.first && pr.second != dragBuffer.second) sandbox.addLink(new PhysicLink2d(*dragBuffer.second, *pr.second, 20));
					dragBuffer.first = false;
				}
				if (dragBuffer.first) dragBuffer.second->isKinematic = kinematicStateBefore;
				dragEnable = false;
			}

			if (dragEnable && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
				if (dragBuffer.first)
					kinematicStateBefore = !kinematicStateBefore;
			}
		}

		/*if (sandbox.getObjectAmount() < 9e3)
			for (int i = 0; i < 5; i++)
				sandbox.add(
					new PhysicBody2d(Vec2{ 350, 300 } + Vec2::random_rad(40),
					(double)rand() / RAND_MAX * 3 + 2,colormap.getNext()));

		objectAmountText.setString(std::to_string(sandbox.getObjectAmount()));*/

		if (mousePressed && clock.getElapsedTime().asMilliseconds() > 2) {
			for (int i = 0; i < 5; i++)
				sandbox.add(
					new PhysicBody2d(
						mousePosition + Vec2::random_rad(40), (double)rand() / RAND_MAX * 3 + 2,
						ColorConv::hsvToRgb((sandbox.getObjectAmount() / 5 % 256) / 256., 1, 1)
					));
			clock.restart();
			statElement.objectAmountUpdate(sandbox.getObjectAmount());
		}
		if (!paused) {
			auto start = std::chrono::steady_clock::now();
			sandbox.update(1 / 60., 10);
			auto end = std::chrono::steady_clock::now();

			statElement.simTimeAdd((std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()));
		}
		
		statElement.update();

		window.draw(sandbox_draw);
		if (dragEnable) {
			dragBuffer.second->move(mousePosition);
		}
		window.draw(statElement);
		window.display();
	}
}