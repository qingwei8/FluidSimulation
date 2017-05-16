#include <fstream>
#include <glm/glm.hpp>
#include <iostream>

void recordRange(const glm::vec3& vTargetPos, glm::vec3& voMax, glm::vec3& voMin)
{
	voMin.x = (voMin.x < vTargetPos.x) ? voMin.x : vTargetPos.x;
	voMax.x = (voMax.x > vTargetPos.x) ? voMax.x : vTargetPos.x;

	voMin.y = (voMin.y < vTargetPos.y) ? voMin.y : vTargetPos.y;
	voMax.y = (voMax.y > vTargetPos.y) ? voMax.y : vTargetPos.y;

	voMin.z = (voMin.z < vTargetPos.z) ? voMin.z : vTargetPos.z;
	voMax.z = (voMax.z > vTargetPos.z) ? voMax.z : vTargetPos.z;
}

int main()
{
	std::ifstream Fin("Box331_0.5.txt");

	glm::vec3 Max = glm::vec3(FLT_MIN);
	glm::vec3 Min = glm::vec3(FLT_MAX);

	while (!Fin.eof())
	{
		glm::vec3 Pos;
		Fin >> Pos.x >> Pos.y >> Pos.z;
		recordRange(Pos, Max, Min);
	}

	std::cout << "MAX_Range: " << Max.x << " " << Max.y << " " << Max.z << std::endl;
	std::cout << "MIN_Range: " << Min.x << " " << Min.y << " " << Min.z << std::endl;

	return 0;
}