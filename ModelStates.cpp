#include "ModelStates.h"

ModelStates::ModelStates()
{
    pushIdentity();
}

ModelStates::~ModelStates()
{
    while(states.size())
	{
        delete states.back();
        states.pop_back();
	}
}


void ModelStates::push()
{
    states.push_back(new ModelState(*current()));
}

void ModelStates::pushIdentity()
{
    states.push_back(new ModelState());
}

void ModelStates::pop()
{
    if(!states.size()) return;
    delete states.back();
    states.pop_back();
}


ModelState *ModelStates::current()
{
    if(!states.size()) return 0;
    return states.back();
}

void ModelStates::load(const ModelState& state)
{
    pop();
    states.push_back(new ModelState(state));
}
