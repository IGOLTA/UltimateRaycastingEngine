#include "Map.h"

Map::Map(glm::ivec2 size) {
	this->size = size;
	this->tiles = new unsigned int[size.x * size.y];
    
    for (int i = size.y - 1; i >= 0; i--) {
        for (int j = 0; j < size.x; j++) {
            setTileIdAt(glm::vec2(j, i), 0);
        }
    }
}

Map::~Map() {
	delete[] tiles;
}

unsigned int Map::getTileIdAt(glm::ivec2 pos) const {
	return tiles[pos.x * size.y + pos.y];
}

void Map::setTileIdAt(glm::ivec2 pos, unsigned int tile) {
	this->tiles[pos.x * size.y + pos.y] = tile;
}

bool Map::inMap(glm::ivec2 pos) const {
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
			if (getTileIdAt(glm::vec2(j, i)) == 0) {
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

RayHit Map::rayCast(glm::vec2 position, glm::vec2 rayDirection) const {
    RayHit outHit;

    //which box of the map we're in
    auto mapPos = glm::ivec2(int(position.x), int(position.y));


    //length of ray from current position to next x or y-side
    glm::vec2 sideDist(0, 0);

    //length of ray from one x or y-side to next x or y-side
    glm::vec2 deltaDist(0, 0);
    deltaDist.x = abs(1 / rayDirection.x);
    deltaDist.y = abs(1 / rayDirection.y);

    //what direction to step in x or y-direction (either +1 or -1)
    glm::ivec2 step;

    int hit = 0; //was there a wall hit?
    int side; //was a NS or a EW wall hit?
    //calculate step and initial sideDist
    if (rayDirection.x < 0)
    {
        step.x = -1;
        sideDist.x = (position.x - mapPos.x) * deltaDist.x;
    }
    else
    {
        step.x = 1;
        sideDist.x = (mapPos.x + 1.0 - position.x) * deltaDist.x;
    }
    if (rayDirection.y < 0)
    {
        step.y = -1;
        sideDist.y = (position.y - mapPos.y) * deltaDist.y;
    }
    else
    {
        step.y = 1;
        sideDist.y = (mapPos.y + 1.0 - position.y) * deltaDist.y;
    }

    //perform DDA
    while (hit == 0)
    {
        //jump to next map square, either in x-direction, or in y-direction
        if (sideDist.x < sideDist.y)
        {
            sideDist.x += deltaDist.x;
            mapPos.x += step.x;
            side = 0;
        }
        else
        {
            sideDist.y += deltaDist.y;
            mapPos.y += step.y;
            side = 1;
        }

        //Check if ray has hit a wall
        if (!this->inMap(mapPos)) {
            hit = 1;
            outHit.tileId = 1;
        } else if (this->getTileIdAt(mapPos) != 0) {
            outHit.tileId = this->getTileIdAt(mapPos);
            hit = 1;
        }
    }

    //Distance
    if (side == 0) outHit.distance = (sideDist.x - deltaDist.x);
    else outHit.distance = (sideDist.y - deltaDist.y);

    //Where exactly the wall was hit
    if (side == 0)  outHit.hitOffset = (float)position.y + (float)outHit.distance * (float)rayDirection.y;
    else            outHit.hitOffset = (float)position.x + (float)outHit.distance * (float)rayDirection.x;


    return outHit;
}
