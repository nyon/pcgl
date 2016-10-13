#include "BaseModel.h"
#include "Model.h"

using namespace std;

BaseModel::BaseModel() : model(0)
{

}

BaseModel::~BaseModel()
{

}

Model* BaseModel::generate(const AABB& aabb)
{
    model = new Model(aabb);
    __init();
    return model;
}



/*
#define divide_macro(AXIS,UVEC,IDX) \
void BaseModel::divide_##AXIS(function<void()> left, function<void()> right, float dividor) \
{\
    pushOBB();\
    {\
        vec scale(1,1,1);\
        scale.AXIS = dividor;\
        vec center = vec(currentOBB()->CenterPoint());\
        float oldr = currentOBB()->r.AXIS;\
        currentOBB()->Scale(center, scale);\
        vec tr = vec(0,0,0);\
        tr.AXIS = -(oldr - currentOBB()->r.AXIS);\
        currentOBB()->Translate(tr);\
        left();\
    }\
    popOBB();\
\
    pushOBB();\
    {\
        vec scale(1,1,1);\
        scale.AXIS = 1.0f - dividor;\
        vec center = vec(currentOBB()->CenterPoint());\
        float oldr = currentOBB()->r.AXIS;\
        currentOBB()->Scale(center, scale);\
        vec tr = vec(0,0,0);\
        tr.AXIS = (oldr - currentOBB()->r.AXIS);\
        currentOBB()->Translate(tr);\
        right();\
    }\
    popOBB();\
}

divide_macro(x,unitX,0)
divide_macro(y,unitY,1)
divide_macro(z,unitZ,2)
*/

// source: http://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

// source: http://stackoverflow.com/questions/32685540/c-unordered-map-with-pair-as-key-not-compiling
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1,T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        hash_combine(h1, h2);
        return h1;
    }
};

