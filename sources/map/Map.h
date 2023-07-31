#pragma once

#include <vector>
#include <glm.hpp>
#include <iostream>
#include <string>

#include "../Utility.h"

extern std::ostream* LogStream;
extern std::ostream* ErrorStream;

const float PORTAL_RADIUS = 1;
const int RECURSION_LEVEL = 200;

enum Side {
	XP = 0,
	YP = 1,
	XN = 2,
	YN = 3
};

struct Portal
{
	glm::vec2 pos;
	Side side;

	bool operator==(const Portal& other) const
	{
		return other.pos == pos && other.side == side;
	}
};

struct RayHit {
	unsigned int tileId;
	float distance;

	glm::vec2 position;
	Side side;

	int portalHit = 0;
};

struct Light {
	glm::vec3 color;
	glm::vec3 position;
};

class Map
{
private:
	glm::ivec2 size;
	unsigned int* tiles;
	Portal portal1;
	Portal portal2;

	void portalCast(RayHit &potralHit, glm::vec2 originalDirection, int recursion, Portal hitPortal, Portal otherPortal) const;
public:
	glm::vec3 ambiantLight = glm::vec3(1, 1, 1);
	std::vector<Light> lights = std::vector<Light>();
	
	Map(glm::ivec2 size);
	~Map();

	unsigned int getTileIdAt(glm::ivec2 pos) const;
	void setTileIdAt(glm::ivec2 pos, unsigned int tile);
	Portal getPortal1() const;
	void setPortal1(Portal portal1);
	Portal getPortal2() const;
	void setPortal2(Portal portal2);

	float getPortal2AngleFrom1() const;

	bool inMap(glm::ivec2 pos) const;

	RayHit rayCast(glm::vec2 origin, glm::vec2 direction, int recursion = 0, bool hitPortals = false) const;

	std::string getMapString() const;
};

