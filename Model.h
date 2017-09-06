#pragma once

#include <QString>

#include "Geometry/Polygon.h"
#include "Geometry/Plane.h"
#include "Geometry/AABB.h"
#include "BaseModel.h"
#include "ModelDataStack.h"

#include <QtOpenGL>
#include <QVector>
#include <QVector3D>
#include <vector>
#include <algorithm>


#include "SurfacePoint.h"


using namespace std;

class BaseModel;

class Model
{
public:
    explicit Model(const AABB& startAABB);
    virtual ~Model();

    void render();


    const AABB& currentAABB();

    ModelData* data();

    void regenerate();

    bool isRenderable();

    void pushModelState();
    void pushCleanModelState();
    ModelData* popModelState();
    void popAndMergeModelState();
    void addPolygons(const vector<ModelPolygon *>& polygons);


    SurfacePoint randomSurfacePoint();
    SurfacePoint randomVertex();
    vec randomPoint();

private:
    ModelDataStack dataStack;
    ModelStates states;

    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo;

    int vertexCount;
    bool ready;
    AABB aabb;
};