void BaseModel::ellipsoid(float rx, float ry, float rz, int ellipsoidDivisions)
{
    int divisions = ellipsoidDivisions;
    if(divisions == -1) divisions = 7;
    //qDebug() << "START SPHERE " << divisions;
    const int numVertices = 24*(1<<(2*divisions));
    const int numTriangles = numVertices / 3;

    // Start subdividing from a diamond shape.
    ModelVertex *xp = ModelVertex::fromVector(1.0f,0,0);
    ModelVertex *xn = ModelVertex::fromVector(-1.0f, 0, 0);
    ModelVertex *yp = ModelVertex::fromVector(0, 1.0f, 0);
    ModelVertex *yn = ModelVertex::fromVector(0, -1.0f, 0);
    ModelVertex *zp = ModelVertex::fromVector(0, 0, 1.0f);
    ModelVertex *zn = ModelVertex::fromVector(0, 0, -1.0f);
    vector<ModelPolygon*> tempPolygons;
    vector<ModelVertex*> tempVertices;

    tempVertices.push_back(xp);
    tempVertices.push_back(xn);
    tempVertices.push_back(yp);
    tempVertices.push_back(yn);
    tempVertices.push_back(zp);
    tempVertices.push_back(zn);

    tempPolygons.reserve(numTriangles);
    tempPolygons.push_back(ModelPolygon::fromTriangle(yp,zp,xp));
    tempPolygons.push_back(ModelPolygon::fromTriangle(xp,zn,yp));
    tempPolygons.push_back(ModelPolygon::fromTriangle(yn,xp,zp));
    tempPolygons.push_back(ModelPolygon::fromTriangle(yn,zn,xp));
    tempPolygons.push_back(ModelPolygon::fromTriangle(zp,yp,xn));
    tempPolygons.push_back(ModelPolygon::fromTriangle(yp,zn,xn));
    tempPolygons.push_back(ModelPolygon::fromTriangle(yn,zp,xn));
    tempPolygons.push_back(ModelPolygon::fromTriangle(xn,zn,yn));

    unordered_map<pair<ModelVertex*, ModelVertex*>, ModelVertex*, pair_hash> edges;

    int oldEnd = 0;
    int iterationsPerDivision = 8;
    int startSphereAt = 0;
    //const AABB& aabb = model->currentAABB();
    //qDebug() << obb.ToString().c_str();
    //qDebug() << divisions;
    for(int division = divisions; division > 0; division--)
    {
        int nextIterationsPerDivision = 0;
        for(int iteration = 0; iteration < iterationsPerDivision; iteration++)
        {
            ModelPolygon* cur = tempPolygons.at(oldEnd);
            ModelVertex* a = cur->vertex(0);
            ModelVertex* b = cur->vertex(1);
            ModelVertex* c = cur->vertex(2);
            //if(aabb.Intersects(Triangle(a.pos, b.pos, c.pos).BoundingAABB()))
            {
                ModelVertex *newA;
                ModelVertex *newB;
                ModelVertex *newC;

                auto newAIterator = edges.find(make_pair(a, b));
                auto newBIterator = edges.find(make_pair(a, c));
                auto newCIterator = edges.find(make_pair(b, c));

                if(newAIterator == edges.end())
                {
                    vec newAVector = ((a->pos + b->pos) * 0.5f).Normalized();

                    newA = ModelVertex::fromVector(newAVector);
                    newA->normal = newAVector.Normalized();
                    edges.emplace(make_pair(a, b), newA);
                    edges.emplace(make_pair(b, a), newA);
                    tempVertices.push_back(newA);
                }
                else newA = newAIterator->second;

                if(newBIterator == edges.end())
                {
                    vec newBVector = ((a->pos + c->pos) * 0.5f).Normalized();

                    newB = ModelVertex::fromVector(newBVector);
                    newB->normal = newBVector.Normalized();
                    edges.emplace(make_pair(a, c), newB);
                    edges.emplace(make_pair(c, a), newB);
                    tempVertices.push_back(newB);
                }
                else newB = newBIterator->second;

                if(newCIterator == edges.end())
                {
                    vec newCVector = ((b->pos + c->pos) * 0.5f).Normalized();

                    newC = ModelVertex::fromVector(newCVector);
                    newC->normal = newCVector.Normalized();
                    edges.emplace(make_pair(b, c), newC);
                    edges.emplace(make_pair(c, b), newC);
                    tempVertices.push_back(newC);
                }
                else newC = newCIterator->second;

                tempPolygons.push_back(ModelPolygon::fromTriangle(a, newA, newB));
                tempPolygons.push_back(ModelPolygon::fromTriangle(b, newC, newA));
                tempPolygons.push_back(ModelPolygon::fromTriangle(c, newB, newC));
                tempPolygons.push_back(ModelPolygon::fromTriangle(newA, newC, newB));


                nextIterationsPerDivision += 4;
            }
            ++oldEnd;
        }
        iterationsPerDivision = nextIterationsPerDivision;
        nextIterationsPerDivision = 0;
        startSphereAt = oldEnd;
    }



    model->data()->addPolygons(tempPolygons.begin()+startSphereAt,tempPolygons.end());
    model->data()->addVertices(tempVertices);

    for (auto cur = tempPolygons.begin(); cur != tempPolygons.begin()+startSphereAt; ++cur)
    {
        delete *cur;
    }

    for(ModelVertex *vertex :  model->data()->vertexData)
    {
        vertex->pos.x *= rx;
        vertex->pos.y *= ry;
        vertex->pos.z *= rz;
        vertex->recalculateNormal();
    }


}


void BaseModel::sphere(float radius, int sphereDivisions)
{
    ellipsoid(radius, radius, radius, sphereDivisions);
}


void BaseModel::normalize(vec& vector)
{
    auto n = vector.Normalized();
    vector.x = n.x;
    vector.y = n.y;
    vector.z = n.z;
}

vec BaseModel::random_vector()
{
    LCG lcg;
    return vec::RandomDir(lcg);
}

void BaseModel::filter_selection_by_normal_angle(const vec& base_normal, float angle)
{
    model->data()->filterSelectionByNormalAngle(base_normal, angle);
}

