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

