#include "Assignment2.h"
#include <iostream>
#include <math.h>

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

Assignment2::Assignment2(std::string file)
{
    sceneData = *SceneParser::parse(file);
    xRes = 800;
    yRes = 800;
    sourceIndx = -1;
    isRightPressed = false;
    isPressed = false;
}


void Assignment2::Init()
{
    unsigned int texIDs[3] = { 0 , 1, 2};
    unsigned int slots[3] = { 0 , 1, 2 };

    AddShader("shaders/pickingShader");
    AddShader("shaders/raytracingShader");

    AddTexture("textures/box0.bmp",2);
    AddTexture("textures/grass.bmp", 2);

    AddMaterial(texIDs,slots, 1);
    AddMaterial(texIDs+1, slots+1, 1);

    AddShape(Plane, -1, TRIANGLES,0);
    SetShapeShader(0,1);
    SetShapeMaterial(0, 0);
    SetShapeStatic(0);


    for (size_t i = 0; i < sceneData.objects.size(); i++) {
        if(i<sceneData.sizes[3])
            transObjects.push_back(sceneData.objects[i]);
        else
            noneTransObjects.push_back(sceneData.objects[i]);
    }

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

    s->SetUniform1i("lastT", transObjects.size());
    s->SetUniform4fv("objects", &sceneData.objects[0], sceneData.objects.size()); // upload normal objects

    s->SetUniform4fv("objColors", &sceneData.colors[0], sceneData.colors.size());
    s->SetUniform4fv("lightsPosition", &sceneData.lights[0], sceneData.lights.size());
    s->SetUniform4fv("lightsDirection", &sceneData.directions[0], sceneData.directions.size());
    s->SetUniform4fv("lightsIntensity", &sceneData.intensities[0], sceneData.intensities.size());
    s->SetUniform4i("sizes", sceneData.sizes[0], sceneData.sizes[1], sceneData.sizes[2], sceneData.sizes[3]);
    s->Unbind();
}

// setting position of object in case of right mouse click

static Eigen::Matrix4f rotationMatrix(float angle, Eigen::Vector3f vec)
{
    vec = vec.normalized();
    Eigen::Matrix4f target = Eigen::Matrix4f::Zero();
    target(0, 0) = cos(angle) + vec[0]*vec[0]*(1.0f-cos(angle));
    target(0, 1) = vec[0]*vec[1]*(1.0f-cos(angle)) - vec[2]*sin(angle);
    target(0, 2) = vec[0]*vec[2]*(1.0f-cos(angle)) + vec[1]*sin(angle);

    target(1, 0) = vec[1] * vec[0]*(1.0f-cos(angle)) + vec[2]*sin(angle);
    target(1, 1) = cos(angle) + vec[1]*vec[1]*(1.0f-cos(angle));
    target(1, 2) = vec[1]*vec[2]*(1.0f-cos(angle))-vec[0]*sin(angle);

    target(2, 0) = vec[2]*vec[0]*(1.0f-cos(angle))-vec[1]*sin(angle);
    target(2, 1) = vec[2]*vec[1]*(1.0f-cos(angle))+vec[0]*sin(angle);
    target(2, 2) = cos(angle)+vec[2]*vec[2]*(1.0f-cos(angle));

    target(3, 3) = 1.0f;

    return target;


}
void Assignment2::SetPosition(int x, int y)
{
    if (isPressed)
    {
        xRel = -xOldPos + (float)x / xRes;
        yRel = yOldPos - (float)y / yRes;

        xOldPos += (float)xRel;
        yOldPos -= (float)(yRel);
        if (isRightPressed) // change location of object
        {
            if (sourceIndx >= 0)
            {
                sceneData.objects[sourceIndx][0] += xRel * 2;
                sceneData.objects[sourceIndx][1] += yRel * 2;
            }
        }
        else // change for left mouse click
        {
             float angle = EIGEN_PI/3.0;
             auto rotX = rotationMatrix(yRel * angle, Eigen::Vector3f(1, 0, 0));
             auto rotY = rotationMatrix(xRel*angle, Eigen::Vector3f(0,1,0));
             for(auto &object : sceneData.objects)
                 object = rotY * rotX * object;
        }
    }
    else
    {
        xOldPos = ((float)x / xRes);
        yOldPos = (float)(y) / yRes;
        //isPressed = true;
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

float Assignment2::Intersection(Eigen::Vector3f sourcePoint)
{
    Eigen::Vector3f v = (sourcePoint - Eigen::Vector3f(0, 0, sceneData.eye[2] - sceneData.eye[3])).normalized();
    sourcePoint = sourcePoint + Eigen::Vector3f(sceneData.eye[0], sceneData.eye[1], sceneData.eye[3]);
    float tmin = 1.0e10;
    int indx = -1;
    for (int i = 0; i < sceneData.sizes[0]; i++) //every object
    {
        if (sceneData.objects[i][3] > 0) //sphere
        {
            Eigen::Vector3f p0o = (sceneData.objects[i].head(3)) - sourcePoint;
            float r = sceneData.objects[i][3];
            if (r > tOffset)
                r -= tOffset;
            float b = v.dot(p0o);
            float delta = b * b - p0o.dot(p0o) + r * r;
            float t;
            if (delta >= 0)
            {
                if (b >= 0)
                    t = b - sqrt(delta);
                else
                    t = b + sqrt(delta);
                if (t < tmin && t>0)
                {
                    tmin = t;
                    indx = i;
                }
            }
        }
        else  //plane
        {
            Eigen::Vector3f n = ((sceneData.objects[i]).head(3)).normalized();
            Eigen::Vector3f p0o = (- sceneData.objects[i][3] * n) - sceneData.objects[i].head(3);

            float t = (n.dot(p0o)) / (n.dot(v));
            if (t > 0 && t < tmin)
            {
                tmin = t;
                indx = i;
            }
        }
    }
    sourceIndx = indx;
    return tmin;
}

void Assignment2::RotateEye(float amt, bool upDown)
{
    if (upDown)
        sceneData.eye[1] += amt;
    else
        sceneData.eye[0] += amt;
}

Assignment2::~Assignment2(void)
{
}