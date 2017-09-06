#include "Geometry/AABB.h"
#include "ModelData.h"
#include "Math/float4x4.h"
#include "PerlinNoise.h"


ModelData::ModelData()
{

}

ModelData::ModelData(const ModelData& other)
{
    polygonData.reserve(other.polygonData.size());
    for(ModelPolygon *otherPolygon : other.polygonData)
    {
        ModelPolygon *newPolygon = new ModelPolygon(*otherPolygon);
        polygonData.push_back(newPolygon);
    }
}

ModelData::~ModelData()
{
    for(ModelPolygon* polygon : polygonData)
    {
        //delete polygon; // TODO: fixme
    }
}

void ModelData::recalculatePlanes()
{
    for(ModelPolygon *p : polygonData)
    {
        p->recalculatePlane();
    }
}
void ModelData::applyNoiseField(function<float (float, float, float)> func, float displace)
{
    for(ModelVertex *vertex : vertexData)
    {
        vertex->recalculateNormal();

    }

    for(ModelVertex *vertex : vertexData)
    {
        vertex->pos += vertex->normal * func(vertex->pos.x, vertex->pos.y, vertex->pos.z) * displace;
    }
}


void ModelData::triangularize()
{

    vector<ModelPolygon*> newPolygons;
    for(ModelPolygon *p : polygonData)
    {
        for(size_t j = p->vertexCount()-1; j >= 3; j--)
        {
            ModelPolygon *newPolygon = ModelPolygon::fromTriangle(p->vertex(0), p->vertex(j-1), p->vertex(j));
            newPolygons.push_back(newPolygon);
        }
        p->forceTriangle();
    }
    for(ModelPolygon *p : newPolygons)
    {
        polygonData.push_back(p);
    }
}

const vector<ModelPolygon*>& ModelData::polygons()
{
    return polygonData;
}

void ModelData::addPolygon(ModelPolygon* polygon)
{
    polygonData.push_back(polygon);
}

void ModelData::addPolygons(const vector<ModelPolygon*>& list)
{
    addPolygons(list.begin(), list.end());
}

void ModelData::merge(ModelData *other)
{
    addPolygons(other->polygons());
    addVertices(other->vertices());
}



void ModelData::addPolygons(vector<ModelPolygon*>::const_iterator first, vector<ModelPolygon*>::const_iterator last)
{
    for(auto cur = first; cur != last; ++cur)
    {
        polygonData.push_back(*cur);
    }
}


const vector<ModelVertex*>& ModelData::vertices()
{
    return vertexData;
}

void ModelData::addVertex(ModelVertex* vertex)
{
    vertexData.push_back(vertex);
}

void ModelData::addVertices(const vector<ModelVertex*>& list)
{
    addVertices(list.begin(), list.end());
}

void ModelData::addVertices(vector<ModelVertex*>::const_iterator first, vector<ModelVertex*>::const_iterator last)
{
    for(auto cur = first; cur != last; ++cur)
    {
        vertexData.push_back(*cur);
    }
}

void ModelData::translateSelected(const vec& translation)
{
    for(ModelVertex *vertex : vertexData)
    {
        vertex->pos += translation * vertex->selection;
    }
}

void ModelData::recenterSelected()
{
    // First: Find common center of all selected vertices
    vec center = findSelectedCenter();

    // Second: Translate to -center, so that center is (0,0,0)
    translateSelected(-center);
}

vec ModelData::findSelectedCenter()
{
    vec center = vec::zero;
    float weight = 0.0f;
    for(ModelVertex *vertex : vertexData)
    {
        center += vertex->pos * vertex->selection;
        weight += vertex->selection;
    }
    if(weight == 0.0f) return vec::zero;

    center /= weight;
    return center;
}

void ModelData::scaleSelected(const vec& scale)
{
    // First: Find common center of all selected vertices
    vec center = findSelectedCenter();

    // Second: Translate to center, scale and translate back
    for(ModelVertex *vertex : vertexData)
    {
        const vec realFactor = scale;
        vertex->pos = ((vertex->pos - center).Mul(realFactor) + center) * vertex->selection + vertex->pos * (1.0f - vertex->selection);
    }
}

