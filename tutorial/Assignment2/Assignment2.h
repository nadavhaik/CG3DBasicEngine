#pragma once
#include "igl/opengl/glfw/Viewer.h"
#include "sceneParser.h"

class Assignment2 : public igl::opengl::glfw::Viewer
{
private:
    SceneData sceneData;
public:
	
	Assignment2(const std::string &filePath);
//	Assignment2(float angle,float relationWH,float near, float far);
	void Init();
	void Update(const Eigen::Matrix4f& Proj, const Eigen::Matrix4f& View, const Eigen::Matrix4f& Model, unsigned int  shaderIndx, unsigned int shapeIndx);
	void WhenRotate();
	void WhenTranslate();
	void Animate() override;
	void ScaleAllShapes(float amt, int viewportIndx);
    void addObject(float eye_x, float eye_y, float eye_z, float obj_x, float obj_y, float obj_z, float obj_radius, Eigen::Vector4f color);
	
	~Assignment2(void);
};


