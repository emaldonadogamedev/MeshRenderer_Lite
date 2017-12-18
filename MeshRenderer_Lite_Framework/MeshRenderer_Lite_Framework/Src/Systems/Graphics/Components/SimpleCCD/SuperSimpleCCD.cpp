#include <Utilities/precompiled.h>
#include <Systems/Graphics/Components/SimpleCCD/SuperSimpleCCD.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>

#include <Systems/Core/GameObject/GameObject.h>
#include <Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include <Systems/Graphics/Components/ModelComponent/ModelComponent.h>
#include <Systems/Graphics/GraphicsUtilities/VertexTypes.h>
#include <Systems/Graphics/ModelClasses/Model/Model.h>

SuperSimpleCCD::SuperSimpleCCD(const GameObject* owner):IComponent( ComponentType::PHYSICS_IK_CCD, owner)
{
	//FindEndEffector();
	aiVector3D tempPos(0,0,0);
	m_jointRotations.resize(s_MADE_UP_JOINT_AMOUNT, aiQuaternion());
	m_jointTransforms.resize(s_MADE_UP_JOINT_AMOUNT, aiMatrix4x4());
	m_zRotations.resize(s_MADE_UP_JOINT_AMOUNT, 0.0f);
	for (int i = 0; i < s_MADE_UP_JOINT_AMOUNT; ++i)
	{
		m_jointPositions.emplace_back(tempPos);
		aiMatrix4x4::Translation(tempPos, m_jointTransforms[i]);//update its transform
		tempPos.y += m_distBetweenJoints;
	}
	m_endEffectorPos = m_jointPositions[s_MADE_UP_JOINT_AMOUNT - 1];

	SetNewTargetPos();
	m_targetFound = false;
	m_runCCDSingleStep = true;
	m_closeEnoughDistance = 1.5f;
	m_jointRotationTime = 1.0f;
}

SuperSimpleCCD::~SuperSimpleCCD()
{

}

void SuperSimpleCCD::SetNewTargetPos()
{
	const float x = RandFloat(-s_TARGET_HALF_RANGE, s_TARGET_HALF_RANGE);
	const float y = RandFloat(-s_TARGET_HALF_RANGE, s_TARGET_HALF_RANGE);

	m_targetPos = aiVector3D(x, y, 0);
	m_runCCD = true;
}

void SuperSimpleCCD::generateVertexBuffers(DX11Renderer * renderContext)
{
	unsigned int indices[s_MADE_UP_JOINT_AMOUNT] = {
		0,1,2,3,4
	};
	renderContext->CreateIndexBuffer(m_drawPoints_IB, BufferUsage::USAGE_DEFAULT, sizeof(indices), indices);

	//std::vector<VertexAnimation> vertices(s_MADE_UP_JOINT_AMOUNT, VertexAnimation());
	//for (int i = 0; i < m_jointPositions.size(); ++i)
	//{
	//	vertices[i].position.x = m_jointPositions[i].x;
	//	vertices[i].position.y = m_jointPositions[i].y;
	//	vertices[i].position.z = m_jointPositions[i].z;
	//}
	//
	//renderContext->CreateVertexBuffer(m_drawPoints_VB, BufferUsage::USAGE_DEFAULT, sizeof(VertexAnimation) * vertices.size(), vertices.data());
}

void SuperSimpleCCD::Update(const float dt)
{
	if (m_runCCD)
	{
		if (m_runCCDSingleStep)
		{
			RunCCDSingleStep();
			m_runCCD = false;
		}
		else
		{
			RunCCD_StateMachine(dt);

		}
	}
}

