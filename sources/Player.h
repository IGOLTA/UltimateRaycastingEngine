#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <ext.hpp>
#include <glm.hpp>

#include "Utility.h"
#include "map/Map.h"

extern std::ostream* LogStream;
extern std::ostream* ErrorStream;

class Player
{
private:
	const float walkSpeed = 7;
	const float panSpeed = 0.002f;
	const float runSpeed = 14;
	const float size = 0.5f;
	const int colisionRays = 8;

	glm::vec2 position = glm::vec2(0, 0);
	glm::vec2 direction = glm::vec2(1, 0);
	glm::vec2 plane = glm::vec2(0, 0.8f);

	bool running = 0;
	
	Map* map;
public:
	Player(Map* map);
	
	void setRunning(bool running);
	bool isRunning() const;
	
	void setPosition(glm::vec2 poisiton);
	const glm::vec2* getPosition() const;
	
	void pan(float amount);
	void move(glm::vec2 moveDirection, float deltaTime);

	void launchPortal1() const;
	void launchPortal2() const;

	RayHit rayCast(float rayAngle); //Pas vraiment un angle, juste une valeur pour multiplier avec plane
	void ceilingScan(int scanLines, float speedFactor, float* scanCoords) const;
	/*
	Scan coords is a pointer to an output array shaped
	[
		leftmost line 1 x, ..., leftmost line n x, 
		leftmost line 1 y, ..., leftmost line n y,
		step line 1 x, ..., step line n x,
		step line 1 y, ..., step line n y,
	]
	*/
};

