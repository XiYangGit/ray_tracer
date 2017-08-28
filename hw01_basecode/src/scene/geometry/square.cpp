#include <scene/geometry/square.h>
#include <iostream>
Intersection SquarePlane::GetIntersection(Ray r)
{
    //TODO
    //by Xi Yang
    Ray local_ray=r.GetTransformedCopy(transform.invT());
    Intersection in;
    glm::vec3 nor(0.0f,0.0f,1.0f);
    glm::vec3 s(-0.5f, 0.5f, 0);
    float dot_nd=glm::dot(nor,local_ray.direction);
    if(dot_nd!=0)
        in.t=glm::dot(nor,(s-local_ray.origin))/dot_nd;
    else
        return in;

    glm::vec3 intersectPt=local_ray.origin+in.t*local_ray.direction;

    if((fabs(intersectPt[0])<0.5f)&&(fabs(intersectPt[1])<0.5f))
    {
        glm::vec4 pt(intersectPt,1.0f);
        pt=transform.T()*pt;


        in.point=glm::vec3(pt);
    }
    else
        return in;
    in.object_hit=this;
    in.normal=glm::normalize(glm::vec3(transform.invTransT()*glm::vec4(nor,1.0f)));

    return in;
}

void SquarePlane::create()
{
    GLuint cub_idx[6];
    glm::vec3 cub_vert_pos[4];
    glm::vec3 cub_vert_nor[4];
    glm::vec3 cub_vert_col[4];

    cub_vert_pos[0] = glm::vec3(-0.5f, 0.5f, 0);  cub_vert_nor[0] = glm::vec3(0, 0, 1); cub_vert_col[0] = material->base_color;
    cub_vert_pos[1] = glm::vec3(-0.5f, -0.5f, 0); cub_vert_nor[1] = glm::vec3(0, 0, 1); cub_vert_col[1] = material->base_color;
    cub_vert_pos[2] = glm::vec3(0.5f, -0.5f, 0);  cub_vert_nor[2] = glm::vec3(0, 0, 1); cub_vert_col[2] = material->base_color;
    cub_vert_pos[3] = glm::vec3(0.5f, 0.5f, 0);   cub_vert_nor[3] = glm::vec3(0, 0, 1); cub_vert_col[3] = material->base_color;

    cub_idx[0] = 0; cub_idx[1] = 1; cub_idx[2] = 2;
    cub_idx[3] = 0; cub_idx[4] = 2; cub_idx[5] = 3;

    count = 6;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(cub_idx, 6 * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(cub_vert_pos, 4 * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(cub_vert_nor, 4 * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(cub_vert_col, 4 * sizeof(glm::vec3));
}
glm::vec2 SquarePlane::GetUVCoordinates(const glm::vec3 &point) const
{
    glm::mat4 inv=transform.inv();
    glm::vec3 local_p=glm::vec3(inv*glm::vec4(point,1));
    glm::vec2 UV;
    UV[0]=local_p[0]+0.5f;
    UV[1]=local_p[1]+0.5f;
    if(UV[0]>1.0)
        std::cout<<"square"<<std::endl;
    if(UV[1]>1.0)
        std::cout<<"square"<<std::endl;
    return UV;
}
