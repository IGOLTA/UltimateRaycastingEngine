#pragma once

#include <glm.hpp>
#include <string>

#include "Tile.h"

struct RayHit {
	const Tile* tile;
	float distance;
	float hitOffset;
};

class Map
{
private:
	glm::ivec2 size;
	Tile* tiles;
public:
	Map(glm::ivec2 size);
	~Map();

	const Tile* getTileAt(glm::ivec2 pos) const;
	void setTileAt(glm::ivec2 pos, Tile tile);
	
	bool inMap(glm::ivec2 pos) const;

	RayHit rayCast(glm::vec2 origin, glm::vec2 direction) const;

	std::string getMapString() const;
};

