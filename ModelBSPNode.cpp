#include "ModelBSPNode.h"
#include "ModelData.h"
#include <algorithm>

ModelBSPNode::ModelBSPNode(ModelBSPOperation *operation, ModelData *data) : operation(operation), data(data), front(0), back(0)
{
    this->plane.normal = vec::zero;
}

ModelBSPNode* ModelBSPNode::buildFromData(ModelBSPOperation *operation, ModelData *data)
{
    ModelBSPNode *node = new ModelBSPNode(operation, data);
    node->build(data->polygons());
    return node;
}

ModelBSPNode::~ModelBSPNode()
{
    if(front) delete front;
    if(back) delete back;
}

void ModelBSPNode::clipTo(const ModelBSPNode * other)
{
    this->polygons = other->clipPolygons(this->polygons);
    if(this->front) this->front->clipTo(other);
    if(this->back) this->back->clipTo(other);
}

void ModelBSPNode::invert()
{
    for(size_t i = 0; i < this->polygons.size(); i++)
    {
        flipPolygon(this->polygons[i]);
    }
    flipPlane(&this->plane);
    if(this->front) this->front->invert();
    if(this->back) this->back->invert();
    swap(this->front, this->back);
}

void ModelBSPNode::flipPolygon(ModelPolygon* polygon)
{
    // Reverse Vertex order
    polygon->reverseOrder();

    flipPlane(&polygon->plane);
}

void ModelBSPNode::flipPlane(Plane* plane)
{
    plane->normal *= -1.0f;
    plane->d *= -1.0f;
}

void ModelBSPNode::build(const vector<ModelPolygon*>& list, int depth)
{
    if(!list.size()) return;
    if(this->plane.IsDegenerate()) this->plane = list[0]->plane;
    vector<ModelPolygon*> list_front, list_back;

    for(size_t i = 0; i < list.size(); i++)
    {
        splitPolygon(this->plane, list[i], this->polygons, this->polygons, list_front, list_back);
    }


    if(list_front.size())
    {
        if(!this->front) this->front = new ModelBSPNode(operation, data);
        this->front->build(list_front, depth+1);
    }

    if(list_back.size())
    {
        if(!this->back) this->back = new ModelBSPNode(operation, data);
        this->back->build(list_back, depth+1);
    }

}

vector<ModelPolygon*> ModelBSPNode::allPolygons() const
{
    vector<ModelPolygon*> list = this->polygons;
    vector<ModelPolygon*> list_front, list_back;
    if(this->front) list_front = this->front->allPolygons();
    if(this->back) list_back = this->back->allPolygons();
    list.insert(list.end(), list_front.begin(), list_front.end());
    list.insert(list.end(), list_back.begin(), list_back.end());
    return list;
}

vector<ModelPolygon*> ModelBSPNode::clipPolygons(const vector<ModelPolygon*> & list) const
{
    if(this->plane.IsDegenerate()) return list;

    vector<ModelPolygon*> list_front;
    vector<ModelPolygon*> list_back;
    for(size_t i = 0; i < list.size(); i++)
    {
        this->splitPolygon(this->plane, list[i], list_front, list_back, list_front, list_back);
    }
    if(this->front) list_front = this->front->clipPolygons(list_front);
    if(this->back) list_back = this->back->clipPolygons(list_back);
    else
    {
        for(ModelPolygon *p : list_back)
        {
            operation->deletePolygons.push_back(p);
        }
        list_back.clear();
    }

    list_front.insert(list_front.end(), list_back.begin(), list_back.end());
    return list_front;
}

void ModelBSPNode::splitPolygon(const Plane& plane,
                             ModelPolygon *polygon,
                             vector<ModelPolygon*>& coplanar_front,
                             vector<ModelPolygon*>& coplanar_back,
                             vector<ModelPolygon*>& front,
                             vector<ModelPolygon*> & back) const
{
    enum
    {
        COPLANAR = 0, // 0b00
        FRONT = 1,    // 0b01
        BACK = 2,     // 0b10
        SPANNING = 3  // 0b11
    };

    // Classify each point as well as the entire polygon into one of the above
    // four classes.
    int polygon_type = 0;
    vector<int> types;

    polygon->eachVertex([&plane,&polygon_type,&types](ModelVertex* v){
        double t = plane.SignedDistance(v->pos);
        int type = (t < -0.0001) ? BACK : ((t > 0.0001) ? FRONT : COPLANAR);
        polygon_type |= type;
        types.push_back(type);
    });

    // Put the polygon in the correct list, splitting it when necessary.
    switch (polygon_type)
    {
    case COPLANAR:
        {
            if(plane.normal.Dot(polygon->plane.normal) > 0)
                coplanar_front.push_back(polygon);
            else
                coplanar_back.push_back(polygon);
            break;
        }
    case FRONT:
        {
            front.push_back(polygon);
            break;
        }
    case BACK:
        {
            back.push_back(polygon);
            break;
        }
    case SPANNING:
        {
            vector<ModelVertex*> f;
            vector<ModelVertex*> b;
            size_t vertexCount = polygon->vertexCount();
            for(size_t i = 0; i < vertexCount; i++)
            {
                int j = (i + 1) % vertexCount;
                int ti = types[i], tj = types[j];
                ModelVertex *vi = polygon->vertex(i);
                ModelVertex *vj = polygon->vertex(j);
                if(ti != BACK) f.push_back(vi);
                if(ti != FRONT) b.push_back(vi);
                if((ti | tj) == SPANNING)
                {
                    double t = (plane.d - plane.normal.Dot(vi->pos)) / plane.normal.Dot(vj->pos - vi->pos);
                    vec v = vi->pos.Lerp(vj->pos, t);

                    int ix = int(v.x / PRECISION);
                    int iy = int(v.y / PRECISION);
                    int iz = int(v.z / PRECISION);
                    auto key = make_tuple(ix,iy,iz);
                    auto index = operation->vertexFix.find(key);
                    ModelVertex *vertex;
                    if(index != operation->vertexFix.end())
                    {
                        vertex = index->second;
                    }
                    else
                    {
                        vertex = ModelVertex::fromVector(v);
                        operation->vertexFix.emplace(key, vertex);
                        operation->newVertices.push_back(vertex);
                    }

                    f.push_back(vertex);
                    b.push_back(vertex);
                }
            }
            if(f.size() >= 3)
            {
                ModelPolygon *newPolygon = ModelPolygon::fromPolygon(f);
                vector<ModelPolygon*> tris = newPolygon->toTriangle();
                for(ModelPolygon *p : tris)
                {
                    p->recalculatePlane();
                }
                operation->newPolygons.insert(operation->newPolygons.end(), tris.begin(), tris.end());
                front.insert(front.end(), tris.begin(), tris.end());
                delete newPolygon;

            }
            if(b.size() >= 3)
            {
                ModelPolygon *newPolygon = ModelPolygon::fromPolygon(b);
                vector<ModelPolygon*> tris = newPolygon->toTriangle();
                for(ModelPolygon *p : tris)
                {
                    p->recalculatePlane();
                }
                operation->newPolygons.insert(operation->newPolygons.end(), tris.begin(), tris.end());
                back.insert(back.end(), tris.begin(), tris.end());
                delete newPolygon;
            }
            if(b.size() >= 3 || f.size() >= 3)
            {
                operation->deletePolygons.push_back(polygon);
            }
            break;
        }
    }
}
