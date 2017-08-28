#include <raytracing/ray.h>
#include <iostream>
Ray::Ray(const glm::vec3 &o, const glm::vec3 &d):
    origin(o),
    direction(d)
{}

Ray::Ray(const glm::vec4 &o, const glm::vec4 &d):
    Ray(glm::vec3(o), glm::vec3(d))
{}

Ray::Ray(const Ray &r):
    Ray(r.origin, r.direction)
{
}

Ray Ray::GetTransformedCopy(const glm::mat4 &T) const
{
    //TODO: Implement this correctly!

    glm::vec4 new_ori(origin,1);
    glm::vec4 new_dir(direction,0);
    new_ori=T*new_ori;
    new_dir=T*new_dir;
    glm::vec3 dir=glm::vec3(new_dir);
    dir=glm::normalize(dir);
    Ray newRay(glm::vec3(new_ori),dir);

    return newRay;
}
