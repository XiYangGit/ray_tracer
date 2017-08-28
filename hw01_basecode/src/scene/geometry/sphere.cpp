#include "sphere.h"

#include <iostream>

#include <la.h>

static const int SPH_IDX_COUNT = 2280;  // 760 tris * 3
static const int SPH_VERT_COUNT = 382;
const float pai=3.1415926;
Intersection Sphere::GetIntersection(Ray r)
{
    //TODO
    Ray local_ray=r.GetTransformedCopy(transform.invT());
    Intersection in;
    float A=local_ray.direction[0]*local_ray.direction[0]+local_ray.direction[1]*local_ray.direction[1]+local_ray.direction[2]*local_ray.direction[2];
    float B=2*(local_ray.direction[0]*local_ray.origin[0]+local_ray.direction[1]*local_ray.origin[1]+local_ray.direction[2]*local_ray.origin[2]);
    float C=local_ray.origin[0]*local_ray.origin[0]+local_ray.origin[1]*local_ray.origin[1]+local_ray.origin[2]*local_ray.origin[2]-0.5*0.5;
    float Discriminant=B*B-4*A*C;
    if(Discriminant<FLT_EPSILON)
        return in;
    else
    {
        //if(this->name=="Transparent Sphere 1")
            //int d=0;
           // std::cout<<"here"<<std::endl;
        float t0=(-1*B-sqrt(Discriminant))/(2*A);
        if(t0<FLT_EPSILON)
            t0=(-1*B+sqrt(Discriminant))/(2*A);
        in.t=t0;
        in.object_hit=this;
        glm::vec3 intersectPt=local_ray.origin+in.t*local_ray.direction;
        //in.point=glm::vec3(transform.T()*glm::vec4(intersectPt,1.0f));
        in.point=glm::vec3(transform.T()*glm::vec4(intersectPt,1.0f));
        //in.point=r.origin+in.t*r.direction;
        glm::vec3 nor=glm::normalize(intersectPt);
        in.normal=glm::normalize(glm::vec3(transform.invTransT()*glm::vec4(nor,1.0f)));
    }
    return in;
//        Intersection P;

//        Ray r_Trans=r.GetTransformedCopy(transform.invT());
//        glm::vec3 r_origin(r_Trans.origin);
//        glm::vec3 r_direction(r_Trans.direction);
//        glm::vec3 center(0,0,0);
//        float A=pow(r_direction[0],2.0)+pow(r_direction[1],2.0)+pow(r_direction[2],2.0),\
//                B=2*(r_direction[0]*r_origin[0]+r_direction[1]*r_origin[1]+r_direction[2]*r_origin[2]),\
//                C=pow(r_origin[0],2)+pow(r_origin[1],2)+pow(r_origin[2],2)-0.5*0.5;
//        float Discriminant=pow(B,2)-4*A*C;
//        if(Discriminant>=0.0f)
//        {
//            if(this->name=="Transparent Sphere 1")
//                //int d=0;
//                std::cout<<"here"<<std::endl;
//            float t_solver=0;
//            float t0=(-B-sqrt(Discriminant))/(2*A);
//            float t1=(-B+sqrt(Discriminant))/(2*A);
//            if(t0>0.0f)
//                t_solver=t0;
//            else if(t1>0.0f)
//                t_solver=t1;
//            if(t_solver>0.0f)
//            {
//                glm::vec3 intersect_point=r_origin+t_solver*r_Trans.direction;
//                glm::vec3 normal=glm::normalize(intersect_point-center);
//                P.point=glm::vec3(transform.T()*glm::vec4(intersect_point,1.0f));
//                P.t=t_solver;
//                P.normal=glm::normalize(glm::vec3(transform.invTransT()*glm::vec4(normal,0.0f)));
//                P.object_hit=this;
//            }
//        }
}

// These are functions that are only defined in this cpp file. They're used for organizational purposes
// when filling the arrays used to hold the vertex and index data.
void createSphereVertexPositions(glm::vec3 (&sph_vert_pos)[SPH_VERT_COUNT])
{
    // Create rings of vertices for the non-pole vertices
    // These will fill indices 1 - 380. Indices 0 and 381 will be filled by the two pole vertices.
    glm::vec4 v;
    // i is the Z axis rotation
    for (int i = 1; i < 19; i++) {
        // j is the Y axis rotation
        for (int j = 0; j < 20; j++) {
            v = glm::rotate(glm::mat4(1.0f), j / 20.f * TWO_PI, glm::vec3(0, 1, 0))
                * glm::rotate(glm::mat4(1.0f), -i / 18.0f * PI, glm::vec3(0, 0, 1))
                * glm::vec4(0, 0.5f, 0, 1);
            sph_vert_pos[(i - 1) * 20 + j + 1] = glm::vec3(v);
        }
    }
    // Add the pole vertices
    sph_vert_pos[0] = glm::vec3(0, 0.5f, 0);
    sph_vert_pos[381] = glm::vec3(0, -0.5f, 0);  // 361 - 380 are the vertices for the bottom cap
}