void ModelData::exportOBJ(QString filename)
{
    QFile file(filename);
    if(file.open(QIODevice::ReadWrite))
    {
        QTextStream stream(&file);
        unordered_map<ModelVertex*,int> indexMap;
        indexMap.reserve(vertexData.size());
        int i = 0;
        for(ModelVertex *vertex : vertexData)
        {
            i++;
            indexMap.emplace(vertex, i);
            stream << "v " << vertex->pos.x << " " << vertex->pos.y << " " <<  vertex->pos.z << endl;
        }

        for(ModelPolygon *polygon : polygonData)
        {
            stream << "f";
            for(size_t i = 0; i < polygon->vertexCount(); i++)
            {
                ModelVertex *vertex = polygon->vertex(i);
                auto vertexIterator = indexMap.find(vertex);
                if(vertexIterator != indexMap.end())
                {
                    stream << " " << vertexIterator->second;
                }
            }
            stream << endl;
        }
        file.close();
    }
}

void ModelData::selectXGradient(const vec& center, float w)
{
    const float smallestAxis = w / 2.0f;
    for(ModelVertex *vertex : vertexData)
    {
        if(abs(vertex->pos.x - center.x) < smallestAxis)
        {
            vertex->selection = 1.0f - min(1.0f, abs(vertex->pos.x - center.x) / smallestAxis);
            vertex->selection = 10 * vertex->selection * vertex->selection * vertex->selection - 15 * vertex->selection * vertex->selection * vertex->selection * vertex->selection + 6 * vertex->selection * vertex->selection * vertex->selection * vertex->selection * vertex->selection;
        }
        else
        {
            vertex->selection = 0.0f;
        }
    }
}

void ModelData::selectYGradient(const vec& center, float w)
{
    const float smallestAxis = w / 2.0f;
    for(ModelVertex *vertex : vertexData)
    {
        if(abs(vertex->pos.y - center.y) < smallestAxis)
        {
            vertex->selection = 1.0f - min(1.0f, abs(vertex->pos.y - center.y) / smallestAxis);
            vertex->selection = 10 * vertex->selection * vertex->selection * vertex->selection - 15 * vertex->selection * vertex->selection * vertex->selection * vertex->selection + 6 * vertex->selection * vertex->selection * vertex->selection * vertex->selection * vertex->selection;
        }
        else
        {
            vertex->selection = 0.0f;
        }
    }
}

void ModelData::selectZGradient(const vec& center, float w)
{
    const float smallestAxis = w / 2.0f;
    for(ModelVertex *vertex : vertexData)
    {
        if(abs(vertex->pos.z - center.z) < smallestAxis)
        {
            vertex->selection = 1.0f - min(1.0f, abs(vertex->pos.z - center.z) / smallestAxis);
            vertex->selection = 10 * vertex->selection * vertex->selection * vertex->selection - 15 * vertex->selection * vertex->selection * vertex->selection * vertex->selection + 6 * vertex->selection * vertex->selection * vertex->selection * vertex->selection * vertex->selection;
        }
        else
        {
            vertex->selection = 0.0f;
        }
    }
}

void ModelData::invertSelection()
{
    for(ModelVertex *vertex : vertexData)
    {
        vertex->selection = 1.0f - vertex->selection;
    }
}

void ModelData::selectSphere(const vec& center, float radius, float randomness, float detail)
{
    const float radiusSq = radius * radius;
    // Optimize, if no randomness is given
    if(randomness < 0.0001f)
    {
        for(ModelVertex *vertex : vertexData)
        {
            const float dist = min(1.0f, vertex->pos.DistanceSq(center) / radiusSq);
            vertex->selection = 1.0f - dist;
        }
    }
    else
    {
        float dist = 0.0f;
        for(ModelVertex *vertex : vertexData)
        {
            float radiusTmp = radius * (1.0f - pnoise3(vertex->pos.x * detail, vertex->pos.y  * detail, vertex->pos.z * detail) * randomness);
            float radiusSqTmp = radiusTmp * radiusTmp;
            dist = min(1.0f,vertex->pos.DistanceSq(center) / radiusSqTmp);
            vertex->selection = 1.0f - dist;
        }
    }

}

void ModelData::randomizeSelection(float amount, float detail)
{
    for(ModelVertex *vertex : vertexData)
    {
        const float selRandomness = amount * noise3(vertex->pos.x * detail, vertex->pos.y * detail, vertex->pos.z * detail);
        vertex->selection *= 1.0f - selRandomness;
    }
}

void ModelData::selectAll()
{
    for(ModelVertex *vertex : vertexData)
    {
        vertex->selection = 1.0f;
    }
}

void ModelData::selectNone()
{
    for(ModelVertex *vertex : vertexData)
    {
        vertex->selection = 0.0f;
    }
}

void ModelData::rotateXSelected(float rotX)
{
    applyMatrix(float4x4::RotateX(rotX));


}

