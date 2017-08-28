#include <scene/materials/lambertmaterial.h>
#include <iostream>
LambertMaterial::LambertMaterial():Material()
{}

LambertMaterial::LambertMaterial(const glm::vec3 &color):Material(color)
{}

glm::vec3 LambertMaterial::EvaluateReflectedEnergy(const Intersection &isx,  const glm::vec3 &outgoing_ray, const glm::vec3 &incoming_ray,
                                                   const glm::vec3 &viewing_ray, Integrator const * const integrator,unsigned int dep,const glm::vec2 &uv) const
//{
//    glm::vec3 total_col;
//    //basic reflected energy
//    float e=glm::dot(incoming_ray,isx.normal);
//    if(e>1.0)
//        e=1.0;
//    if(e<0.0)
//        e=0.0;
//    glm::vec3 basic_col=base_color*e;

//    //specularReflectedEnergy
//    glm::vec3 specular_col;

//    //unsigned int maxdep=intergrator->GetDepth();
//    unsigned int maxdep=integrator->max_depth;
//    while(dep<maxdep)
//    {
//        if(reflectivity>0.0f)
//        {

//            dep++;
//            Ray reflected_ray(isx.point,outgoing_ray);
//            specular_col=integrator->TraceRay(reflected_ray,dep);

//        }
//        else
//            break;
////        if(reflectivity<=1.0f)
////            total_col=(1-reflectivity)*basic_col+reflectivity*specular_col;
//    }


//    if(reflectivity<=1.0f)
//        total_col=(1-reflectivity)*basic_col+reflectivity*specular_col;

//    for(int i=0;i<3;i++)
//    {
//        if(total_col[i]>1.0f)
//            total_col[i]=1.0f;
//        if(total_col[i]<0.0f)
//            total_col[i]=0.0f;
//    }



//    //return basic_col;
//    return total_col;
//    //return glm::vec3();
//}
//{
//    //uv mapping

//    glm::vec3 uv_col=GetImageColor(uv,texture);
//    glm::vec3 base_uv_col=uv_col*base_color;

//    //
//    unsigned int maxdep=integrator->max_depth;
//    glm::vec3 basic_col;
//    glm::vec3 total_col;
//    if((fabs(refract_idx_out)<FLT_EPSILON)||(fabs(refract_idx_out)<FLT_EPSILON))
//    {
//    float e=glm::dot(incoming_ray,isx.normal);
//    if(e>1.0)
//        e=1.0;
//    if(e<0.0)
//        e=0.0;
//    glm::vec3 col_diffuse=base_uv_col*e;
//    basic_col=col_diffuse;
//    }

//    //mirror ReflectedEnergy
//    glm::vec3 specular_col;
//    float translate;
//    if(glm::dot(viewing_ray,isx.normal)>0.0f)
//    {   //entering surface
//        translate=-0.0001f;
//    }
//    else
//    {   //leaving surface
//        translate=0.0001f;
//    }
//    if(reflectivity>0.0f)
//    {

//        if(dep<maxdep)
//        {

//            int depNext=dep+1;
//            glm::vec3 v_ref=glm::reflect(viewing_ray,isx.normal);
//            Ray reflected_ray(isx.point+translate*isx.normal,v_ref);
//            specular_col=integrator->TraceRay(reflected_ray,depNext);

//        }
//    }
//    specular_col=specular_col*basic_col;
//    if(reflectivity<=1.0f)
//        total_col=(1.0f-reflectivity)*basic_col+reflectivity*specular_col*base_uv_col;


//    for(int i=0;i<3;i++)
//    {
//        if(total_col[i]>1.0f)
//            total_col[i]=1.0f;
//        if(total_col[i]<0.0f)
//            total_col[i]=0.0f;

//    }
//    return total_col;


//}
{
    unsigned int maxdep=integrator->max_depth;
    glm::vec3 basic_col;
    glm::vec3 total_col;
    //uv mapping

        glm::vec3 uv_col=GetImageColor(uv,texture);

    if((fabs(refract_idx_out)<FLT_EPSILON)||(fabs(refract_idx_out)<FLT_EPSILON))
    {

        glm::vec3 norm;
        norm=isx.normal;
        float translate;
        if(glm::dot(outgoing_ray,isx.normal)<0.0f)
        {   //entering surface
            translate=0.0001f;
            norm=isx.normal;
        }
        else
        {   //leaving surface
            translate=-0.0001f;
            norm=-1.0f*isx.normal;
        }
        float e=glm::dot(incoming_ray,isx.normal);
        if(e>1.0)
            e=1.0;
        if(e<0.0)
            e=0.0;
        glm::vec3 col_diffuse=base_color*e;
        basic_col=col_diffuse*uv_col;


        //mirror ReflectedEnergy
        glm::vec3 specular_col;
        if((reflectivity>0.0f)&&(reflectivity<=1.0f))
        {

            if(dep<maxdep)
            {

                int depNext=dep+1;
                glm::vec3 v_ref=glm::reflect(outgoing_ray,norm);
                Ray reflected_ray(isx.point+translate*isx.normal,v_ref);
                specular_col=integrator->TraceRay(reflected_ray,depNext);

            }
        }
        total_col=(1-reflectivity)*basic_col+reflectivity*specular_col*basic_col;
        total_col=glm::clamp(total_col,0.0f,1.0f);
        return total_col;
    }

    else
    {
        //compute refraction ray

        //Ray refracted_ray;
        float refract_ratio;
        glm::vec3 total_col;
        float translate;
        glm::vec3 norm;
        if(glm::dot(outgoing_ray,isx.normal)<0.0f)
        {    refract_ratio=refract_idx_out/refract_idx_in;//entering surface
            translate=-0.0001f;
            norm=isx.normal;
        }
        else
        {    refract_ratio=refract_idx_in/refract_idx_out;//leaving surface
            translate=0.0001f;
            norm=-1.0f*isx.normal;
        }

        glm::vec3 Incident_ray = outgoing_ray;
        float dot_in=glm::dot(norm,Incident_ray);
        float delta=1-refract_ratio*refract_ratio*(1-pow(dot_in,2));
        if(delta<0.0f)
        {
            //mirror
            glm::vec3 mirror_col;
            if(dep<maxdep)
            {

                int depNext=dep+1;
                glm::vec3 v_ref=glm::reflect(outgoing_ray,norm);
                Ray reflected_ray(isx.point-translate*isx.normal,v_ref);
                mirror_col=integrator->TraceRay(reflected_ray,depNext);

            }
            total_col=mirror_col;

        }else
        {
            glm::vec3 T=((float)(-1.0*refract_ratio*dot_in-sqrt(delta)))*norm+refract_ratio*Incident_ray;

            glm::vec3 mirror_col;
            glm::vec3 transmission_col;

            Ray   refracted_ray(isx.point + translate * isx.normal,T);
            if(dep<maxdep)
            {
                int depNext = dep+1;
                transmission_col=integrator->TraceRay(refracted_ray,depNext);
                if(reflectivity>0.0f)
                {
                    glm::vec3 v_ref=glm::reflect(outgoing_ray,norm);
                    Ray reflected_ray(isx.point-translate*isx.normal,v_ref);
                    mirror_col=integrator->TraceRay(reflected_ray,depNext);


                }



            }
            total_col=(1-reflectivity)*transmission_col+reflectivity*mirror_col;
            total_col*=base_color;
        }


        total_col*=uv_col;
        total_col=glm::clamp(total_col,0.0f,1.0f);
        return total_col;

    }
}

