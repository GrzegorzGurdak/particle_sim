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

int main()
{
	sf::Clock clock;
	sf::Clock fclock;
	sf::RenderWindow window(sf::VideoMode(770, 730), "Orbiting", sf::Style::Titlebar | sf::Style::Close);
	sf::Event event;
	window.setFramerateLimit(60);

	sf::Font font; font.loadFromFile("arial.ttf");
	
	sf::Text objectAmountText("", font);
	sf::Text simTime("", font); simTime.setPosition(0, 30);
	int simTimeValue{}, frame_count{};
	sf::Text fps_text("", font); fps_text.setPosition(0, 60);
	unsigned int FPS = 0, frame_counter = 0;
	
	PhysicSolver sandbox(ChunkGrid(10, window.getSize().x, window.getSize().y));
	PhysicDrawer sandbox_draw(sandbox);
	sandbox.add(PhysicBody2d(Vec2(150, 180),5));
	sandbox.add(PhysicBody2d(Vec2(450, 180),5));

	bool mousePressed = false;
	Vec2 mousePosition;

	std::pair<bool, PhysicBody2d> dragBuffer;

	srand(time(NULL));
	sf::err().rdbuf(NULL);

	sandbox.set_acceleration(Vec2(0, 100));
	//sandbox.set_constraints(PhysicExamples::Constrains::boxRestrain({ 30,30 }, { 730,690 }));
	sandbox.set_constraints([](PhysicBody2d& i) {
		Vec2 diff = i.getPos() - Vec2(350, 350);
		float diffLen = diff.length();
		if (diffLen + i.getRadius() > 300) {
			return i.getPos() - diff / diffLen * (diffLen + i.getRadius() - 300);
		}
		return i.getPos();
	});

	while (window.isOpen()){
		window.clear();
		if (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
				window.close();

			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
				mousePressed = true;
				mousePosition.set(event.mouseButton.x, event.mouseButton.y);
			}else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
				mousePressed = false;
				mousePosition.set(event.mouseButton.x, event.mouseButton.y);
			}else if(event.type == sf::Event::MouseMoved) 
				mousePosition.set(event.mouseMove.x, event.mouseMove.y);

			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
				dragBuffer = sandbox.pop_from_position({ event.mouseButton.x,event.mouseButton.y });
			}
			else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right) {
				if (dragBuffer.first) {
					dragBuffer.second.move(mousePosition);
					sandbox.add(dragBuffer.second);
					dragBuffer.first = false;
				}
			}
		}

		if (mousePressed && clock.getElapsedTime().asMilliseconds() > 2) {
			for (int i = 0; i < 5; i++) 
				sandbox.add(PhysicBody2d(mousePosition + Vec2::random_rad(40), (double)rand() / RAND_MAX * 10 + 2));
			clock.restart();
			objectAmountText.setString(std::to_string(sandbox.getObjectAmount()));
		}

		auto start = std::chrono::steady_clock::now();
		sandbox.update(1 / 60., 10);
		//sandbox.update(1 / 60., Vec2(0, 100),PhysicExamples::Constrains::circleRestrain(Vec2(350,350),300), 10);
		//sandbox.update(1 / 60., Vec2(0, 100), PhysicExamples::Constrains::boxRestrain({10,10}, {750,710}), 10);
		/*sandbox.update(1 / 60., Vec2(0, 100), [](PhysicBody2d& i) {
				Vec2 diff = i.getPos() - Vec2(350,350);
				float diffLen = diff.length();
				if (diffLen + i.getRadius() > 300) {
					i.setPos(i.getPos() - diff / diffLen * (diffLen + i.getRadius() - 300));

				}
			}, 10);*/
		//sandbox.update(1 / 60., [](PhysicBody2d& i, std::vector<PhysicBody2d>&) {return Vec2(0, 100); }, 10);
		auto end = std::chrono::steady_clock::now();

		simTimeValue += (std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());

		if (fclock.getElapsedTime().asSeconds() >= 1.0f)
		{
			if (frame_counter != 0) {
				simTime.setString(std::to_string(simTimeValue / frame_counter) + "us");
				simTimeValue = 0;
			}

			FPS = (unsigned int)((float)frame_counter / fclock.getElapsedTime().asSeconds());
			fps_text.setString("fps: " + std::to_string(FPS));
			fclock.restart();
			frame_counter = 0;
		}
		frame_counter++;

		window.draw(sandbox_draw);
		if (dragBuffer.first) {
			dragBuffer.second.move(mousePosition);
			window.draw(dragBuffer.second.getFigure());
		}

		window.draw(objectAmountText);
		window.draw(simTime);
		window.draw(fps_text);

		window.display();
	}
}