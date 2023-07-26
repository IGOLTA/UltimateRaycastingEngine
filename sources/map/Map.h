#pragma once

#include <glm.hpp>
#include <string>

#include "Tile.h"

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
	
	bool inMap(glm::ivec2 pos);

	std::string getMapString() const;
};

