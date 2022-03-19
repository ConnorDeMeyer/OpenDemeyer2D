#pragma once
#include "../ComponentBase.h"
#include "../Delegate.h"

class CollisionComponent : public ComponentBase
{
public:

	MultiDelegate<> OnOverlap;

};
