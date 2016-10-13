#include "ModelPolygon.h"

ModelPolygon::ModelPolygon()
{

}

ModelPolygon::ModelPolygon(const ModelPolygon& other)
{
    vertices.reserve(other.vertices.size());
    for(ModelVertex *v : other.vertices)
    {
        addVertex(v);
    }
}

ModelPolygon::~ModelPolygon()
{
    for(ModelVertex *vertex : vertices)
    {
        vertex->announceDeath(this);
    }
}

void ModelPolygon::replaceVertex(ModelVertex *toBeReplaced, ModelVertex *with)
{
    auto index = std::find(vertices.begin(), vertices.end(), toBeReplaced);
    if(index != vertices.end())
    {
        vertices.erase(index);
        vertices.insert(index, with);

    }
}

void ModelPolygon::announceDeath(ModelVertex* vertex)
{
    auto index = std::find(vertices.begin(), vertices.end(), vertex);
    if(index != vertices.end())
    {
        vertices.erase(index);
    }
}

ModelPolygon* ModelPolygon::fromTriangle(ModelVertex *p1, ModelVertex *p2, ModelVertex *p3)
{
    ModelPolygon *poly = new ModelPolygon();
    poly->addVertex(p1);
    poly->addVertex(p2);
    poly->addVertex(p3);
    poly->plane.Set(p1->pos, p2->pos, p3->pos);
    return poly;
}


ModelPolygon* ModelPolygon::fromPolygon(vector<ModelVertex*> p)
{
    ModelPolygon *poly = new ModelPolygon();
    for(ModelVertex *v : p)
    {
        poly->addVertex(v);
    }
    // Workaround. Plane will be fixed when you call toTriangle
    poly->plane.Set(p.at(0)->pos, p.at(1)->pos, p.at(2)->pos);
    return poly;
}

size_t ModelPolygon::vertexCount()
{
    return vertices.size();
}

void ModelPolygon::eachVertex(function<void(ModelVertex*)> fn)
{
    for(ModelVertex* v : vertices)
    {
        fn(v);
    }
}

void ModelPolygon::recalculatePlane()
{
    // Workaround. Plane will be fixed when you call toTriangle
    plane.Set(vertices.at(0)->pos, vertices.at(1)->pos, vertices.at(2)->pos);
}

void ModelPolygon::addVertex(ModelVertex* v)
{
    vertices.push_back(v);
    v->addDependantPolygon(this);
}

void ModelPolygon::forceTriangle()
{
    while(vertices.size() > 3)
    {
        ModelVertex *vertex = vertices.back();
        vertex->announceDeath(this);
        vertices.pop_back();
    }
}

ModelVertex* ModelPolygon::vertex(int n)
{
    return vertices.at(n);
}

vector<ModelPolygon*> ModelPolygon::toTriangle()
{
    vector<ModelPolygon*> newPolygons;
    for(size_t j = vertices.size()-1; j >= 2; j--)
    {
        ModelPolygon *newPolygon = new ModelPolygon();
        newPolygon->addVertex(vertex(0));
        newPolygon->addVertex(vertex(j-1));
        newPolygon->addVertex(vertex(j));
        newPolygons.push_back(newPolygon);
    }
    return newPolygons;
}

void ModelPolygon::reverseOrder()
{
    reverse(vertices.begin(), vertices.end());
}