void BaseModel::translate_selection_along_normals(float amount)
{
    model->data()->translateSelectionAlongNormals(amount);
}

void BaseModel::translate_selection(const vec& vector)
{
    model->data()->translateSelected(vector);
}

void BaseModel::recenter_selection()
{
    model->data()->recenterSelected();
}

void BaseModel::scale_selection(const vec& vector)
{
    model->data()->scaleSelected(vector);
}

void BaseModel::rotate_x_selection(float rot)
{
    model->data()->rotateXSelected(rot);
}
void BaseModel::rotate_y_selection(float rot)
{
    model->data()->rotateYSelected(rot);
}
void BaseModel::rotate_z_selection(float rot)
{
    model->data()->rotateZSelected(rot);

}

void BaseModel::push_model_state()
{
    model->pushModelState();

}

void BaseModel::push_clean_model_state()
{
    model->pushCleanModelState();

}

void BaseModel::pop_model_state()
{
    model->popModelState();


}

void BaseModel::pop_and_merge_model_state()
{
    model->popAndMergeModelState();
}




void BaseModel::select_sphere(const vec& center, float radius, float randomness, float detail)
{
    model->data()->selectSphere(center, radius, randomness, detail);
}

void BaseModel::select_x_gradient(const vec& center, float w)
{
    model->data()->selectXGradient(center, w);
}

void BaseModel::select_y_gradient(const vec& center, float w)
{
    model->data()->selectYGradient(center, w);
}

void BaseModel::select_z_gradient(const vec& center, float w)
{
    model->data()->selectZGradient(center, w);
}

void BaseModel::invert_selection()
{
    model->data()->invertSelection();
}

void BaseModel::randomize_selection(float amount)
{
    model->data()->randomizeSelection(amount);

}

void BaseModel::apply_noise_field(function<float (float, float, float)> func, float displace)
{
    model->data()->applyNoiseField(func, displace);
}

void BaseModel::evaluate(function<float(float,float,float)> implicit_func, float grid)
{
    vec corner[8];
    model->currentAABB().GetCornerPoints(corner);
    float minx = std::numeric_limits<float>::max();
    float miny = std::numeric_limits<float>::max();
    float minz = std::numeric_limits<float>::max();
    float maxx = std::numeric_limits<float>::min();
    float maxy = std::numeric_limits<float>::min();
    float maxz = std::numeric_limits<float>::min();
    for(int i = 0; i < 8; i++)
    {
        minx = min(minx, corner[i].x);
        miny = min(miny, corner[i].y);
        minz = min(minz, corner[i].z);

        maxx = max(maxx, corner[i].x);
        maxy = max(maxy, corner[i].y);
        maxz = max(maxz, corner[i].z);
    }

    for(float x = minx; x <= maxx; x+= grid)
    {
        for(float y = miny; y <= maxy; y+= grid)
        {
            for(float z = minz; z <= maxz; z+= grid)
            {
                implicit_func(x, y, z);
            }
        }
    }
}


SurfacePoint BaseModel::random_surface_point()
{
    return model->randomSurfacePoint();
}

SurfacePoint BaseModel::random_vertex()
{
    return model->randomVertex();
}

vec BaseModel::random_point()
{
    return model->randomPoint();
}

float BaseModel::random_float()
{
    LCG lcg;
    return lcg.Float01Incl();
}

void BaseModel::repeat(int times, function<void()> func)
{
    for(int i = 0; i < times; i++)
    {
        func();
    }
}

void BaseModel::select_all()
{
    model->data()->selectAll();
}

void BaseModel::select_none()
{
    model->data()->selectNone();
}

void BaseModel::combine(function<void()> left, function<void()> right)
{
    model->pushCleanModelState();
    left();

    model->pushCleanModelState();
    right();

    ModelData *rightModel = model->popModelState();
    ModelData *leftModel  = model->popModelState();

    ModelBSPOperation models(model->data(), leftModel, rightModel);

    models.combine();
}
