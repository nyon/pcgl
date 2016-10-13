#include "ModelVertex.h"

ModelVertex::ModelVertex() : ModelVertex(vec())
{

}

ModelVertex::ModelVertex(const vec& pos) : pos(pos), selection(0.0f), normal(0,0,0)
{

}

ModelVertex::~ModelVertex()
{
    for(ModelPolygon *polygon : connectedPolygons)
    {
        polygon->announceDeath(this);
    }
}

ModelVertex* ModelVertex::fromVector(const vec& pos)
{
    return new ModelVertex(pos);
}

ModelVertex* ModelVertex::fromVector(float x, float y, float z)
{
    return ModelVertex::fromVector(vec(x,y,z));
}

void ModelVertex::announceDeath(ModelPolygon* polygon)
{
    auto index = std::find(connectedPolygons.begin(), connectedPolygons.end(), polygon);
    if(index != connectedPolygons.end())
    {
        connectedPolygons.erase(index);
    }
}

void ModelVertex::addDependantPolygon(ModelPolygon *polygon)
{
    connectedPolygons.push_back(polygon);
}

void ModelVertex::recalculateNormal()
{
    vec normalAggregator = vec::zero;
    for(ModelPolygon *polygon : connectedPolygons)
    {
        polygon->recalculatePlane();
        normalAggregator += polygon->normal();
    }
    normal = normalAggregator.Normalized();
}
