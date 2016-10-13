#pragma once

#include <vector>
#include "ModelState.h"

using namespace std;

class ModelStates
{
public:
	ModelStates();
	~ModelStates();

	void push();
	void pushIdentity();
    void load(const ModelState& state);
	void pop();
	ModelState *current();

private:
    vector<ModelState*> states;
};

