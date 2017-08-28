#include "rendertask.h"
#include <iostream>
void RenderTask::run()
{
    for( int i=minPixel[0];i<=maxPixel[0];i++)
    {
        for( int j=minPixel[1];j<=maxPixel[1];j++)
        {
            Ray ray_ij=camera->Raycast(i,j);
            unsigned int dep=0;
            integrator->scene->film.pixels[i][j]=integrator->TraceRay(ray_ij,dep);
        }
    }

}
