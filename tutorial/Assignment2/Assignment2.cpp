#include "Assignment2.h"
#include <iostream>


static void printMat(const Eigen::Matrix4d& mat)
{
	std::cout<<" matrix:"<<std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout<< mat(j,i)<<" ";
		std::cout<<std::endl;
	}
}

Assignment2::Assignment2(const std::string &filePath): sceneData(*SceneParser::parse(filePath)) {}

void Assignment2::Init()
{
    unsigned int texIDs[3] = { 0 , 1, 2};
    unsigned int slots[3] = { 0 , 1, 2 };

    AddShader("shaders/pickingShader");
    int shader = AddShader("shaders/raytracingShader");

    int met = AddMaterial(texIDs,slots, 1);
    AddMaterial(texIDs+1, slots+1, 1);

    int shape = AddShape(Plane, -1, TRIANGLES,0);
    SetShapeShader(shape,shader);
    SetShapeMaterial(shape, met);

    SetShapeStatic(shape);

}

void Assignment2::Update(const Eigen::Matrix4f& Proj, const Eigen::Matrix4f& View, const Eigen::Matrix4f& Model, unsigned int  shaderIndx, unsigned int shapeIndx)
{
    Shader *s = shaders[shaderIndx];

    s->Bind();

    s->SetUniformMat4f("Proj", Proj);
    s->SetUniformMat4f("View", View);
    s->SetUniformMat4f("Model", Model);

    s->SetUniform4fv("eye",&sceneData.eye,1);
    s->SetUniform4fv("ambient", &sceneData.ambient, 1);
    s->SetUniform4fv("objects",&sceneData.objects[0], (int)sceneData.objects.size());
    s->SetUniform4fv("objColors", &sceneData.colors[0], (int)sceneData.colors.size());
    s->SetUniform4fv("lightsPosition", &sceneData.lights[0], (int)sceneData.lights.size());
    s->SetUniform4fv("lightsDirection", &sceneData.directions[0], (int)sceneData.directions.size());
    s->SetUniform4fv("lightsIntensity", &sceneData.intensities[0], (int)sceneData.intensities.size());
    s->SetUniform4i("sizes", sceneData.sizes[0], sceneData.sizes[1], sceneData.sizes[2], sceneData.sizes[3]);
    s->Unbind();
}


void Assignment2::WhenRotate()
{
}

void Assignment2::WhenTranslate()
{
}

void Assignment2::Animate() {
    if(isActive)
	{
		
	}
}

void Assignment2::ScaleAllShapes(float amt,int viewportIndx)
{
	for (int i = 1; i < data_list.size(); i++)
	{
		if (data_list[i]->Is2Render(viewportIndx))
		{
            data_list[i]->MyScale(Eigen::Vector3d(amt, amt, amt));
		}
	}
}

Assignment2::~Assignment2(void) {}




