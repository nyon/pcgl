#pragma once

#include <vector>
#include <unordered_map>
#include <QVector>

#include "ModelPolygon.h"
#include "ModelBSPNode.h"
#include "ValueNoise.h"

#include <QtOpenGL>

using namespace std;

class ModelData
{
public:
    ModelData();
    ModelData(const ModelData& other);
    virtual ~ModelData();

    void triangularize();

    // Modification
    void addPolygon(ModelPolygon* polygon);
    void addPolygons(const vector<ModelPolygon*>& list);
    void addPolygons(vector<ModelPolygon*>::const_iterator first, vector<ModelPolygon*>::const_iterator last);
    void addVertex(ModelVertex* polygon);
    void addVertices(const vector<ModelVertex*>& list);
    void addVertices(vector<ModelVertex*>::const_iterator first, vector<ModelVertex*>::const_iterator last);
    void applyNoiseField(function<float (float, float, float)> func, float displace);



    // Selection
    void translateSelected(const vec& translation);
    void translateSelectionAlongNormals(float amount);
    void recenterSelected();
    vec  findSelectedCenter();
    void filterSelectionByNormalAngle(const vec& base_normal, float angle);
    void scaleSelected(const vec& scale);
    void rotateXSelected(float rotX);
    void rotateYSelected(float rotY);
    void rotateZSelected(float rotZ);
    void selectSphere(const vec& center, float radius, float randomness = 0.0f, float detail = 10.0f);
    void selectXGradient(const vec& center, float w);
    void selectYGradient(const vec& center, float w);
    void selectZGradient(const vec& center, float w);
    void invertSelection();

    void selectAll();
    void selectNone();
    void randomizeSelection(float amount = 0.1f, float detail = 10.00f);



    // Generation
    QVector<GLfloat> generateModel();
    void exportOBJ(QString filename);

    const vector<ModelPolygon*>& polygons();
    const vector<ModelVertex*>& vertices();

    void merge(ModelData *other);
    void replacePolygons(vector<ModelPolygon*>& replaceData);

    void recalculatePlanes();

//private: // TODO: fixme
    void applyMatrix(const float4x4& matrix);

    vector<ModelPolygon*> polygonData;
    vector<ModelVertex*> vertexData;
};

