#include "Player.h"


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
	moveDirection = Utility::complexProduct(moveDirection, direction);

	glm::vec2 mouvment = moveDirection * deltaTime;
	if (running) mouvment *= runSpeed;
	else mouvment *= walkSpeed;

	//Collide
	bool collide[4] = { false };

	for(int i = 0; i < colisionRays; i ++)
	{
		glm::vec2 rayDir = Utility::rotateVector(direction, (2 * Utility::PI * i) / colisionRays);

		RayHit hit = map->rayCast(position, rayDir);
		if (hit.distance < size) collide[hit.side] = true;
	}

	if (
		(mouvment.x > 0 && collide[XN]) ||
		(mouvment.x < 0 && collide[XP])
		)
		mouvment.x = 0;
	if (
		(mouvment.y > 0 && collide[YN]) ||
		(mouvment.y < 0 && collide[YP])
		)
		mouvment.y = 0;

	//Detect portals
	glm::vec2 normalizedMouvment = glm::normalize(mouvment);
	RayHit portalHit = map->rayCast(position, normalizedMouvment, 0, true);
	float mouveDist = glm::length(mouvment);
	if(portalHit.distance < mouveDist && portalHit.portalHit != 0 )
	{
		Portal portal = portalHit.portalHit == 1 ? map->getPortal1() : map->getPortal2();
		Portal otherPortal = portalHit.portalHit == 1 ? map->getPortal2() : map->getPortal1();
		float angle = portalHit.portalHit == 1 ? map->getPortal2AngleFrom1() : -map->getPortal2AngleFrom1();

		glm::vec2 portalOffset = position + (normalizedMouvment * portalHit.distance) - portal.pos;
		glm::vec2 exitVector = normalizedMouvment * (mouveDist - portalHit.distance);

		glm::vec2 rotatedPortalOffset = -Utility::rotateVector(portalOffset, angle);
		glm::vec2 rotatedExitVector = -Utility::rotateVector(exitVector, angle);

		plane = -Utility::rotateVector(plane, angle);
		direction = -Utility::rotateVector(direction, angle);
		position = otherPortal.pos + rotatedPortalOffset + rotatedExitVector;
	}
	else
	{
		position += mouvment;
	}
}

void Player::pan(float amount) {
    plane = Utility::rotateVector(plane, -amount * panSpeed);
    direction = Utility::rotateVector(direction, -amount * panSpeed);
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

		// Horizontal distance from the camera to the ceiling for the current row.
		float rowDistance = 0.5f / fracY;

		// calculate the real world step vector we have to add for each x (parallel to camera plane)
		glm::vec2 floorStep = glm::vec2(rowDistance * (rayDir1.x - rayDir0.x), rowDistance * (rayDir1.y - rayDir0.y));

		// real world coordinates of the leftmost column.
		glm::vec2 leftmost = glm::vec2(ceilPos.x + rowDistance * rayDir0.x, ceilPos.y + rowDistance * rayDir0.y);

		scanCoords[y] = leftmost.x;
		scanCoords[y + scanLines] = leftmost.y;
		scanCoords[y + 2 * scanLines] = floorStep.x;
		scanCoords[y + 3 * scanLines] = floorStep.y;
	}	
}

void Player::launchPortal1() const
{
	RayHit hit = map->rayCast(position, direction);
	Portal portal;

	portal.pos = hit.position;
	portal.side = hit.side;

	map->setPortal1(portal);
}

void Player::launchPortal2() const
{
	RayHit hit = map->rayCast(position, direction);
	Portal portal;

	portal.pos = hit.position;
	portal.side = hit.side;

	map->setPortal2(portal);
}


