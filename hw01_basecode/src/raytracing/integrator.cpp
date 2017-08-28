#include <raytracing/integrator.h>
#include<iostream>
bool comparefunction(const Intersection &t1,const Intersection &t2);
Integrator::Integrator():
    max_depth(5), scene(nullptr), intersection_engine(nullptr)
{
}

//Basic ray trace
glm::vec3 Integrator::TraceRay(Ray r, unsigned int depth) const
{
    //std::cout<<max_depth<<std::endl;

    Intersection inter_r=intersection_engine->GetIntersection(r);
    if(inter_r.object_hit==nullptr)
        return glm::vec3(0,0,0);
    QList<glm::vec3> cols;
    glm::vec2 uv=inter_r.object_hit->GetUVCoordinates(inter_r.point);
    for(int i=0;i<scene->lights.size();i++)
    {

        glm::vec3 incoming=glm::normalize(scene->lights[i]->transform.position()-inter_r.point);
        glm::vec3 viewing=glm::normalize(scene->camera.eye-inter_r.point);
        glm::vec3 outgoing;
        outgoing=glm::reflect(incoming,inter_r.normal);
        //glm::vec3 color1=inter_r.object_hit->material->EvaluateReflectedEnergy(inter_r,r.direction,incoming,viewing,this,depth,uv);
        glm::vec3 color1=inter_r.object_hit->material->EvaluateReflectedEnergy(inter_r,r.direction,incoming,viewing,this,depth,uv);
        float translate=0.0001f;
//        if(glm::dot(inter_r.normal,incoming)<0)
//            translate=-1.0*translate;
        Ray obj2lit(inter_r.point + translate * inter_r.normal,incoming);  //0.000025
        //Intersection back_inter=intersection_engine->GetIntersection(obj2lit);
        //glm::vec3 lightcolor =scene-> lights[i]->material->base_color;
        //color1=color1*lightcolor;
       // cols.push_back(color1);
        //shadow-------------
//        if(inter_r.object_hit->material->refract_idx_in>0.0f)
//            cols.push_back(color1);
//        else if((back_inter.object_hit == scene->lights[i])||(back_inter.object_hit == nullptr))
//        //if((back_inter.object_hit == scene->lights[i]))
//            cols.push_back(color1);
//        else if(back_inter.object_hit->material->refract_idx_in>0.0f)
//        {
            //shadow feeler
        if(inter_r.object_hit->material->refract_idx_in>0.0f)
        {
            glm::vec3 final_col=color1*scene-> lights[i]->material->base_color;
            glm::vec3 text_col=Material::GetImageColor(uv,inter_r.object_hit->material->texture);
            final_col=text_col*final_col;
            cols.push_back(final_col);
        }
        else{
            QList<Intersection> interall=intersection_engine->GetAllIntersections(obj2lit);

            QList<glm::vec3> shadow_col;
            qSort(interall.begin(),interall.end(),comparefunction);
            for(int i=0;i<interall.size();i++)
            {

                if(interall[i].object_hit->material->emissive==true)    //hit light
                {
                    shadow_col.push_back(interall[i].object_hit->material->base_color);
                    break;
                }
                else
                {
                    if(interall[i].object_hit->material->refract_idx_in>0.0f)   //hit transparent objects
                    {
                        glm::vec3 text_col=Material::GetImageColor(uv,interall[i].object_hit->material->texture);
                    shadow_col.push_back(text_col*interall[i].object_hit->material->base_color);
                    }
                    else
                    {
                        shadow_col.push_back(glm::vec3(0.0f));
                        break;
                    }
                }
            }
            glm::vec3 final_col(1.0f);
            for(unsigned int i=0;i<shadow_col.size();i++)
            {
                final_col=final_col*shadow_col[i];
            }
            final_col=final_col*color1;
            if(interall.size()==0)
                final_col=color1*scene-> lights[i]->material->base_color;
            cols.push_back(final_col);

        }
    }
    glm::vec3 color(0,0,0);
    if(cols.size()==0)
        return glm::vec3(0,0,0);
    for(int i=0;i<cols.size();i++)
    {
        color=color+cols[i];
    }
    color[0]=color[0]/cols.size();
    color[1]=color[1]/cols.size();
    color[2]=color[2]/cols.size();

    return color;

}

void Integrator::SetDepth(unsigned int depth)
{
    max_depth = depth;
}
//unsigned int Integrator::GetDepth()
//{
//    return max_depth;
//}
bool comparefunction(const Intersection &t1,const Intersection &t2)
{
    return t1.t<t2.t;
}
