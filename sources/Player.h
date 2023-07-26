#pragma once

#include <glm.hpp>

#include "map/Map.h"

struct RayHit {
	const Tile* tile;
	float distance;
};

class Player
{
private:
	const float walkSpeed = 3;
	const float panSpeed = 0.001;
	const float runSpeed = 4;
	const float fov = 90;

	glm::vec2 position = glm::vec2(0, 0);
	glm::vec2 direction = glm::vec2(1, 0);
	glm::vec2 plane = glm::vec2(0, 1);

	bool running = 0;
	
	Map* map;

	static glm::vec2 complexProduct(glm::vec2 a, glm::vec2 b);
	static glm::vec2 rotateVector(glm::vec2 vec, float angle);
	static int signe(float nb);

public:
	Player(Map* map);
	
	void setRunning(bool running);
	bool isRunning() const;
	
	void setPosition(glm::vec2 poisiton);
	const glm::vec2* getPosition() const;
	
	void pan(float amount);
	void move(glm::vec2 moveDirection, float deltaTime);

	RayHit rayCast(float rayAngle); //Pas vraiment un angle, juste une valeur pour multiplier avec plane
};

