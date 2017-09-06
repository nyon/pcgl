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

// source: http://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

// source: http://stackoverflow.com/questions/32685540/c-unordered-map-with-pair-as-key-not-compiling
struct pair_hash
{
    template <class T1, class T2>
    size_t operator () (const pair<T1,T2> &p) const
    {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        hash_combine(h1, h2);
        return h1;
    }
};

void BaseModel::ellipsoid(float xRadius, float yRadius, float zRadius, int ellipsoidDivisions)
{
    // Set sane default if no sane number is given as parameter
    if(ellipsoidDivisions < 1)
    {
        ellipsoidDivisions = 7;
    }

    // Precalculate number of triangles for allocation speedup
    const int numVertices = 24 * (1 << (2 * ellipsoidDivisions));
    const int numTriangles = numVertices / 3;

    // the resulting polygons and vertices are stored in the following
    // STL-containers
    vector<ModelPolygon*> generatedPolygons;
    vector<ModelVertex*> generatedVertices;

    // Start subdividing from a diamond shape
    ModelVertex *positiveXVertex = ModelVertex::fromVector( 1.0f,  0.0f,  0.0f);
    ModelVertex *negativeXVertex = ModelVertex::fromVector(-1.0f,  0.0f,  0.0f);
    ModelVertex *positiveYVertex = ModelVertex::fromVector( 0.0f,  1.0f,  0.0f);
    ModelVertex *negativeYVertex = ModelVertex::fromVector( 0.0f, -1.0f,  0.0f);
    ModelVertex *positiveZVertex = ModelVertex::fromVector( 0.0f,  0.0f,  1.0f);
    ModelVertex *negativeZVertex = ModelVertex::fromVector( 0.0f,  0.0f, -1.0f);

    generatedVertices.push_back(positiveXVertex);
    generatedVertices.push_back(negativeXVertex);
    generatedVertices.push_back(positiveYVertex);
    generatedVertices.push_back(negativeYVertex);
    generatedVertices.push_back(positiveZVertex);
    generatedVertices.push_back(negativeZVertex);

    generatedPolygons.reserve(numTriangles);
    generatedPolygons.push_back(ModelPolygon::fromTriangle(positiveYVertex,positiveZVertex,positiveXVertex));
    generatedPolygons.push_back(ModelPolygon::fromTriangle(positiveXVertex,negativeZVertex,positiveYVertex));
    generatedPolygons.push_back(ModelPolygon::fromTriangle(negativeYVertex,positiveXVertex,positiveZVertex));
    generatedPolygons.push_back(ModelPolygon::fromTriangle(negativeYVertex,negativeZVertex,positiveXVertex));
    generatedPolygons.push_back(ModelPolygon::fromTriangle(positiveZVertex,positiveYVertex,negativeXVertex));
    generatedPolygons.push_back(ModelPolygon::fromTriangle(positiveYVertex,negativeZVertex,negativeXVertex));
    generatedPolygons.push_back(ModelPolygon::fromTriangle(negativeYVertex,positiveZVertex,negativeXVertex));
    generatedPolygons.push_back(ModelPolygon::fromTriangle(negativeXVertex,negativeZVertex,negativeYVertex));

    // Lookup table for previously generated triangle edges
    unordered_map<pair<ModelVertex*, ModelVertex*>, ModelVertex*, pair_hash> edges;

    int polygonPivot = 0;
    int iterationsPerDivision = 8;
    int polygonStartPivot = 0;

    for(int division = ellipsoidDivisions; division > 0; division--)
    {
        int nextIterationsPerDivision = 0;
        for(int iteration = 0; iteration < iterationsPerDivision; iteration++)
        {
            ModelPolygon* currentPolygon = generatedPolygons.at(polygonPivot);
            ModelVertex* vertexA = currentPolygon->vertex(0);
            ModelVertex* vertexB = currentPolygon->vertex(1);
            ModelVertex* vertexC = currentPolygon->vertex(2);

            ModelVertex *vertexAB;
            ModelVertex *vertexAC;
            ModelVertex *vertexBC;

            auto newAIterator = edges.find(make_pair(vertexA, vertexB));
            auto newBIterator = edges.find(make_pair(vertexA, vertexC));
            auto newCIterator = edges.find(make_pair(vertexB, vertexC));

            if(newAIterator == edges.end())
            {
                vec newAVector = ((vertexA->pos + vertexB->pos) * 0.5f).Normalized();

                vertexAB = ModelVertex::fromVector(newAVector);
                vertexAB->normal = newAVector.Normalized();
                edges.emplace(make_pair(vertexA, vertexB), vertexAB);
                edges.emplace(make_pair(vertexB, vertexA), vertexAB);
                generatedVertices.push_back(vertexAB);
            }
            else
            {
                vertexAB = newAIterator->second;
            }

            if(newBIterator == edges.end())
            {
                vec newBVector = ((vertexA->pos + vertexC->pos) * 0.5f).Normalized();

                vertexAC = ModelVertex::fromVector(newBVector);
                vertexAC->normal = newBVector.Normalized();
                edges.emplace(make_pair(vertexA, vertexC), vertexAC);
                edges.emplace(make_pair(vertexC, vertexA), vertexAC);
                generatedVertices.push_back(vertexAC);
            }
            else
            {
                vertexAC = newBIterator->second;
            }

            if(newCIterator == edges.end())
            {
                vec newCVector = ((vertexB->pos + vertexC->pos) * 0.5f).Normalized();

                vertexBC = ModelVertex::fromVector(newCVector);
                vertexBC->normal = newCVector.Normalized();
                edges.emplace(make_pair(vertexB, vertexC), vertexBC);
                edges.emplace(make_pair(vertexC, vertexB), vertexBC);
                generatedVertices.push_back(vertexBC);
            }
            else
            {
                vertexBC = newCIterator->second;
            }

            generatedPolygons.push_back(ModelPolygon::fromTriangle(vertexA, vertexAB, vertexAC));
            generatedPolygons.push_back(ModelPolygon::fromTriangle(vertexB, vertexBC, vertexAB));
            generatedPolygons.push_back(ModelPolygon::fromTriangle(vertexC, vertexAC, vertexBC));
            generatedPolygons.push_back(ModelPolygon::fromTriangle(vertexAB, vertexBC, vertexAC));


            nextIterationsPerDivision += 4;

            polygonPivot++;
        }
        iterationsPerDivision = nextIterationsPerDivision;
        nextIterationsPerDivision = 0;
        polygonStartPivot = polygonPivot;
    }

    // Add polygons and vertices to the current model state
    model->data()->addPolygons(generatedPolygons.begin() + polygonStartPivot, generatedPolygons.end());
    model->data()->addVertices(generatedVertices);

    // Deallocate leftover polygons, that are generated in the subdivision process
    // and are no longer needed
    for(auto currentPolygonIterator = generatedPolygons.begin(); currentPolygonIterator != generatedPolygons.begin()+polygonStartPivot; ++currentPolygonIterator)
    {
        delete *currentPolygonIterator;
    }

    // Now scale every vertex to the given radius and recalculate the vertex normal
    for(ModelVertex *vertex :  model->data()->vertexData)
    {
        vertex->pos.x *= xRadius;
        vertex->pos.y *= yRadius;
        vertex->pos.z *= zRadius;
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