void SuperSimpleCCD::FixNodeHeirarchy(const float factor)
{
	//auto currentJoint = m_endeEffectorNode;
	//
	////for each joint...
	//while (currentJoint->mParent)
	//{
	//	//rotate node
	//}

	aiMatrix4x4 parentTransform;
	aiMatrix4x4::RotationZ(m_zRotations[0], parentTransform);
	for (int i = 1; i < m_jointTransforms.size(); ++i)
	{
		//aiMatrix4x4 translation;
		//aiMatrix4x4::Translation(m_jointPositions[i], translation);
		//aiMatrix4x4 orientation = aiMatrix4x4(m_jointRotations[i].GetMatrix());
		m_jointPositions[i] *= parentTransform;// *orientation;
		aiMatrix4x4 orientation;
		aiMatrix4x4::RotationZ( factor *  m_zRotations[i], orientation);
		parentTransform *= orientation;
	}

	//for (int i = s_MADE_UP_JOINT_AMOUNT - 1; i > 0; --i)
	//{
	//	m_jointPositions[i].x = m_jointPositions[i - 1].x + (m_distBetweenJoints *  cos(factor * m_zRotations[i]));
	//	m_jointPositions[i].y = m_jointPositions[i - 1].y + (m_distBetweenJoints *  sin(factor * m_zRotations[i]));
	//	m_jointPositions[i].z = 0;
	//}
}

void SuperSimpleCCD::RunCCDSingleStep()
{
	m_endEffectorPos = m_jointPositions.back();
	float currentDistanceSquared = m_lastDistanceSquared = LengthSquaredBetween2Points(m_endEffectorPos, m_targetPos);
	while (currentDistanceSquared >= (m_closeEnoughDistance * m_closeEnoughDistance))
	{
		//for each joint starting from the EE's parent...
		for(int i = s_MADE_UP_JOINT_AMOUNT - 2; i >= 0; --i)
		{
			const aiVector3D currentJointPos = m_jointPositions[i];
			const aiVector3D jointToEE = (m_endEffectorPos - currentJointPos).NormalizeSafe(); //Vck
			const aiVector3D jointToTarget = (m_targetPos - currentJointPos).NormalizeSafe(); //Vdk
			
			const float dot = AiVec3_Dot(jointToEE, jointToTarget);
			const float angle = acos(dot);
			m_zRotations[i] = angle;

			const aiVector3D rotationVec = AiVec3_Cross(jointToEE, jointToTarget).NormalizeSafe();
			const aiQuaternion currentJointOrientation = aiQuaternion(rotationVec, angle);
			m_jointRotations[i] = currentJointOrientation;

			FixNodeHeirarchy(1.0f);

			m_endEffectorPos = m_jointPositions.back();
			currentDistanceSquared = LengthSquaredBetween2Points(m_endEffectorPos, m_targetPos);

			//If we have a distance like the last attempt
			if (fabs(currentDistanceSquared - m_lastDistanceSquared) <= 0.001f)
			{
				m_lastDistanceSquared = currentDistanceSquared;
				--m_lastDistanceChances;
				if (m_lastDistanceChances == 0)
				{
					m_lastDistanceChances = 5; //reset the chances for next time we run the algorithm
					m_runCCD = false; //Don't run algorithm until prompted to do so
					m_targetFound = false;
					return;
				}
			}
			else
				m_lastDistanceChances = 5; //reset the chances for next time we run the algorithm

			//If we're actually there...
			if (currentDistanceSquared <= (m_closeEnoughDistance * m_closeEnoughDistance))
			{
				//gooooooooooooooool!
				m_runCCD = false; //Don't run algorithm until prompted to do so
				m_targetFound = true;
				return;
			}
		}
	}
}

