#pragma once

#include "Geometry/AABB.h"

#include <stack>
#include <vector>
#include <QVector>
#include <QtOpenGL>



using namespace std;

class ModelState
{
public:
    ModelState();
    explicit ModelState(const AABB& aabb);
    ~ModelState();

    AABB aabb;
};
