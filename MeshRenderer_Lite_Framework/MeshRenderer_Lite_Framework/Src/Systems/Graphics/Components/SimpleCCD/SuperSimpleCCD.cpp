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

void SuperSimpleCCD::SetNewTargetPos()
{
	m_targetPos = aiVector3D(RandFloat(-100.0f, 100.0f), 0, RandFloat(-100.0f, 100.0f));
}

void SuperSimpleCCD::Update(const float dt)
{
	if (m_runCCD)
	{
		if (m_runCCDSingleStep)
		{
			RunCCDSingleStep();
		}
		else
		{
			RunCCD_StateMachine();
		}
	}
}

void SuperSimpleCCD::RunCCDSingleStep()
{
	float currentDistance = LengthSquaredBetween2Points(m_endeEffectorPos, m_targetPos);
	while (currentDistance <= (m_closeEnoughDistance * m_closeEnoughDistance))
	{
		auto currentJoint = m_endeEffectorNode;
		while (currentJoint->mParent)
		{
			//aiVector3D Vck = currentJoint;

			//go to the next joint
			currentJoint = currentJoint->mParent;
		}
	}
}

void SuperSimpleCCD::RunCCD_StateMachine()
{
	switch (m_ccdState)
	{
	case CCD_STATES::IDLE:
		m_ccdState = CCD_STATES::CALCULATE_DISTANCE;
		break;
	case CCD_STATES::CALCULATE_DISTANCE:
		break;
	case CCD_STATES::PREPARE_ROTATION:
		break;
	case CCD_STATES::UPDATE_HEIRARCHY:
		break;
	default:
		break;
	}
}

void SuperSimpleCCD::FindEndEffector()
{
	/*
	Note: For now, in order for this to work a ModelComponent is required. So it has to be added to the game object first.
	*/
	const auto modelComp  = static_cast<const ModelComponent* const>(m_owner->GetComponent(ComponentType::RENDERABLE_3D));

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

const float SuperSimpleCCD::LengthBetween2Points(const aiVector3D& a, const aiVector3D& b)
{
	return sqrt(LengthSquaredBetween2Points(a, b));
}

const float SuperSimpleCCD::LengthSquaredBetween2Points(const aiVector3D& a, const aiVector3D& b)
{
	const float dx = a.x - b.x;
	const float dy = a.y - b.y;
	const float dz = a.z - b.z;

	return (dx*dx) + (dy*dy) + (dz*dz);
}
