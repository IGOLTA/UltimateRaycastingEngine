#pragma once

#include <vector>
#include <glm.hpp>
#include <string>

enum HitSide {
	XP = 0,
	XN = 1,
	YP = 2,
	YN = 3
};

struct RayHit {
	unsigned int tileId;
	float distance;

	glm::vec2 position;
	HitSide side;
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
public:
	glm::vec3 ambiantLight = glm::vec3(1, 1, 1);
	std::vector<Light> lights = std::vector<Light>();
	
	Map(glm::ivec2 size);
	~Map();

	unsigned int getTileIdAt(glm::ivec2 pos) const;
	void setTileIdAt(glm::ivec2 pos, unsigned int tile);
	
	bool inMap(glm::ivec2 pos) const;

	RayHit rayCast(glm::vec2 origin, glm::vec2 direction) const;
	
	std::string getMapString() const;
};

