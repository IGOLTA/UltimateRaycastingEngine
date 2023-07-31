#pragma once

#include <glm.hpp>

namespace Utility
{
	const float PI = 3.141593f;

	glm::vec2 complexProduct(glm::vec2 a, glm::vec2 b);

	glm::vec2 rotateVector(glm::vec2 vec, float angle);

	int signe(float nb);
}