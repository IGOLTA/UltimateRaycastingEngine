#include "Player.h"
#include <iostream>

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

void Player::pan(float amount) {
    plane = rotateVector(plane, amount * panSpeed);
    direction = rotateVector(direction, amount * panSpeed);
}

RayHit Player::rayCast(float rayAngle) {
    //calculate ray position and direction
    glm::vec2 rayDirection = direction + rayAngle * plane;

    return map->rayCast(position, rayDirection);
}

void Player::ceilingScan(int scanLines, float speedFactor, float* scanCoords) const {
	auto ceilPos = -position / speedFactor;
	for (int y = 0; y < scanLines; y++)
	{
		float fracY = (float)y / (scanLines - 1);
		// rayDir for leftmost ray (x = 0) and rightmost ray (x = w)
		glm::vec2 rayDir0 = -direction - plane;
		glm::vec2 rayDir1 = -direction + plane;

		// Horizontal distance from the camera to the floor for the current row.
		float rowDistance = 0.5f / fracY;

		// calculate the real world step vector we have to add for each x (parallel to camera plane)
		// adding step by step avoids multiplications with a weight in the inner loop
		glm::vec2 floorStep = glm::vec2(rowDistance * (rayDir1.x - rayDir0.x), rowDistance * (rayDir1.y - rayDir0.y));

		// real world coordinates of the leftmost column. This will be updated as we step to the right.
		glm::vec2 leftmost = glm::vec2(ceilPos.x + rowDistance * rayDir0.x, ceilPos.y + rowDistance * rayDir0.y);

		scanCoords[y] = leftmost.x;
		scanCoords[y + scanLines] = leftmost.y;
		scanCoords[y + 2 * scanLines] = floorStep.x;
		scanCoords[y + 3 * scanLines] = floorStep.y;
	}	
}