#pragma once
#include <glm\core\type.hpp>
using namespace glm;

class Camera
{
	enum CameraMode
	{
		FOCUSED,
		FREE
	};
public:
	Camera(void);
	~Camera(void);

	vec3 position;
	vec3 direction;
	vec3 target;
	CameraMode mode;
private:
	vec3 velocity;
};

