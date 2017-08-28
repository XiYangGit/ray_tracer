#include <scene/geometry/mesh.h>
#include <la.h>
#include <tinyobj/tiny_obj_loader.h>
#include <iostream>

Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3):
    Triangle(p1, p2, p3, glm::vec3(1), glm::vec3(1), glm::vec3(1), glm::vec2(0), glm::vec2(0), glm::vec2(0))
{
    for(int i = 0; i < 3; i++)
    {
        normals[i] = plane_normal;
    }
}


Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &n1, const glm::vec3 &n2, const glm::vec3 &n3):
    Triangle(p1, p2, p3, n1, n2, n3, glm::vec2(0), glm::vec2(0), glm::vec2(0))
{}


Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &n1, const glm::vec3 &n2, const glm::vec3 &n3, const glm::vec2 &t1, const glm::vec2 &t2, const glm::vec2 &t3)
{
    plane_normal = glm::normalize(glm::cross(p2 - p1, p3 - p2));
    points[0] = p1;
    points[1] = p2;
    points[2] = p3;
    normals[0] = n1;
    normals[1] = n2;
    normals[2] = n3;
    uvs[0] = t1;
    uvs[1] = t2;
    uvs[2] = t3;
}

//Returns the interpolation of the triangle's three normals based on the point inside the triangle that is given.
glm::vec3 Triangle::GetNormal(const glm::vec3 &position)
{
    float d0 = glm::distance(position, points[0]);
    float d1 = glm::distance(position, points[1]);
    float d2 = glm::distance(position, points[2]);
    return (normals[0] * d0 + normals[1] * d1 + normals[2] * d2)/(d0+d1+d2);
}

glm::vec4 Triangle::GetNormal(const glm::vec4 &position)
{
    glm::vec3 result = GetNormal(glm::vec3(position));
    return glm::vec4(result, 0);
}
float area(glm::vec3 v0,glm::vec3 v1,glm::vec3 v2)
{
    return 0.5*glm::length(glm::cross(v0-v1,v2-v1));
}
//The ray in this function is not transformed because it was *already* transformed in Mesh::GetIntersection
Intersection Triangle::GetIntersection(Ray r)
{
    //TODO
    Intersection in;
    glm::vec3 s=points[0];
    glm::vec3 nor=plane_normal;
    float dot_nd=glm::dot(nor,r.direction);

    if(dot_nd!=0)
        in.t=glm::dot(nor,(s-r.origin))/dot_nd;
    else
        return Intersection();
    glm::vec3 intersectPt=r.origin+in.t*r.direction;
    float ss=area(points[0],points[1],points[2]);
    float s0=area(intersectPt,points[1],points[2]);
    float s1=area(points[0],intersectPt,points[2]);
    float s2=area(points[0],points[1],intersectPt);
    //if((s0+s1+s2>ss)||(s0==0.0)||(s1==0.0)||(s2==0.0))
    if(s0+s1+s2-ss>FLT_EPSILON)
        return Intersection();
    in.point=glm::vec3(transform.T()*glm::vec4(intersectPt,1.0f));
    glm::vec3 nor_pt=GetNormal(intersectPt);
    in.normal=glm::normalize(glm::vec3(transform.invTransT()*glm::vec4(nor_pt,1.0f)));
    in.object_hit=this;

    return in;
}

Intersection Mesh::GetIntersection(Ray r)
{
    //TODO
    Ray local_ray=r.GetTransformedCopy(transform.invT());
    QList<float> faces_t;
    for(int i=0;i<faces.size();i++)
    {
        Intersection t_i=faces[i]->GetIntersection(local_ray);
        faces_t.push_back(t_i.t);
    }
    bool Intersect=false;
    int min_t=0;
    for(int i=0;i<faces.size();i++)
    {
        if(faces_t[i]>-1.0f)
        {
            Intersect=true;
            min_t=i;
            break;
        }
    }
    if(Intersect==false)
        return Intersection();

    for(int i=0;i<faces_t.size();i++)
    {
        if((faces_t[min_t]>faces_t[i])&&(faces_t[i]>-1.0))
            min_t=i;
    }
    Intersection in;
    in=faces[min_t]->GetIntersection(local_ray);
    in.object_hit=this;

    return in;
}

void Mesh::SetMaterial(Material *m)
{
    this->material = m;
    for(Triangle *t : faces)
    {
        t->SetMaterial(m);
    }
}


