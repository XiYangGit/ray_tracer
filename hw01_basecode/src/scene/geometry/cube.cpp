#include "cube.h"
#include <la.h>
#include <iostream>
#include<float.h>
#include <math.h>

static const int CUB_IDX_COUNT = 36;
static const int CUB_VERT_COUNT = 24;
const float far_clip=1500;
bool CheckCubeIntersection(Ray r, glm::vec3 &pt, glm::vec3 &nor);
bool CheckIntersection(Ray r, float &t);
Intersection Cube::GetIntersection(Ray r)
{
    //TODO

//    Intersection in;
//    Ray local_ray=r.GetTransformedCopy(transform.invT());
//    float t;
//    bool intersect=CheckIntersection(local_ray,t);
//    glm::vec3 intersectPt=local_ray.origin+t*local_ray.direction;
//    if(intersect)
//    {


//        in.object_hit=this;
//        in.point=glm::vec3(transform.T()*glm::vec4(intersectPt,1.0f));
//        in.t=t;
//        glm::vec3 nor;
//        if((fabs(intersectPt[0])>=fabs(intersectPt[1]))&&(fabs(intersectPt[0])>=fabs(intersectPt[2])))
//            nor=glm::vec3(intersectPt[0]/fabs(intersectPt[0]),0,0);
//        else if((fabs(intersectPt[1])>=fabs(intersectPt[0]))&&(fabs(intersectPt[1])>=fabs(intersectPt[2])))
//            nor=glm::vec3(0,intersectPt[1]/fabs(intersectPt[1]),0);
//        else if((fabs(intersectPt[2])>=fabs(intersectPt[1]))&&(fabs(intersectPt[2])>=fabs(intersectPt[0])))
//                nor=glm::vec3(0,0,intersectPt[2]/fabs(intersectPt[2]));
//        in.normal=glm::normalize(glm::vec3(transform.invTransT()*glm::vec4(nor,1.0f)));
//    }

//    return in;
        Intersection in;
        float length=0.5f;
        Ray r_Trans=r.GetTransformedCopy(transform.invT());
        glm::vec3 r_origin(r_Trans.origin);
        glm::vec3 r_direction(r_Trans.direction);
        float t_near=-INFINITE;
        float t_far=INFINITE;

        bool flag=true;
        for(int i=0;i<3;i++)
        {
            if(fabs(r_direction[i])<FLT_EPSILON)
            {
                if(fabs(r_origin[i])>length)
                {
                    flag=false;
                    break;
                }
            }
            else
            {
                float t0=(-length-r_origin[i])/r_direction[i];
                float t1=(length-r_origin[i])/r_direction[i];
                if(t0>t1)
                    std::swap(t0,t1);
                if(t0>t_near)
                    t_near=t0;
                if(t1<t_far)
                    t_far=t1;
            }
        }

        if(flag)
        {
            if(t_near<t_far&&t_near>0)
            {
                glm::vec3 intersect_point=r_origin+t_near*r_direction;
                glm::vec3 normal(0,0,0);
                int index=0;
                float largest=-1;
                for(int i=0;i<3;i++)
                {
                    if(fabs(intersect_point[i])>largest)
                    {
                        largest=fabs(intersect_point[i]);
                        index=i;
                    }
                }
                if(largest!=0.0f)
                    normal[index]=(intersect_point[index]/largest);

                in.point=glm::vec3(transform.T()*glm::vec4(intersect_point,1));
                in.t=t_near;
                in.normal=glm::normalize(glm::vec3(transform.invTransT()*glm::vec4(normal,0)));
                in.object_hit=this;
            }
        }

        return in;
}

bool CheckIntersection(Ray r,float &t)
{
    //x slab
    float t_near=-INFINITE;
    float t_far=INFINITE;
    if(r.direction[0]==0.0f)
    {
        if((r.origin[0]<-0.5f)||(r.origin[0]>0.5f))
            return false;
    }
    else
    {
        float t0=(-0.5f-r.origin[0])/r.direction[0];
        float t1=(0.5f-r.origin[0])/r.direction[0];
        if(t0>t1)
            std::swap(t0,t1);
        if(t0>t_near)
            t_near=t0;
        if(t1<t_far)
            t_far=t1;
        if(t_near>t_far)
            return false;
    }
    if(r.direction[1]==0.0f)
    {
        if((r.origin[1]<-0.5f)||(r.origin[1]>0.5f))
            return false;
    }
    else
    {
        float t0=(-0.5f-r.origin[1])/r.direction[1];
        float t1=(0.5f-r.origin[1])/r.direction[1];
        if(t0>t1)
            std::swap(t0,t1);
        if(t0>t_near)
            t_near=t0;
        if(t1<t_far)
            t_far=t1;
        if(t_near>t_far)
            return false;
    }
    if(r.direction[2]==0.0f)
    {
        if((r.origin[2]<-0.5f)||(r.origin[2]>0.5f))
            return false;
    }
    else
    {
        float t0=(-0.5f-r.origin[2])/r.direction[2];
        float t1=(0.5f-r.origin[2])/r.direction[2];
        if(t0>t1)
            std::swap(t0,t1);
        if(t0>t_near)
            t_near=t0;
        if(t1<t_far)
            t_far=t1;
        if(t_near>t_far)
            return false;
    }
    t=t_near;
    return true;

}

