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
    scnData = *SceneParser::parse(file);
    xResolution = 800;
    yResolution = 800;
    //x = 0.5f;
    //y = 0;
    sourceIndx = -1;
    isRightPressed = false;
    isPressed = false;
}

//Assignment2::Assignment2(float angle ,float relationWH, float near, float far) : Scene(angle,relationWH,near,far)
//{
//}

void Assignment2::Init()
{
    unsigned int texIDs[3] = { 0 , 1, 2};
    unsigned int slots[3] = { 0 , 1, 2 };

    AddShader("shaders/pickingShader");
    AddShader("shaders/raytracingShader");
    //AddShader("shaders/nirRaytracingShader");

    AddTexture("textures/box0.bmp",2);
    AddTexture("textures/grass.bmp", 2);

    AddMaterial(texIDs,slots, 1);
    AddMaterial(texIDs+1, slots+1, 1);

    AddShape(Plane, -1, TRIANGLES,0);
    SetShapeShader(0,1);
    SetShapeMaterial(0, 0);
    SetShapeStatic(0);


    //Eigen::Vector4i sizes = scnData.sizes;		// total object num, lights num, reflecting object num, transmitting object num
    //std::cout << "sizes: " << sizes << std::endl;

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
    s->SetUniform4fv("eye",&scnData.eye,1);
    s->SetUniform4fv("ambient", &scnData.ambient, 1);
    s->SetUniform1f("tOffset", tOffset);

    Eigen::Vector4i sizes = scnData.sizes;				// total object num, lights num, reflecting object num, transmitting object num
    int tObjects = sizes[3];
    for (int i = 0; i < tObjects; i++) {
        float tObjW = scnData.objects[i][3];
        if (tObjW > 0 && tObjW < tOffset)
            scnData.objects[i][3] += tOffset;
    }

    for (int i = 0; i < tObjects; i++) {
        float tObjW = scnData.objects[i][3];
        if (tObjW >  tOffset)
            scnData.objects[i][3] -= tOffset;
    }
    s->SetUniform4fv("objects", &scnData.objects[0], scnData.objects.size()); // upload normal objects

    s->SetUniform4fv("objColors", &scnData.colors[0], scnData.colors.size());
    s->SetUniform4fv("lightsPosition", &scnData.lights[0], scnData.lights.size());
    s->SetUniform4fv("lightsDirection", &scnData.directions[0], scnData.directions.size());
    s->SetUniform4fv("lightsIntensity", &scnData.intensities[0], scnData.intensities.size());

    s->SetUniform4i("sizes", scnData.sizes[0], scnData.sizes[1], scnData.sizes[2], scnData.sizes[3]);
    if (data_list[shapeIndx]->GetMaterial() >= 0 && !materials.empty())
    {
        BindMaterial(s, data_list[shapeIndx]->GetMaterial());
    }
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
        xRel = -xOldPos + (float)x / xResolution;
        yRel = yOldPos - (float)y / yResolution;

        xOldPos += (float)xRel;
        yOldPos -= (float)(yRel);
        if (isRightPressed) // change location of object
        {
            if (sourceIndx >= 0)
            {
                scnData.objects[sourceIndx][0] += xRel * 2;
                scnData.objects[sourceIndx][1] += yRel * 2;
            }
        }
        else // change for left mouse click
        {


             //float angle = 1.5*EIGEN_PI;
             float angle = EIGEN_PI/3.0;

//             auto rotX = rotationMatrix(yRel * angle, Eigen::Vector3f(1, 0, 0));
//             auto rotY = rotationMatrix(xRel*angle, Eigen::Vector3f(0,1,0));
             auto rotX = rotationMatrix(yRel * angle, Eigen::Vector3f(1, 0, 0));
             auto rotY = rotationMatrix(xRel*angle, Eigen::Vector3f(0,1,0));
             for(auto &object : scnData.objects) {
                 object = rotY * rotX * object;
             }


            // } else if(xRel != 0) {
            // 	angle = xRel/abs(xRel) * EIGEN_PI;
            // } else {
            // 	angle = yRel/abs(yRel) * EIGEN_PI/2.0f;
            // }

            // scnData.eye[0] -= cos(angle);
            // scnData.eye[2] -= sin(angle);

            //nextRotationAngle += angle;

            // cant place eye as is. need to place coordinates of original vector
            // for (int i=0; i<scnData.objects; i++)
            // 	scnData.objects[i] = fullRotate(AxisType::Z,angle,scnData.objects[i]);
//            float offset = EIGEN_PI / (9.0);
//            scnData.eye = fullRotate(AxisType::Y, xRel*offset, scnData.eye);
//            scnData.eye = fullRotate(AxisType::X, yRel*offset, scnData.eye);

            // if (yRel !=0){
            // 	Eigen::Matrix4f rotMat = rotationMarix(AxisType::X, angle); // the rotation matrix to transform eye
            // 	scnData.eye[1]-=xRel;
            // 	scnData.eye[2]-=yRel;
            // 	scnData.eye = rotMat*scnData.eye;
            // 	scnData.eye[1]+=xRel;
            // 	scnData.eye[2]+=yRel;

            // }
            // if (xRel !=0){
            // 	Eigen::Matrix4f rotMat = rotationMarix(AxisType::Z, angle); // the rotation matrix to transform eye
            // 	scnData.eye[0]-=xRel;
            // 	scnData.eye[1]-=yRel;
            // 	scnData.eye = rotMat*scnData.eye;
            // 	scnData.eye[0]+=xRel;
            // 	scnData.eye[1]+=yRel;
            // }



            // auto m = rotationMarix(AxisType::Y,angle);
            // Eigen::Vector3f pEye;
            // pEye[0] = scnData.eye[0]-xRel;
            // pEye[1] = scnData.eye[1];
            // pEye[2] = scnData.eye[2]-yRel;

            // pEye = m*pEye;

            // scnData.eye[0] = pEye[0]+xRel;
            // scnData.eye[1] = pEye[1];
            // scnData.eye[2] = pEye[2]+yRel;


            // Eigen::Vector3f trans;
            // trans[0] = 1.0f;
            // trans[1] = 0.0f;
            // trans[2] = 1.0f;

            // trans = m * trans.normalized();
            // scnData.eye[0] += trans[0];
            // scnData.eye[1] += trans[1];
            // scnData.eye[2] += trans[2];



            // rotate around x - creates a zoom in effect
            // scnData.eye[1] = cosA*eyeP1-sinA*eyeP2+y;
            // scnData.eye[2] = sinA*eyeP1+cosA*eyeP2;

            // rotate around y - zoom in and out
            // scnData.eye[0] = cosA*eyeP0+sinA*eyeP2+x;
            // scnData.eye[2] = -sinA*eyeP0+cosA*eyeP2;

            // rotate around z - strange diagonal with x
            //scnData.eye[0] = cosA*eyeP0-sinA*eyeP1+x;
            //scnData.eye[1] = sinA*eyeP0+cosA*eyeP1+y;

            // second rotation

            // angle = yRel/20;
            // sinA = sin(angle);
            // cosA = cos(angle);
            // eyePointVec[0] = scnData.eye[0]-x;
            // eyePointVec[1] = scnData.eye[1]-y;
            // eyePointVec[2] = scnData.eye[2];
            // eyeP0 = eyePointVec[0];
            // eyeP1 = eyePointVec[1];
            // eyeP2 = eyePointVec[2];

            // scnData.eye[0] = cosA*eyeP0-sinA*eyeP1+x;
            // scnData.eye[1] = sinA*eyeP0+cosA*eyeP1+y;

        }
    }
    else
    {
        xOldPos = ((float)x / xResolution);
        yOldPos = (float)(y) / yResolution;
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
    Eigen::Vector3f v = (sourcePoint - Eigen::Vector3f(0, 0, scnData.eye[2] - scnData.eye[3])).normalized();
    sourcePoint = sourcePoint + Eigen::Vector3f(scnData.eye[0], scnData.eye[1], scnData.eye[3]);
    float tmin = 1.0e10;
    int indx = -1;
    for (int i = 0; i < scnData.sizes[0]; i++) //every object
    {
        if (scnData.objects[i][3] > 0) //sphere
        {
            Eigen::Vector3f p0o = (scnData.objects[i].head(3)) - sourcePoint;
            float r = scnData.objects[i][3];
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
            Eigen::Vector3f n = ((scnData.objects[i]).head(3)).normalized();
            Eigen::Vector3f p0o = ( - scnData.objects[i][3] * n) - scnData.objects[i].head(3);

            float t = (n.dot(p0o)) / (n.dot(v));
            if (t > 0 && t < tmin)
            {
                tmin = t;
                indx = i;
            }
        }
    }
    sourceIndx = indx;
    //std::cout<<"indx "<<indx<<std::endl;
    return tmin;
}

void Assignment2::RotateEye(float amt, bool upDown)
{
    float n = scnData.eye.norm();
    if (upDown)
        scnData.eye[1] += amt;
    else
        scnData.eye[0] += amt;
    //scnData.eye = scnData.eye.normalized()*n;
}

Assignment2::~Assignment2(void)
{
}