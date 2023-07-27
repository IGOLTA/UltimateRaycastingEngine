#pragma once

#include <glm.hpp>
#include <string>

struct RayHit {
	unsigned int tileId;
	float distance;
	float hitOffset;
};

class Map
{
private:
	glm::ivec2 size;
	unsigned int* tiles;
public:
	Map(glm::ivec2 size);
	~Map();

	unsigned int getTileIdAt(glm::ivec2 pos) const;
	void setTileIdAt(glm::ivec2 pos, unsigned int tile);
	
	bool inMap(glm::ivec2 pos) const;

	RayHit rayCast(glm::vec2 origin, glm::vec2 direction) const;

	std::string getMapString() const;
};