void createSphereVertexNormals(glm::vec3 (&sph_vert_nor)[SPH_VERT_COUNT])
{
    // Unlike a cylinder, a sphere only needs to be one normal per vertex
    // because a sphere does not have sharp edges.
    glm::vec4 v;
    // i is the Z axis rotation
    for (int i = 1; i < 19; i++) {
        // j is the Y axis rotation
        for (int j = 0; j < 20; j++) {
            v = glm::rotate(glm::mat4(1.0f), j / 20.0f * TWO_PI, glm::vec3(0, 1, 0))
                * glm::rotate(glm::mat4(1.0f), -i / 18.0f * PI, glm::vec3(0, 0, 1))
                * glm::vec4(0, 1.0f, 0, 0);
            sph_vert_nor[(i - 1) * 20 + j + 1] = glm::vec3(v);
        }
    }
    // Add the pole normals
    sph_vert_nor[0] = glm::vec3(0, 1.0f, 0);
    sph_vert_nor[381] = glm::vec3(0, -1.0f, 0);
}


void createSphereIndices(GLuint (&sph_idx)[SPH_IDX_COUNT])
{
    int index = 0;
    // Build indices for the top cap (20 tris, indices 0 - 60, up to vertex 20)
    for (int i = 0; i < 19; i++) {
        sph_idx[index] = 0;
        sph_idx[index + 1] = i + 1;
        sph_idx[index + 2] = i + 2;
        index += 3;
    }
    // Must create the last triangle separately because its indices loop
    sph_idx[57] = 0;
    sph_idx[58] = 20;
    sph_idx[59] = 1;
    index += 3;

    // Build indices for the body vertices
    // i is the Z axis rotation
    for (int i = 1; i < 19; i++) {
        // j is the Y axis rotation
        for (int j = 0; j < 20; j++) {
            sph_idx[index] = (i - 1) * 20 + j + 1;
            sph_idx[index + 1] = (i - 1) * 20 +  j + 2;
            sph_idx[index + 2] = (i - 1) * 20 +  j + 22;
            sph_idx[index + 3] = (i - 1) * 20 +  j + 1;
            sph_idx[index + 4] = (i - 1) * 20 +  j + 22;
            sph_idx[index + 5] = (i - 1) * 20 +  j + 21;
            index += 6;
        }
    }

    // Build indices for the bottom cap (20 tris, indices 2220 - 2279)
    for (int i = 0; i < 19; i++) {
        sph_idx[index] = 381;
        sph_idx[index + 1] = i + 361;
        sph_idx[index + 2] = i + 362;
        index += 3;
    }
    // Must create the last triangle separately because its indices loop
    sph_idx[2277] = 381;
    sph_idx[2278] = 380;
    sph_idx[2279] = 361;
    index += 3;
}

void Sphere::create()
{
    GLuint sph_idx[SPH_IDX_COUNT];
    glm::vec3 sph_vert_pos[SPH_VERT_COUNT];
    glm::vec3 sph_vert_nor[SPH_VERT_COUNT];
    glm::vec3 sph_vert_col[SPH_VERT_COUNT];

    createSphereVertexPositions(sph_vert_pos);
    createSphereVertexNormals(sph_vert_nor);
    createSphereIndices(sph_idx);
    for (int i = 0; i < SPH_VERT_COUNT; i++) {
        sph_vert_col[i] = material->base_color;
    }

    count = SPH_IDX_COUNT;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(sph_idx, SPH_IDX_COUNT * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(sph_vert_pos, SPH_VERT_COUNT * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(sph_vert_col, SPH_VERT_COUNT * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(sph_vert_nor, SPH_VERT_COUNT * sizeof(glm::vec3));
}
glm::vec2 Sphere::GetUVCoordinates(const glm::vec3 &point) const
{

    glm::mat4 inv=transform.inv();
    glm::vec3 local_p=glm::vec3(inv*glm::vec4(point,1));
    local_p=glm::normalize(local_p);
    float fai;
    if(fabs(local_p.x)<FLT_EPSILON)
        fai=pai/2;
    else
        fai=atan2f(local_p.z, local_p.x)+pai;

    float theta=acos(local_p.y);
    glm::vec2 UV;
    UV[0]=1-fai/(2*pai);
    UV[1]=1-theta/pai;
    if(UV[0]>1.0)
        std::cout<<"sphere"<<std::endl;
    if(UV[1]>1.0)
        std::cout<<"sphere"<<std::endl;
    return UV;
}
