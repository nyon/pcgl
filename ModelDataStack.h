#ifndef MODELDATASTACK_H
#define MODELDATASTACK_H

#include <vector>

using namespace std;

#include "ModelData.h"

class ModelDataStack
{
public:
    ModelDataStack();
    virtual ~ModelDataStack();

    ModelData* current();
    void push();
    void pushClean();
    ModelData* pop();

private:
    vector<ModelData*> modelData;
};

#endif // MODELDATASTACK_H
