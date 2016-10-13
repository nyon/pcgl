#include "ModelDataStack.h"

ModelDataStack::ModelDataStack()
{
    modelData.push_back(new ModelData());
}

ModelDataStack::~ModelDataStack()
{
    while(modelData.size())
    {
        delete modelData.back();
        modelData.pop_back();
    }
}

ModelData* ModelDataStack::current()
{
    return modelData.back();
}

void ModelDataStack::push()
{
    modelData.push_back(new ModelData(*current()));
}

void ModelDataStack::pushClean()
{
    modelData.push_back(new ModelData());
}


ModelData *ModelDataStack::pop()
{
    if(modelData.size())
    {
        ModelData *data = modelData.back();
        modelData.pop_back();
        return data;
    }
    return 0;
}

