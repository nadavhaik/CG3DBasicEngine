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


//Assignment2::Assignment2(float angle ,float relationWH, float near, float far) : Scene(angle,relationWH,near,far)
//{ 	
//}

void Assignment2::addObject(float eye_x, float eye_y, float eye_z, float obj_x, float obj_y, float obj_z, float obj_radius, Eigen::Vector4f color) {
    pickedShape = AddShape(Sphere, -1, TRIANGLES);
    int shape = pickedShape;
    ShapeTransformation(xTranslate, obj_x-eye_x, 0);
    ShapeTransformation(yTranslate, obj_y-eye_y, 0);
    ShapeTransformation(zTranslate, obj_z-eye_z, 0);
    ShapeTransformation(scaleAll, obj_radius, 0);
    SetShapeShader(pickedShape, 0);
    //SetShapeStatic(pickedShape);
}



void Assignment2::Init()
{

	unsigned int texIDs[3] = { 0 , 1, 2};
	unsigned int slots[3] = { 0 , 1, 2 };
	

	AddShader("shaders/basicShader");
	AddShader("shaders/pickingShader");
    AddShader("shaders/pickingShader");
    AddShader("shaders/cubemapShader");
	
	AddTexture("textures/box0.bmp",2);
	AddTexture("textures/cubemaps/Daylight Box_", 3);
	AddTexture("textures/grass.bmp", 2);
	//AddTexture("../res/textures/Cat_bump.jpg", 2);

	AddMaterial(texIDs,slots, 1);
//	AddMaterial(texIDs+1, slots+1, 1);
//	AddMaterial(texIDs + 2, slots + 2, 1);



    for(size_t i = 0; i<sceneData.objects.size(); i++) {
        addObject(sceneData.eye[0], sceneData.eye[1], sceneData.eye[2],
                  sceneData.objects[i][0], sceneData.objects[i][1], sceneData.objects[i][2], sceneData.objects[i][3],
                  sceneData.colors[i]);
    }
	pickedShape = -1;
//	AddShape(Cube, -2, TRIANGLES);
//	AddShape(Tethrahedron, -1, TRIANGLES);
//
//	AddShape(Octahedron, -1, TRIANGLES);
//	AddShape(Octahedron, 2, LINE_LOOP);
//    AddShape(Tethrahedron, 1, LINE_LOOP);
//
//    AddShape(Cube, -1, TRIANGLES);
//	AddShapeFromFile("data/sphere.obj", -1, TRIANGLES);
//	//AddShapeFromFile("../res/objs/Cat_v1.obj", -1, TRIANGLES);
//	AddShape(Plane, -2, TRIANGLES,3);
//
//	SetShapeShader(1, 2);
//	SetShapeShader(2, 2);
//	SetShapeShader(5, 2);
//	SetShapeShader(6, 3);
//	SetShapeMaterial(1, 0);
//	SetShapeMaterial(0, 1);
//	SetShapeMaterial(2, 2);
//	SetShapeMaterial(5, 2);
//	SetShapeMaterial(6, 0);
//	pickedShape = 0;
//	float s = 60;
//	ShapeTransformation(scaleAll, s,0);
//	pickedShape = 1;
//	ShapeTransformation(xTranslate, 10,0);
//
//	pickedShape = 5;
//	ShapeTransformation(xTranslate, -10,0);
//	pickedShape = 6;
//	ShapeTransformation(zTranslate, -1.1,0);
//	pickedShape = -1;
//	SetShapeStatic(0);
//	SetShapeStatic(6);

	//SetShapeViewport(6, 1);
//	ReadPixel(); //uncomment when you are reading from the z-buffer
}

void Assignment2::Update(const Eigen::Matrix4f& Proj, const Eigen::Matrix4f& View, const Eigen::Matrix4f& Model, unsigned int  shaderIndx, unsigned int shapeIndx)
{
	Shader *s = shaders[shaderIndx];
	int r = ((shapeIndx+1) & 0x000000FF) >>  0;
	int g = ((shapeIndx+1) & 0x0000FF00) >>  8;
	int b = ((shapeIndx+1) & 0x00FF0000) >> 16;


		s->Bind();
	s->SetUniformMat4f("Proj", Proj);
	s->SetUniformMat4f("View", View);
	s->SetUniformMat4f("Model", Model);
	if (data_list[shapeIndx]->GetMaterial() >= 0 && !materials.empty())
	{
//		materials[shapes[pickedShape]->GetMaterial()]->Bind(textures);
		BindMaterial(s, data_list[shapeIndx]->GetMaterial());
	}
	if (shaderIndx == 0)
		s->SetUniform4f("lightColor", r / 255.0f, g / 255.0f, b / 255.0f, 0.0f);
	else
		s->SetUniform4f("lightColor", 4/100.0f, 60 / 100.0f, 99 / 100.0f, 0.5f);
	//textures[0]->Bind(0);

	
	

	//s->SetUniform1i("sampler2", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(1));
	//s->SetUniform4f("lightDirection", 0.0f , 0.0f, -1.0f, 0.0f);
//	if(shaderIndx == 0)
//		s->SetUniform4f("lightColor",r/255.0f, g/255.0f, b/255.0f,1.0f);
//	else 
//		s->SetUniform4f("lightColor",0.7f,0.8f,0.1f,1.0f);
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




