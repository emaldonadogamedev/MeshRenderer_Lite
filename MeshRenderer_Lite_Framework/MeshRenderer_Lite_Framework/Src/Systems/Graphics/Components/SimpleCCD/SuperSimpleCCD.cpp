#include <Utilities/precompiled.h>
#include <Systems/Graphics/Components/SimpleCCD/SuperSimpleCCD.h>

SuperSimpleCCD::SuperSimpleCCD(const GameObject* owner):IComponent( ComponentType::PHYSICS_IK_CCD, owner)
{

}

SuperSimpleCCD::~SuperSimpleCCD()
{

}

void SuperSimpleCCD::Update(const float dt)
{

}

float SuperSimpleCCD::RandFloat(const float minValue, const float maxValue) const
{
	return minValue + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (int)(maxValue - minValue)));
}