void ModelData::applyMatrix(const float4x4& matrix)
{
    // First: Find common center of all selected vertices
    vec center = vec::zero;
    float weight = 0.0f;
    for(ModelVertex *vertex : vertexData)
    {
        center += vertex->pos * vertex->selection;
        weight += vertex->selection;
    }

    center /= weight;

    for(ModelVertex *vertex : vertexData)
    {
        vertex->pos = (matrix.TransformPos(vertex->pos - center) + center) * vertex->selection + vertex->pos * (1.0f - vertex->selection);
    }
}

void ModelData::rotateYSelected(float rotY)
{
    applyMatrix(float4x4::RotateY(rotY));
}

void ModelData::rotateZSelected(float rotZ)
{
    applyMatrix(float4x4::RotateZ(rotZ));
}

void ModelData::translateSelectionAlongNormals(float amount)
{
    for(ModelVertex *vertex : vertexData)
    {
        if(vertex->selection > 0.0001f) vertex->recalculateNormal();
        vertex->pos += vertex->normal * amount * vertex->selection;
    }
}

void ModelData::filterSelectionByNormalAngle(const vec& base_normal, float angle)
{
    for(ModelVertex *vertex : vertexData)
    {
        if(vertex->normal.AngleBetweenNorm(base_normal) > angle)
        {
            vertex->selection = 0.0f;
        }
    }
}

QVector<GLfloat> ModelData::generateModel()
{
    float x1, x2, x3;
    float y1, y2, y3;
    float z1, z2, z3;
    
    triangularize();
    QVector<GLfloat> data;
    data.reserve(polygonData.size() * 18);
    for(ModelVertex *vertex : vertexData)
    {
        vertex->normal = vec::zero;
    }
    for(ModelPolygon *polygon : polygonData)
    {

        x1 = polygon->vertex(0)->pos.x;
        x2 = polygon->vertex(1)->pos.x;
        x3 = polygon->vertex(2)->pos.x;
        y1 = polygon->vertex(0)->pos.y;
        y2 = polygon->vertex(1)->pos.y;
        y3 = polygon->vertex(2)->pos.y;
        z1 = polygon->vertex(0)->pos.z;
        z2 = polygon->vertex(1)->pos.z;
        z3 = polygon->vertex(2)->pos.z;

        vec v1(x2 - x1, y2 - y1, z2 - z1);
        vec v2(x3 - x1, y3 - y1, z3 - z1);
        vec n = v1.Cross(v2).Normalized();

        polygon->vertex(0)->normal += n;
        polygon->vertex(1)->normal += n;
        polygon->vertex(2)->normal += n;
    }

    for(ModelVertex *vertex : vertexData)
    {
        vertex->normal.Normalize();
    }

    for(ModelPolygon *polygon : polygonData)
    {
        float nx1, nx2, nx3;
        float ny1, ny2, ny3;
        float nz1, nz2, nz3;
        x1 = polygon->vertex(0)->pos.x;
        x2 = polygon->vertex(1)->pos.x;
        x3 = polygon->vertex(2)->pos.x;
        y1 = polygon->vertex(0)->pos.y;
        y2 = polygon->vertex(1)->pos.y;
        y3 = polygon->vertex(2)->pos.y;
        z1 = polygon->vertex(0)->pos.z;
        z2 = polygon->vertex(1)->pos.z;
        z3 = polygon->vertex(2)->pos.z;

        nx1 = polygon->vertex(0)->normal.x;
        nx2 = polygon->vertex(1)->normal.x;
        nx3 = polygon->vertex(2)->normal.x;
        ny1 = polygon->vertex(0)->normal.y;
        ny2 = polygon->vertex(1)->normal.y;
        ny3 = polygon->vertex(2)->normal.y;
        nz1 = polygon->vertex(0)->normal.z;
        nz2 = polygon->vertex(1)->normal.z;
        nz3 = polygon->vertex(2)->normal.z;

        data.push_back(x1);
        data.push_back(y1);
        data.push_back(z1);
        data.push_back(nx1);
        data.push_back(ny1);
        data.push_back(nz1);

        data.push_back(x2);
        data.push_back(y2);
        data.push_back(z2);
        data.push_back(nx2);
        data.push_back(ny2);
        data.push_back(nz2);

        data.push_back(x3);
        data.push_back(y3);
        data.push_back(z3);
        data.push_back(nx3);
        data.push_back(ny3);
        data.push_back(nz3);

    }

    printf("%zu polys\n", polygonData.size());
    return data;
}

void ModelData::replacePolygons(vector<ModelPolygon*>& replaceData)
{
    polygonData.clear();
    for(ModelPolygon *polygon : replaceData)
    {
        polygonData.push_back(polygon);
    }
}
