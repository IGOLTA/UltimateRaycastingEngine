#include "Utility.h"

namespace Utility
{
	glm::vec2 complexProduct(glm::vec2 a, glm::vec2 b) {
		return glm::vec2(
			a.x * b.x - a.y * b.y,
			a.x * b.y + a.y * b.x
		);
	}

	glm::vec2 rotateVector(glm::vec2 vec, float angle) {

		glm::vec2 rotated(
			glm::cos(angle) * vec.x - glm::sin(angle) * vec.y,
			glm::sin(angle) * vec.x + glm::cos(angle) * vec.y
		);

		return rotated;
	}

	int signe(float nb) {
		if (nb > 0) {
			return 1;
		}
		else if (nb < 0) {
			return -1;
		}

		return 0;
	}
}
