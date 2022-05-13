#version 330

uniform vec4 eye;
uniform vec4 ambient;
uniform vec4[20] objects;
uniform vec4[20] objColors;
uniform vec4[10] lightsDirection;
uniform vec4[10] lightsIntensity;
uniform vec4[10] lightsPosition;
uniform ivec4 sizes;
uniform int lastT;

in vec3 position0;
in vec3 normal0;

float intersection(inout int sourceIndx,vec3 sourcePoint,vec3 unitVectorRay)
{
    float tmin = 1.0e10;
    int minIndex = -1;
    for(int i=0;i<sizes.x;i++) //every object
    {
        if(i==sourceIndx)
        continue;
        if(objects[i].w > 0) //sphere
        {
            float radius = objects[i].w;
            vec3 p0o =  objects[i].xyz - sourcePoint;
            float b = dot(unitVectorRay,p0o);
            float delta = b*b - dot(p0o,p0o) + radius*radius;
            float t;
            if(delta >= 0)
            {
                if(b>=0)
                t = b - sqrt(delta);
                else
                t = b + sqrt(delta);
                if(t<tmin && t>0)
                {
                    tmin = t;
                    minIndex = i;
                }
            }
        }
        else  //plane
        {
            vec3 n =  normalize(objects[i].xyz);
            vec3 p0o = -objects[i].w*n/length(objects[i].xyz) - sourcePoint;
            float t = dot(n,p0o)/dot(n,unitVectorRay);
            if(t>0 && t<tmin)
            {
                tmin = t;
                minIndex = i;
            }
        }
    }
    sourceIndx = minIndex;
    return tmin;
}


//body index in objects, point on surface of object, diffuseFactor for plane squares
vec3 colorCalc(int sourceIndx, vec3 sourcePoint,vec3 u,float diffuseFactor)
{
    vec3 color = ambient.rgb*objColors[sourceIndx].rgb;
    float specularCoeff = 0.7f;
    for(int i = 0;i<sizes.y;i++) //every light source
    {
        vec3 unitVectorRay;
        if(lightsDirection[i].w < 0.5 ) //directional
        {
            int minIndex = sourceIndx;
            unitVectorRay = normalize(lightsDirection[i].xyz);
            float t = intersection(minIndex,sourcePoint,-unitVectorRay);


            if(minIndex < 0 || objects[minIndex].w<=0) //no intersection
            {
                if(objects[sourceIndx].w > 0) //sphere
                {
                    vec3 n = -normalize( sourcePoint - objects[sourceIndx].xyz);
                    vec3 refl;
                    if (sourceIndx < lastT)
                    refl = normalize(refract(2*unitVectorRay,n,sqrt(1.5)));
                    else
                    refl = normalize(reflect(unitVectorRay,n));
                    if(dot(unitVectorRay,n)>0.0 )
                    color+= max(specularCoeff * lightsIntensity[i].rgb * pow(dot(refl,u),objColors[sourceIndx].a),vec3(0.0,0.0,0.0));  //specular
                    color+= max(diffuseFactor * objColors[sourceIndx].rgb * lightsIntensity[i].rgb * dot(unitVectorRay,n),vec3(0.0,0.0,0.0)) ;  //difuse
                }
                else  //plane
                {
                    vec3 n = normalize(objects[sourceIndx].xyz);
                    vec3 refl = normalize(reflect(unitVectorRay,n));

                    color = min(color + max(specularCoeff * lightsIntensity[i].rgb * pow(dot(refl,u),objColors[sourceIndx].a),vec3(0.0,0.0,0.0)),vec3(1.0,1.0,1.0)); //specular
                    color = min( color + max(diffuseFactor * objColors[sourceIndx].rgb * lightsIntensity[i].rgb * dot(unitVectorRay,n),vec3(0.0,0.0,0.0)),vec3(1.0,1.0,1.0)); //difuse

                }
            }

        }
        else  //flashlight
        {
            int minIndex = -1;
            unitVectorRay = -normalize(lightsPosition[i].xyz - sourcePoint);
            if(dot(unitVectorRay,normalize(lightsDirection[i].xyz))<lightsPosition[i].w)
            {
                continue;
            }
            else
            {
                float t = intersection(minIndex,lightsPosition[i].xyz,unitVectorRay);
                if(minIndex == sourceIndx) //no intersection
                {
                    if(objects[sourceIndx].w > 0) //sphere
                    {
                        vec3 n = -normalize( sourcePoint - objects[sourceIndx].xyz);
                        vec3 refl;
                        if (sourceIndx >= lastT)
                        refl = normalize(reflect(unitVectorRay,n));
                        else
                        refl = normalize(refract(2*unitVectorRay,n,sqrt(1.5)));


                        if(dot(unitVectorRay,n)>0.0)
                        color+=max(specularCoeff * lightsIntensity[i].rgb * pow(dot(refl,u),objColors[sourceIndx].a),vec3(0.0,0.0,0.0)); //specular
                        color+= max(diffuseFactor * objColors[sourceIndx].rgb * lightsIntensity[i].rgb * dot(unitVectorRay,n),vec3(0.0,0.0,0.0));
                    }
                    else  //plane
                    {

                        vec3 n = normalize(objects[sourceIndx].xyz);
                        vec3 refl = normalize(reflect(unitVectorRay,n)); //specular
                        color = min(color + max(specularCoeff * lightsIntensity[i].rgb * pow(dot(refl,u),objColors[sourceIndx].a),vec3(0.0,0.0,0.0)),vec3(1.0,1.0,1.0));
                        color = min(color + max(diffuseFactor * objColors[sourceIndx].rgb * lightsIntensity[i].rgb *dot(unitVectorRay,n),vec3(0.0,0.0,0.0)),vec3(1.0,1.0,1.0));
                    }
                }
            }
        }
    }
    return min(color,vec3(1.0,1.0,1.0));
}

void main()
{
    vec3 eyeXY = eye.xyw;
    vec3 unitVectorRay = normalize( position0 + eyeXY - eye.xyz);
    int minIndex = -1;
    float minDistIntersect = intersection(minIndex,position0 + eyeXY ,unitVectorRay);

    if(minIndex < 0)
        discard;
    else
    {
        int counter = 5;
        vec3 p = position0 + eyeXY + minDistIntersect*unitVectorRay;
        vec3 n;
        while(counter>0 && minIndex<sizes.z-0.1f)
        {
            if(objects[minIndex].w <=0)
            n = normalize(objects[minIndex].xyz);
            else
            n = normalize(p - objects[minIndex].xyz);

            if (minIndex < lastT)
            unitVectorRay = normalize(refract(2*unitVectorRay,n,sqrt(1.5)));
            else
            unitVectorRay = normalize(reflect(unitVectorRay,n));


            minDistIntersect = intersection(minIndex,p,unitVectorRay);
            counter--;
            p = p + minDistIntersect*unitVectorRay;
        }

        float x = p.x;
        float y = p.y;


        if(objects[minIndex].w <= 0 && (((mod(int(1.5*x),2) == mod(int(1.5*y),2)) && ((x>0 && y>0) || (x<0 && y<0))) || ((mod(int(1.5*x),2) != mod(int(1.5*y),2) && ((x<0 && y>0) || (x>0 && y<0))))))
            gl_FragColor = vec4(colorCalc(minIndex,p,unitVectorRay,0.5),1);
        else
            gl_FragColor = vec4(colorCalc(minIndex,p,unitVectorRay,1.0),1);
    }
}
 