void Mesh::LoadOBJ(const QStringRef &filename, const QStringRef &local_path)
{
    QString filepath = local_path.toString(); filepath.append(filename);
    std::vector<tinyobj::shape_t> shapes; std::vector<tinyobj::material_t> materials;
    std::string errors = tinyobj::LoadObj(shapes, materials, filepath.toStdString().c_str());
    std::cout << errors << std::endl;
    if(errors.size() == 0)
    {
        //Read the information from the vector of shape_ts
        for(unsigned int i = 0; i < shapes.size(); i++)
        {
            std::vector<float> &positions = shapes[i].mesh.positions;
            std::vector<float> &normals = shapes[i].mesh.normals;
            std::vector<float> &uvs = shapes[i].mesh.texcoords;
            std::vector<unsigned int> &indices = shapes[i].mesh.indices;
            for(unsigned int j = 0; j < indices.size(); j += 3)
            {
                glm::vec3 p1(positions[indices[j]*3], positions[indices[j]*3+1], positions[indices[j]*3+2]);
                glm::vec3 p2(positions[indices[j+1]*3], positions[indices[j+1]*3+1], positions[indices[j+1]*3+2]);
                glm::vec3 p3(positions[indices[j+2]*3], positions[indices[j+2]*3+1], positions[indices[j+2]*3+2]);

                Triangle* t = new Triangle(p1, p2, p3);
                if(normals.size() > 0)
                {
                    glm::vec3 n1(normals[indices[j]*3], normals[indices[j]*3+1], normals[indices[j]*3+2]);
                    glm::vec3 n2(normals[indices[j+1]*3], normals[indices[j+1]*3+1], normals[indices[j+1]*3+2]);
                    glm::vec3 n3(normals[indices[j+2]*3], normals[indices[j+2]*3+1], normals[indices[j+2]*3+2]);
                    t->normals[0] = n1;
                    t->normals[1] = n2;
                    t->normals[2] = n3;
                }
                if(uvs.size() > 0)
                {
                    glm::vec2 t1(uvs[indices[j]*2], uvs[indices[j]*2+1]);
                    glm::vec2 t2(uvs[indices[j+1]*2], uvs[indices[j+1]*2+1]);
                    glm::vec2 t3(uvs[indices[j+2]*2], uvs[indices[j+2]*2+1]);
                    t->uvs[0] = t1;
                    t->uvs[1] = t2;
                    t->uvs[2] = t3;
                }
                this->faces.append(t);
            }
        }
        std::cout << "" << std::endl;
    }
    else
    {
        //An error loading the OBJ occurred!
        std::cout << errors << std::endl;
    }
}

void Mesh::create(){
    //Count the number of vertices for each face so we can get a count for the entire mesh
        std::vector<glm::vec3> vert_pos;
        std::vector<glm::vec3> vert_nor;
        std::vector<glm::vec3> vert_col;
        std::vector<GLuint> vert_idx;

        unsigned int index = 0;

        for(int i = 0; i < faces.size(); i++){
            Triangle* tri = faces[i];
            vert_pos.push_back(tri->points[0]); vert_nor.push_back(tri->normals[0]); vert_col.push_back(tri->material->base_color);
            vert_pos.push_back(tri->points[1]); vert_nor.push_back(tri->normals[1]); vert_col.push_back(tri->material->base_color);
            vert_pos.push_back(tri->points[2]); vert_nor.push_back(tri->normals[2]); vert_col.push_back(tri->material->base_color);
            vert_idx.push_back(index++);vert_idx.push_back(index++);vert_idx.push_back(index++);
        }

        count = vert_idx.size();
        int vert_count = vert_pos.size();

        bufIdx.create();
        bufIdx.bind();
        bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufIdx.allocate(vert_idx.data(), count * sizeof(GLuint));

        bufPos.create();
        bufPos.bind();
        bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufPos.allocate(vert_pos.data(), vert_count * sizeof(glm::vec3));

        bufCol.create();
        bufCol.bind();
        bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufCol.allocate(vert_col.data(), vert_count * sizeof(glm::vec3));

        bufNor.create();
        bufNor.bind();
        bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufNor.allocate(vert_nor.data(), vert_count * sizeof(glm::vec3));
}

//This does nothing because individual triangles are not rendered with OpenGL; they are rendered all together in their Mesh.
void Triangle::create(){}
glm::vec2 Triangle::GetUVCoordinates(const glm::vec3 &point) const
{
    float ss=area(points[0],points[1],points[2]);
    float s0=area(point,points[1],points[2]);
    float s1=area(points[0],point,points[2]);
    float s2=area(points[0],points[1],point);
    if(fabs(s0+s1+s2-ss)<FLT_EPSILON)
    {
        glm::vec2 UV=(uvs[0]*s0+uvs[1]*s1+uvs[2]*s2)/ss;
        return UV;
    }

    return glm::vec2();
}
glm::vec2 Mesh::GetUVCoordinates(const glm::vec3 &point) const
{
    for(unsigned int i=0;i<faces.size();i++)
    {
        glm::vec3 points[3]=faces[i]->points;
        glm::vec2 uvs[3]=faces[i]->uvs;
        float ss=area(points[0],points[1],points[2]);
        float s0=area(point,points[1],points[2]);
        float s1=area(points[0],point,points[2]);
        float s2=area(points[0],points[1],point);
        if(fabs(s0+s1+s2-ss)<FLT_EPSILON)
        {
            glm::vec2 UV=(uvs[0]*s0+uvs[1]*s1+uvs[2]*s2)/ss;
            if(UV[0]>1.0)
                std::cout<<"mesh"<<std::endl;
            if(UV[1]>1.0)
                std::cout<<"mesh"<<std::endl;
            return UV;
        }

    }


    return glm::vec2();
}
