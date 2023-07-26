#include "Map.h"

Map::Map(glm::ivec2 size) {
	this->size = size;
	this->tiles = new Tile[size.x * size.y];
}

Map::~Map() {
	delete[] tiles;
}

const Tile* Map::getTileAt(glm::ivec2 pos) const {
	return &tiles[pos.x * size.y + pos.y];
}

void Map::setTileAt(glm::ivec2 pos, Tile tile) {
	this->tiles[pos.x * size.y + pos.y] = tile;
}

bool Map::inMap(glm::ivec2 pos) {
	if (pos.x < 0 || pos.y < 0) {
		return false;
	}
	if (pos.x >= size.x || pos.y >= size.y) {
		return false;
	}

	return true;
}

std::string Map::getMapString() const {
	std::string mapstring = "^ Y    \n|      \n|     X\nO----->\n";
	mapstring += "\n";
	for (int i = size.y - 1; i >= 0; i--) {
		for (int j = 0; j < size.x; j++) {
			if (getTileAt(glm::vec2(j, i))->type == Tile::VOID) {
				mapstring += "o";
			}
			else {
				mapstring += "X";
			}
		}
		mapstring += "\n";
	}

	return mapstring;
}