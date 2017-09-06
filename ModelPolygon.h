#pragma once

#include "Geometry/Plane.h"
#include <functional>
#include <vector>
#include <algorithm>

#include "ModelVertex.h"

class ModelVertex;


using namespace std;

class ModelPolygon
{
public:
    ModelPolygon();
    ModelPolygon(const ModelPolygon& other);

    virtual ~ModelPolygon();
    void recalculatePlane();
    void announceDeath(ModelVertex* vertex);
    static ModelPolygon* fromTriangle(ModelVertex *p1, ModelVertex *p2, ModelVertex *p3);
    static ModelPolygon* fromPolygon(vector<ModelVertex *> p);
    size_t vertexCount();

    void replaceVertex(ModelVertex *toBeReplaced, ModelVertex *with);

    void addVertex(ModelVertex* v);
    void eachVertex(function<void(ModelVertex*)> fn);
    void forceTriangle();
    void reverseOrder();
    inline vec normal()
    {
        return plane.normal;
    }

    vector<ModelPolygon*> toTriangle();
    ModelVertex* vertex(int n);

    Plane plane;
    vector<ModelVertex*> vertices;

};
