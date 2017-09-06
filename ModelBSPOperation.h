#pragma once

class ModelBSPNode;
class ModelData;
class ModelPolygon;
class ModelVertex;

#include <vector>
#include <map>
#include <tuple>

using namespace std;

#define PRECISION 0.0001

class ModelBSPOperation
{
public:
    ModelBSPOperation(ModelData *current, ModelData *left, ModelData *right);
    virtual ~ModelBSPOperation();

    void combine();

    ModelData *current;
    ModelBSPNode *leftRoot;
    ModelBSPNode *rightRoot;


    vector<ModelPolygon*> newPolygons;
    vector<ModelVertex*> newVertices;
    vector<ModelPolygon*> deletePolygons;
    vector<ModelVertex*> deleteVertices;

    map<tuple<int, int, int>, ModelVertex*> vertexFix;
};

