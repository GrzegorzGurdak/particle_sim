#pragma once

#include "PhysicBody2d.h"
#include "MVec.h"

namespace PhysicExamples {
	namespace Constrains {
		auto circleRestrain(Vec2 centre, float radius) {
			return[centre, radius](PhysicBody2d& i) {
				Vec2 diff = i.getPos() - centre;
				float diffLen = diff.length();
				if (diffLen + i.getRadius() > radius) {
					return i.getPos() - diff / diffLen * (diffLen + i.getRadius() - radius);

				}
			};
		}
		auto boxRestrain(Vec2 pos1, Vec2 pos2) {
			Vec2 minPos{ std::fminf(pos1.x,pos2.x),std::fminf(pos1.y,pos2.y) };
			Vec2 maxPos{ std::fmaxf(pos1.x,pos2.x),std::fmaxf(pos1.y,pos2.y) };
			pos1 = minPos;
			pos2 = maxPos;
			return[pos1, pos2](PhysicBody2d& i) {
				Vec2 npos = i.getPos();
				//Vec2 radVec{ i.getRadius(),i.getRadius() };
				if (npos.x < pos1.x + i.getRadius()) npos.x = pos1.x + i.getRadius();
				else if (npos.x > pos2.x - i.getRadius()) npos.x = pos2.x - i.getRadius();
				if (npos.y < pos1.y + i.getRadius()) npos.y = pos1.y + i.getRadius();
				else if (npos.y > pos2.y - i.getRadius()) npos.y = pos2.y - i.getRadius();
				return npos;
			};
		}
	}
}