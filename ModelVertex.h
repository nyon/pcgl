#ifndef MODELVERTEX_H
#define MODELVERTEX_H

#include "MathGeoLibFwd.h"
#include "ModelPolygon.h"
#include "Math/float4.h"

#include <vector>

class ModelPolygon;

using namespace std;

class ModelVertex
{
public:
    ModelVertex();
    explicit ModelVertex(const vec& pos);
    virtual ~ModelVertex();

    static ModelVertex* fromVector(const vec& pos);
    static ModelVertex* fromVector(float x, float y, float z);

    void announceDeath(ModelPolygon *polygon);
    void addDependantPolygon(ModelPolygon *polygon);
    void recalculateNormal();

    vec pos;
    float selection;
    vec normal;

    vector<ModelPolygon*> connectedPolygons;
};

#endif // MODELVERTEX_H
