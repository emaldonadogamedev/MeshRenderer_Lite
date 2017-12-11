#include <Utilities/precompiled.h>
#include <Systems/Graphics/Components/SimpleCCD/SuperSimpleCCD.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>

#include <Systems/Core/GameObject/GameObject.h>
#include <Systems/Graphics/Components/ModelComponent/ModelComponent.h>
#include <Systems/Graphics/ModelClasses/Model/Model.h>

SuperSimpleCCD::SuperSimpleCCD(const GameObject* owner):IComponent( ComponentType::PHYSICS_IK_CCD, owner)
{
	FindEndEffector();
}

SuperSimpleCCD::~SuperSimpleCCD()
{

}

void SuperSimpleCCD::Update(const float dt)
{

}

void SuperSimpleCCD::FindEndEffector()
{
	/*
	Note: For now, in order for this to work a ModelComponent is required. So it has to be added to the game object first.
	*/
	const auto modelComp = static_cast<ModelComponent*>(m_owner->GetComponent(ComponentType::RENDERABLE_3D));

	if (modelComp)
	{
		/*
		Note: The real reason as to why this is super simple, For this 1st implementation I always just go to the first child
		forever until number of children is 0
		*/
		m_assimpScene = modelComp->GetModel()->m_assimpScene;
		m_endeEffectorNode = m_assimpScene->mRootNode;
		while (m_endeEffectorNode->mNumChildren > 0)
		{
			m_endeEffectorNode = m_endeEffectorNode->mChildren[0];
		}
	}
}

float SuperSimpleCCD::Clamp(const float value, const float minValue, const float maxValue) const
{
	return min(max(minValue, value), maxValue);
}

float SuperSimpleCCD::RandFloat(const float minValue, const float maxValue) const
{
	return minValue + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (int)(maxValue - minValue)));
}
