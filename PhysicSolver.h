#pragma once

#include <vector>
#include <algorithm>
#include <functional>
#include <SFML/Graphics.hpp>

#include "PhysicBody2d.h"

#include <omp.h>

class ChunkGrid {
public:
	ChunkGrid(int cS, int wW, int wH);

	void assignGrid(std::vector<PhysicBody2d>& obj);
	void updateChunkSize(const PhysicBody2d& obj);

	///make sure to reassign grid "assignGrid()", before using
	void update_collision();
	void update_collision_mt();

	void solve_collision(std::vector<PhysicBody2d*>& central, const std::vector<PhysicBody2d*>& neigh);

	int count();

protected:
	std::vector<std::vector<std::vector<PhysicBody2d*>>> grid;
	int cellSize;
	int grid_width;
	int grid_height;
	int window_width;
	int window_height;

	std::vector<std::vector<PhysicBody2d*>> emptyCol;
};

class PhysicSolver{
public:
	PhysicSolver(ChunkGrid g) : grid { g } {}

	PhysicSolver& add(const PhysicBody2d& obj);

	void update(const float dtime, const int sub_step = 1);
	//void update(const float dtime, const std::function<Vec2(PhysicBody2d&, std::vector<PhysicBody2d>&)> &acceleration_function, const int sub_step = 1);
	//void update(const float dtime, const Vec2 &acc, const std::function<Vec2(PhysicBody2d&)> &constratint_fun, const int sub_step);
	//void update(const float dtime, const std::function<Vec2(PhysicBody2d&, std::vector<PhysicBody2d>&)> &acceleration_function, const std::function<Vec2(PhysicBody2d&)> &constratint_fun, const int sub_step = 1);

	void update_position(const float dtime);
	void update_acceleration();
	//void update_acceleration(Vec2 accelerationValue);
	//void update_acceleration(const std::function<Vec2(PhysicBody2d&, std::vector<PhysicBody2d>&)> &acceleration_function);
	void update_constraints();
	//void update_constraints(const std::function<Vec2(PhysicBody2d&)> &constratint_fun);
	///much slower for many objects, better use grid.assign() and grid.update_collision()
	void update_collision();

	void set_acceleration(const Vec2 accVal) {
		acceleration_type = VALUE;
		accelerationValue = accVal;
	}
	void set_acceleration(const std::function<Vec2(PhysicBody2d&, std::vector<PhysicBody2d>&)> accFun) {
		acceleration_type = FUNC;
		acceleration_function = accFun;
	}
	void set_acceleration() {
		acceleration_type = NONE;
	}

	void set_constraints_def() {
		constraint_type = DEFAULT;
	}
	void set_constraints(const std::function<Vec2(PhysicBody2d&)> conFun) {
		constraint_type = FUNC;
		constratint_fun = conFun;
	}
	void set_constraints() {
		constraint_type = NONE;
	}

	std::pair<bool, PhysicBody2d> pop_from_position(const Vec2 cord);

	int getObjectAmount() const { return objects.size(); }
	ChunkGrid& getChunkGrid() { return grid; }

protected:
	std::vector<PhysicBody2d> objects{};
	ChunkGrid grid;
	friend class PhysicDrawer;
	//int acceleration_type=0; //-1 using function, 0 none, 1 value

	enum { FUNC, NONE, VALUE, DEFAULT } acceleration_type{ NONE }, constraint_type{ DEFAULT };
	Vec2 accelerationValue;
	std::function<Vec2(PhysicBody2d&, std::vector<PhysicBody2d>&)> acceleration_function;
	std::function<Vec2(PhysicBody2d&)> constratint_fun;
};

class PhysicDrawer : public sf::Drawable {
public:
	PhysicDrawer(const PhysicSolver& ps) : physicSolver{ ps } {}
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
protected:
	const PhysicSolver& physicSolver;
};