#include "Core/Camera.h"
// 注意初始视角改变的话摄像机里面有个地方也需要改
camera cam(vec3(2.8, 2.8, -8.8), vec3(0.0, 0.0, 1), vec3(0, 1, 0), 40, 1.0f, 0.0, 1.0, 0.0, 1.0);

void setCameraDirection(float xBias, float yBias, camera &cam)
{
	cam.xAngle += xBias / 5.0f;
	if (cam.xAngle > 180.0f)
		cam.xAngle -= 360.0f;
	if (cam.xAngle < -180.0f)
		cam.xAngle += 360.0f;
	cam.yAngle += yBias / 5.0f;
	if (cam.yAngle > 85.0f)
		cam.yAngle = 85.0f;
	if (cam.yAngle < -85.0f)
		cam.yAngle = -85.0f;
}

void setCameraPositionFront(float Bias, camera &cam)
{
	cam.origin += Bias * cam.w;
}
void setCameraPositionBack(float Bias, camera &cam)
{
	cam.origin += Bias * cam.w;
}
void setCameraPositionLeft(float Bias, camera &cam)
{
	cam.origin += Bias * cam.u;
}
void setCameraPositionRight(float Bias, camera &cam)
{
	cam.origin += Bias * cam.u;
}
