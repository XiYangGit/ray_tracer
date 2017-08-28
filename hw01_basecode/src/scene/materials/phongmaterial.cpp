#include <scene/materials/phongmaterial.h>
#include <iostream>
PhongMaterial::PhongMaterial():
    PhongMaterial(glm::vec3(0.5f, 0.5f, 0.5f))
{}

PhongMaterial::PhongMaterial(const glm::vec3 &color):
    Material(color),
    specular_power(10)
{}

glm::vec3 PhongMaterial::EvaluateReflectedEnergy(const Intersection &isx, const glm::vec3 &outgoing_ray, const glm::vec3 &incoming_ray, const glm::vec3 &viewing_ray, Integrator const * const integrator,unsigned int dep,const glm::vec2 &uv) const
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
        glm::vec3 ref=glm::reflect(incoming_ray,norm);
        //glm::vec3 reflect_ray=glm::reflect(incoming_ray,isx.normal);
        float r_v_n=pow(glm::dot(ref,outgoing_ray),specular_power);

        float e=glm::dot(incoming_ray,isx.normal);
        if(e>1.0)
            e=1.0;
        if(e<0.0)
            e=0.0;
        glm::vec3 col_diffuse=base_color*e;
        glm::vec3 col_specular=base_color*r_v_n;
        basic_col=col_diffuse+col_specular;


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
        total_col*=uv_col;
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
