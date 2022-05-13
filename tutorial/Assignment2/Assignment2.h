#pragma once
#include "igl/opengl/glfw/Viewer.h"
#include "./sceneParser.h"
enum AxisType {X, Y, Z};
class Assignment2 : public igl::opengl::glfw::Viewer
{
	int xRes, yRes;
	float xOldPos, yOldPos, xRel, yRel, xPos, yPos;
	bool isPressed;
	bool isRightPressed;
	int sourceIndx;
    std::vector<Eigen::Vector4f> transObjects;
    std::vector<Eigen::Vector4f> noneTransObjects;
	int tOffset = 15000;
public:
	float time;
	Assignment2(std::string file);
	SceneData sceneData;
	void Init();
	void Update(const Eigen::Matrix4f& Proj, const Eigen::Matrix4f& View, const Eigen::Matrix4f& Model, unsigned int  shaderIndx, unsigned int shapeIndx);
	void SetPosition(int x, int y);
	void ScaleAllShapes(float amt, int viewportIndx);
	void SetPress() { isPressed = !isPressed; }
	void SetRightPress() { isRightPressed = !isRightPressed; }
	float Intersection(Eigen::Vector3f sourcePoint);
	void RotateEye(float amt, bool upDown);
	~Assignment2(void);


};