void SuperSimpleCCD::RunCCD_StateMachine(const float dt)
{
	switch (m_ccdState)
	{
	case CCD_STATES::IDLE:
		m_ccdState = CCD_STATES::CALCULATE_DISTANCE;
		m_jointIndex_SM = s_MADE_UP_JOINT_AMOUNT - 2;
		m_lastDistanceChances = 5;
		break;

	case CCD_STATES::CALCULATE_DISTANCE:
	{
		m_endEffectorPos = m_jointPositions.back();
		float currentDistanceSquared = m_lastDistanceSquared = LengthSquaredBetween2Points(m_endEffectorPos, m_targetPos);
		if (currentDistanceSquared <= (m_closeEnoughDistance * m_closeEnoughDistance))
		{
			//gooooooooooooooool!
			m_runCCD = false; //Don't run algorithm until prompted to do so
			m_targetFound = true;
			m_lastDistanceChances = 5;
			m_ccdState = CCD_STATES::IDLE;
			return;
		}
		m_ccdState = CCD_STATES::PREPARE_ROTATION;

		//for each joint starting from the EE's parent...
		m_jointIndex_SM = m_jointPositions.size() - 1;
		break;
	}

	case CCD_STATES::PREPARE_ROTATION:
	{
		const aiVector3D currentJointPos = m_jointPositions[m_jointIndex_SM];
		const aiVector3D jointToEE = (m_endEffectorPos - currentJointPos).NormalizeSafe(); //Vck
		const aiVector3D jointToTarget = (m_targetPos - currentJointPos).NormalizeSafe(); //Vdk

		const float dot = AiVec3_Dot(jointToEE, jointToTarget);
		const float angle = acos(dot);
		m_zRotations[m_jointIndex_SM] = angle;

		const aiVector3D rotationVec = AiVec3_Cross(jointToEE, jointToTarget).NormalizeSafe();
		const aiQuaternion currentJointOrientation = aiQuaternion(rotationVec, angle);
		m_jointRotations[m_jointIndex_SM] = currentJointOrientation;

		m_currJointRotationTime = 0.0f;

		m_ccdState = CCD_STATES::LERP_ROTATION;
		break;
	}

	case CCD_STATES::LERP_ROTATION:
		
		if (m_currJointRotationTime >= m_jointRotationTime)
		{
			m_currJointRotationTime = 0;
			--m_jointIndex_SM;
		}

		m_currJointRotationTime += dt;
		m_ccdState = CCD_STATES::UPDATE_HEIRARCHY;
		break;

	case CCD_STATES::UPDATE_HEIRARCHY:
	{
		FixNodeHeirarchy((m_currJointRotationTime / m_jointRotationTime));

		m_endEffectorPos = m_jointPositions.back();
		float currentDistanceSquared = LengthSquaredBetween2Points(m_endEffectorPos, m_targetPos);

		//If we have a distance like the last attempt
		if (fabs(currentDistanceSquared - m_lastDistanceSquared) <= 0.001f)
		{
			m_lastDistanceSquared = currentDistanceSquared;
			--m_lastDistanceChances;
			if (m_lastDistanceChances == 0)
			{
				m_lastDistanceChances = 5; //reset the chances for next time we run the algorithm
				m_runCCD = false; //Don't run algorithm until prompted to do so
				m_targetFound = false;
				return;
			}
		}
		else
			m_lastDistanceChances = 5; //reset the chances for next time we run the algorithm

		if (currentDistanceSquared <= (m_closeEnoughDistance * m_closeEnoughDistance))
		{
			//gooooooooooooooool!
			m_runCCD = false; //Don't run algorithm until prompted to do so
			m_targetFound = true;
			m_ccdState = CCD_STATES::IDLE;
			return;
		}

		m_ccdState = CCD_STATES::LERP_ROTATION;

		break;
	}

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
	modelComp->m_componentType;
	if (modelComp)
	{
		int jointRotationCount = 0;
		/*
		Note: The real reason as to why this is super simple, For this 1st implementation I always just go to the first child
		forever until number of children is 0
		*/
		m_assimpScene = modelComp->GetModel()->m_assimpScene;
		
		m_endeEffectorNode = m_assimpScene->mRootNode;

		while (m_endeEffectorNode->mNumChildren > 0)
		{
			m_endeEffectorNode = m_endeEffectorNode->mChildren[0];
			jointRotationCount++;
		}
		m_jointRotations.clear();
		m_jointPositions.clear();

		m_jointRotations.resize(jointRotationCount);
		m_jointPositions.resize(jointRotationCount);
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

float SuperSimpleCCD::AiVec3_Dot(const aiVector3D& a, const aiVector3D& b) const
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

aiVector3D SuperSimpleCCD::AiVec3_Cross(const aiVector3D& a, const aiVector3D& b) const
{
	aiVector3D result;

	result.x =   (a.y * b.z) - (b.y * a.z);
	result.y = -((a.x * b.z) - (b.x * a.z));
	result.z =   (a.x * b.y) - (b.x * a.y);

	return result;
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

const float SuperSimpleCCD::s_TARGET_HALF_RANGE = 7.f;
