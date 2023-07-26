#include "Player.h"

glm::vec2 Player::complexProduct(glm::vec2 a, glm::vec2 b) {
	return glm::vec2(
		a.x * b.x - a.y * b.y,
		a.x * b.y + a.y * b.x
	);
}

glm::vec2 Player::rotateVector(glm::vec2 vec, float angle) {
    glm::vec2 rotated(
        glm::cos(angle) * vec.x - glm::sin(angle) * vec.y,
        glm::sin(angle) * vec.x + glm::cos(angle) * vec.y
    );

    return rotated;
}

int Player::signe(float nb) {
	if (nb > 0) {
		return 1;
	}
	else if (nb < 0) {
		return -1;
	}

	return 0;
}

Player::Player(Map* map) {
	this->map = map;
}

void Player::setRunning(bool running) {
	this->running = running;
}

bool Player::isRunning() const {
	return running;
}

void Player::setPosition(glm::vec2 position) {
	this->position = position;
}

const glm::vec2 * Player::getPosition() const {
	return &position;
}

void Player::move(glm::vec2 moveDirection, float deltaTime) {
	if (moveDirection.x == 0 && moveDirection.y == 0) return;
	moveDirection = glm::normalize(moveDirection);
	moveDirection = complexProduct(moveDirection, direction);
	if (running) {
		position = position + moveDirection * runSpeed * deltaTime;
	} else {
		position = position + moveDirection * walkSpeed * deltaTime;
	}
}

void Player::pan(float amout) {
    plane = rotateVector(plane, amout * panSpeed);
    direction = rotateVector(direction, amout * panSpeed);
}

RayHit Player::rayCast(float rayAngle) {
    //calculate ray position and direction
    glm::vec2 rayDirection = direction + rayAngle * plane;
    //which box of the map we're in
    auto mapPos = glm::ivec2(int(position.x), int(position.y));

    //length of ray from current position to next x or y-side
    glm::vec2 sideDist(0, 0);

    //length of ray from one x or y-side to next x or y-side
    //these are derived as:
    //deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX))
    //deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY))
    //which can be simplified to abs(|rayDir| / rayDirX) and abs(|rayDir| / rayDirY)
    //where |rayDir| is the length of the vector (rayDirX, rayDirY). Its length,
    //unlike (dirX, dirY) is not 1, however this does not matter, only the
    //ratio between deltaDistX and deltaDistY matters, due to the way the DDA
    //stepping further below works. So the values can be computed as below.
    // Division through zero is prevented, even though technically that's not
    // needed in C++ with IEEE 754 floating point values.
    glm::vec2 deltaDist(0, 0);
    deltaDist.x = abs(1 / rayDirection.x);
    deltaDist.y = abs(1 / rayDirection.y);

    double perpWallDist;

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
        if (!map->inMap(mapPos)) hit = 1;
        else if (map->getTileAt(mapPos)->type != Tile::VOID) hit = 1;
    }

    //Calculate distance projected on camera direction. This is the shortest distance from the point where the wall is
    //hit to the camera plane. Euclidean to center camera point would give fisheye effect!
    //This can be computed as (mapX - posX + (1 - stepX) / 2) / rayDirX for side == 0, or same formula with Y
    //for size == 1, but can be simplified to the code below thanks to how sideDist and deltaDist are computed:
    //because they were left scaled to |rayDir|. sideDist is the entire length of the ray above after the multiple
    //steps, but we subtract deltaDist once because one step more into the wall was taken above.
    
    RayHit outHit;

    if (side == 0) outHit.distance = (sideDist.x - deltaDist.x);
    else outHit.distance = (sideDist.y - deltaDist.y);

    return outHit;
}

