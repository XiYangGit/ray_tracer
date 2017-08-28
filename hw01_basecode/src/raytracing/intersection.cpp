#include <raytracing/intersection.h>
#include <iostream>
Intersection::Intersection():
    point(glm::vec3(0)),
    normal(glm::vec3(0)),
    t(-1),
    object_hit(nullptr)
{
}

IntersectionEngine::IntersectionEngine()
    : scene(nullptr)
{
}

Intersection IntersectionEngine::GetIntersection(Ray r)
{
    //TODO

    QList<Intersection> interall=GetAllIntersections(r);
    if(interall.size()==0)
        return Intersection();
    QList<float> distall;
    for(int i=0;i<interall.size();i++)
    {
        glm::vec3 dist=interall[i].point-r.origin;
        if(interall[i].object_hit==nullptr)
            distall.push_back(INFINITE);
        else
            distall.push_back(glm::length(dist));
    }
    int dist_min=0;
    for(int i=0;i<distall.size();i++)
    {
        if(distall[i]<distall[dist_min])
            dist_min=i;
    }

    return interall[dist_min];
}

QList<Intersection> IntersectionEngine::GetAllIntersections(Ray r)
{
    QList<Intersection> interall;
    for(int i=0;i<scene->objects.size();i++)
    {
        Geometry* obj_i=scene->objects[i];
        Intersection obj_inter=((Intersectable*)obj_i)->GetIntersection(r);
        if((obj_inter.object_hit!=nullptr)&&(obj_inter.t>0.0f))
        //if((obj_inter.object_hit!=nullptr))
            interall.push_back(obj_inter);
    }
    return interall;
}
