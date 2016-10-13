#pragma once

#include <vector>
#include "ModelPolygon.h"
#include "Geometry/Plane.h"
#include "ModelBSPOperation.h"

using namespace std;

class ModelBSPNode
{
public:
    ModelBSPNode(ModelBSPOperation *operation, ModelData *data);
    ~ModelBSPNode();
    static ModelBSPNode *buildFromData(ModelBSPOperation *operation, ModelData *data);

    ModelBSPOperation *operation;
    ModelData *data;

    vector<ModelPolygon*> polygons;

    ModelBSPNode *front;
    ModelBSPNode *back;
    Plane plane;

    void flipPolygon(ModelPolygon* polygon);
    void flipPlane(Plane* plane);
    void splitPolygon(const Plane& plane,
                      ModelPolygon* polygon,
                      vector<ModelPolygon *> &coplanar_front,
                      vector<ModelPolygon *> &coplanar_back,
                      vector<ModelPolygon *> &front,
                      vector<ModelPolygon *> &back) const;

    void clipTo(const ModelBSPNode * other);
    void invert();
    void build(const vector<ModelPolygon*>& list, int depth = 0);
    vector<ModelPolygon*> clipPolygons(const vector<ModelPolygon*> & list) const;
    vector<ModelPolygon*> allPolygons() const;
};
