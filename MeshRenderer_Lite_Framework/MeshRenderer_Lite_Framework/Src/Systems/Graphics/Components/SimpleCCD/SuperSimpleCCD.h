#pragma once

#include <Systems/Core/Components/IComponent.h>
#include <Systems/Graphics/GraphicsUtilities/ObjectHandle.h>
#include <assimp/vector3.h>
#include <assimp/quaternion.h>

struct aiScene;
struct aiNode;
struct aiBone;

//States for running the algorithm non-single step mode
enum class CCD_STATES
{
	IDLE,
	CALCULATE_DISTANCE,
	PREPARE_ROTATION,
	UPDATE_HEIRARCHY,
	COUNT
};

class SuperSimpleCCD : public IComponent
{
public:
	SuperSimpleCCD(const GameObject* owner);
	~SuperSimpleCCD();

	//This is fetched from the Model Component
	const aiScene* m_assimpScene = nullptr;
	const aiNode* m_endeEffectorNode = nullptr;

	bool m_runCCD = true;
	bool m_runCCDSingleStep = true;
	float m_closeEnoughDistance = 0.005f;
	void SetNewTargetPos();

private:
	void Update(const float dt);
	void RunCCDSingleStep();
	void RunCCD_StateMachine();
	void FixHeirarchy();

	void FindEndEffector();

	float Clamp(const float value, const float minValue = 0.0f, const float maxValue = 1.0f) const;
	float RandFloat(const float minValue = 0.f, const float maxValue = 1.0f) const;

	float AiVec3_Dot(const aiVector3D& a, const aiVector3D& b)const;
	aiVector3D AiVec3_Cross(const aiVector3D& a, const aiVector3D& b)const;

	aiVector3D m_targetPos;
	aiVector3D m_endeEffectorPos;

	std::vector<aiVector3D> m_jointPositions;
	std::vector<aiQuaternion> m_jointRotations;

	float m_walkSpeed = 1.0f;
	float m_jointRotationSpeed = 1.0f;

	CCD_STATES m_ccdState;

	static const float LengthBetween2Points(const aiVector3D& a, const aiVector3D& b);
	static const float LengthSquaredBetween2Points(const aiVector3D& a, const aiVector3D& b);

	friend class ImGuiStage;
	friend class GraphicsSystem;
};