//These are functions that are only defined in this cpp file. They're used for organizational purposes
//when filling the arrays used to hold the vertex and index data.
void createCubeVertexPositions(glm::vec3 (&cub_vert_pos)[CUB_VERT_COUNT])
{
    int idx = 0;
    //Front face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, 0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, 0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, 0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, 0.5f);

    //Right face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, -0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, -0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, 0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, 0.5f);

    //Left face
    //UR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, 0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, 0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, -0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, -0.5f);

    //Back face
    //UR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, -0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, -0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, -0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, -0.5f);

    //Top face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, -0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, 0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, 0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, -0.5f);

    //Bottom face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, 0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, -0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, -0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, 0.5f);
}


void createCubeVertexNormals(glm::vec3 (&cub_vert_nor)[CUB_VERT_COUNT])
{
    int idx = 0;
    //Front
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,0,1);
    }
    //Right
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(1,0,0);
    }
    //Left
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(-1,0,0);
    }
    //Back
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,0,-1);
    }
    //Top
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,1,0);
    }
    //Bottom
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,-1,0);
    }
}

void createCubeIndices(GLuint (&cub_idx)[CUB_IDX_COUNT])
{
    int idx = 0;
    for(int i = 0; i < 6; i++){
        cub_idx[idx++] = i*4;
        cub_idx[idx++] = i*4+1;
        cub_idx[idx++] = i*4+2;
        cub_idx[idx++] = i*4;
        cub_idx[idx++] = i*4+2;
        cub_idx[idx++] = i*4+3;
    }
}

void Cube::create()
{
    GLuint cub_idx[CUB_IDX_COUNT];
    glm::vec3 cub_vert_pos[CUB_VERT_COUNT];
    glm::vec3 cub_vert_nor[CUB_VERT_COUNT];
    glm::vec3 cub_vert_col[CUB_VERT_COUNT];

    createCubeVertexPositions(cub_vert_pos);
    createCubeVertexNormals(cub_vert_nor);
    createCubeIndices(cub_idx);

    for(int i = 0; i < CUB_VERT_COUNT; i++){
        cub_vert_col[i] = material->base_color;
    }

    count = CUB_IDX_COUNT;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(cub_idx, CUB_IDX_COUNT * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(cub_vert_pos,CUB_VERT_COUNT * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(cub_vert_nor, CUB_VERT_COUNT * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(cub_vert_col, CUB_VERT_COUNT * sizeof(glm::vec3));

}
glm::vec2 Cube::GetUVCoordinates(const glm::vec3 &point) const
{
    glm::vec3 local_p=glm::vec3(transform.inv()*glm::vec4(point,1));
    glm::vec3 nor;
    if((fabs(local_p[0])>=fabs(local_p[1]))&&(fabs(local_p[0])>=fabs(local_p[2])))
                nor=glm::vec3(local_p[0]/fabs(local_p[0]),0,0);
            else if((fabs(local_p[1])>=fabs(local_p[0]))&&(fabs(local_p[1])>=fabs(local_p[2])))
                nor=glm::vec3(0,local_p[1]/fabs(local_p[1]),0);
            else if((fabs(local_p[2])>=fabs(local_p[1]))&&(fabs(local_p[2])>=fabs(local_p[0])))
                    nor=glm::vec3(0,0,local_p[2]/fabs(local_p[2]));
    nor=glm::normalize(nor);
    glm::vec2 uv;
//    //left
//    if(fabs(nor[0]+1)<FLT_EPSILON)
//    {
//        uv[0]=0.5-local_p.z;
//        uv[1]=1.5-local_p.y;
//    }
//    //right
//    else if(fabs(nor[0]-1)<FLT_EPSILON)
//    {
//        uv[0]=2.5+local_p.z;
//        uv[1]=1.5-local_p.y;
//    }
//    //bottom
//    else if(fabs(nor[1]+1)<FLT_EPSILON)
//    {
//        uv[0]=1.5+local_p.x;
//        uv[1]=1.5-local_p.y;
//    }
//    //top
//    else if(fabs(nor[1]-1)<FLT_EPSILON)
//    {
//        uv[0]=3.5-local_p.x;
//        uv[1]=1.5-local_p.y;
//    }
//    //back
//    else if(fabs(nor[2]+1)<FLT_EPSILON)
//    {
//        uv[0]=1.5+local_p.x;
//        uv[1]=2.5+local_p.z;
//    }
//    //front
//    else if(fabs(nor[2]-1)<FLT_EPSILON)
//    {
//        uv[0]=1.5+local_p.x;
//        uv[1]=0.5-local_p.z;
//    }
    //left
    if(fabs(nor[0]+1)<FLT_EPSILON)
    {
        uv[0]=0.5-local_p.y;
        uv[1]=1.5-local_p.z;
    }
    //right
    else if(fabs(nor[0]-1)<FLT_EPSILON)
    {
        uv[0]=2.5+local_p.y;
        uv[1]=1.5-local_p.z;
    }
    //bottom
    else if(fabs(nor[1]+1)<FLT_EPSILON)
    {
        uv[0]=1.5+local_p.x;
        uv[1]=1.5-local_p.z;
    }
    //top
    else if(fabs(nor[1]-1)<FLT_EPSILON)
    {
        uv[0]=3.5-local_p.x;
        uv[1]=1.5-local_p.z;
    }
    //back
    else if(fabs(nor[2]+1)<FLT_EPSILON)
    {
        uv[0]=1.5+local_p.x;
        uv[1]=2.5+local_p.y;
    }
    //front
    else if(fabs(nor[2]-1)<FLT_EPSILON)
    {
        uv[0]=1.5+local_p.x;
        uv[1]=0.5-local_p.y;
    }
    uv[0]=uv[0]*0.25;
    uv[1]=uv[1]/3.0f;
    if(uv[0]>1.0)
        std::cout<<"cube"<<std::endl;
    if(uv[1]>1.0)
        std::cout<<"cube"<<std::endl;
    return uv;
}
