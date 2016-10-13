#include "ModelBSPOperation.h"
#include "ModelBSPNode.h"
#include "ModelData.h"
#include <set>

ModelBSPOperation::ModelBSPOperation(ModelData *current, ModelData *left, ModelData *right) : current(current)
{
    // TODO: optimize this
    left->triangularize();
    right->triangularize();
    left->recalculatePlanes();
    right->recalculatePlanes();
    leftRoot = ModelBSPNode::buildFromData(this, left);
    rightRoot = ModelBSPNode::buildFromData(this, right);
}

ModelBSPOperation::~ModelBSPOperation()
{
    delete leftRoot;
    delete rightRoot;
}

void ModelBSPOperation::combine()
{
    leftRoot->clipTo(rightRoot);
    rightRoot->clipTo(leftRoot);
    rightRoot->invert();
    rightRoot->clipTo(leftRoot);
    rightRoot->invert();
    leftRoot->build(rightRoot->allPolygons());
    vector<ModelPolygon*> polygons = leftRoot->allPolygons();
    current->replacePolygons(polygons);

    set<ModelVertex*> verts;
    current->triangularize();
    for(ModelPolygon *p : current->polygonData)
    {
        for(int i = 0; i < p->vertexCount();i++)
        {
            verts.insert(p->vertex(i));
        }
    }
    current->vertexData.clear();
    current->vertexData.insert(current->vertexData.end(), verts.begin(), verts.end());
    for(ModelVertex *v : current->vertexData)
    {
        v->recalculateNormal();
    }
}

