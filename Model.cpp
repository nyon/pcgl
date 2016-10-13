#include "Model.h"
#include <qmath.h>
#include <QObject>

using namespace std;

Model::Model(const AABB& startAABB) : vertexCount(0), ready(true), aabb(startAABB)
{
    states.load(ModelState(startAABB));
}

Model::~Model()
{
    if(vbo.isCreated()) vbo.destroy();
}

const AABB& Model::currentAABB()
{
    return states.current()->aabb;
}



ModelData* Model::data()
{
    return dataStack.current();
}

void Model::pushModelState()
{
    dataStack.push();
}

void Model::pushCleanModelState()
{
    dataStack.pushClean();
}

ModelData* Model::popModelState()
{
    return dataStack.pop();
}

void Model::popAndMergeModelState()
{
    ModelData *data = dataStack.pop();
    dataStack.current()->merge(data);
    delete data;
}

void Model::addPolygons(const vector<ModelPolygon *>& polygons)
{
    this->data()->addPolygons(polygons);
}

vec Model::randomPoint()
{
    // Select random point in model bounding box
    LCG lcg;
    return states.current()->aabb.RandomPointInside(lcg);
}

SurfacePoint Model::randomVertex()
{
    LCG lcg;
    ModelData *modelData = data();
    size_t vertexCount = modelData->vertexData.size();
    if(!vertexCount) return SurfacePoint(vec::zero, vec::unitZ);
    ModelVertex *v = modelData->vertexData.at(lcg.Int() % vertexCount);
    return SurfacePoint(v->pos, v->normal);
}

SurfacePoint Model::randomSurfacePoint()
{
    // Select random point in model bounding box
    LCG lcg;
    ModelData *modelData = data();
    size_t polygonCount = modelData->polygonData.size();
    if(!polygonCount) return SurfacePoint(vec::zero, vec::unitZ);
    ModelPolygon *nearestPolygon = modelData->polygonData.at(lcg.Int() % polygonCount);

    // Triangulate Polygon and pick point on triangle
    vector<ModelPolygon*> triangles = nearestPolygon->toTriangle();
    ModelPolygon *randomTriangle = triangles.at(lcg.Int() % triangles.size()); // TODO: reproducible randomness required here
    ModelVertex* a = randomTriangle->vertex(0);
    ModelVertex* b = randomTriangle->vertex(1);
    ModelVertex* c = randomTriangle->vertex(2);

    float u = lcg.FloatIncl(0.0f, 1.0f);
    float v = lcg.FloatIncl(0.0f, 1.0f);
    if (u + v >= 1.0f)
    {
        u = 1.0f - u;
        v = 1.0f - v;
    }

    // In case the triangle is far away from the origin but is small in size, the elements of 'a' will have large magnitudes,
    // and the elements of (b-a) and (c-a) will be much smaller quantities. Therefore be extra careful with the
    // parentheses and first sum the small floats together before adding it to the large one.
    // Source: MathGeoLib by Jukka Jylänki
    SurfacePoint randomPosition = SurfacePoint(a->pos + ((b->pos - a->pos) * u + (c->pos - a->pos) * v),
                                               a->normal + ((b->normal - a->normal) * u + (c->normal - a->normal) * v));

    for(ModelPolygon *polygon : triangles)
    {
        delete polygon;
    }
    return randomPosition;
}

void Model::regenerate()
{
    ready = true;
}


void Model::render()
{
    if(ready)
    {
        qDebug() << "RENDER " << data();
        QVector<GLfloat> modelData = data()->generateModel();
        vertexCount = modelData.count();

        vao.create();
        QOpenGLVertexArrayObject::Binder vaoBinder(&vao);

        vbo.create();
        vbo.bind();
        vbo.allocate(modelData.constData(), modelData.count() * sizeof(GLfloat));


        QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
        f->glEnableVertexAttribArray(0);
        f->glEnableVertexAttribArray(1);
        f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
        f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
        vbo.release();

        ready = false;
    }


    if(vertexCount)
    {
        QOpenGLVertexArrayObject::Binder vaoBinder(&vao);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }
}

bool Model::isRenderable()
{
    return vertexCount;
}


