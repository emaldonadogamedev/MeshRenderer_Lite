#pragma once

#include <Systems/Core/Components/IComponent.h>
#include <Systems/Graphics/GraphicsUtilities/ObjectHandle.h>
#include <assimp/vector3.h>
#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>

struct aiScene;
struct aiNode;
struct aiBone;
struct DX11Renderer;

//States for running the algorithm non-single step mode
enum class CCD_STATES
{
	IDLE,
	CALCULATE_DISTANCE,
	PREPARE_ROTATION,
	LERP_ROTATION,
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
	float m_closeEnoughDistance = 3.0f;
	float m_lastDistanceSquared = 0.f;
	int m_lastDistanceChances = 5;
	void SetNewTargetPos();

	//Initialization
	void generateVertexBuffers(DX11Renderer* renderContext);

	ObjectHandle m_drawBoxTargetVB;
	ObjectHandle m_drawBoxTargetIB;

private:
	void Update(const float dt);
	void RunCCDSingleStep();
	void RunCCD_StateMachine(const float dt);
	void FixNodeHeirarchy(const float factor);

	void FindEndEffector();

	float Clamp(const float value, const float minValue = 0.0f, const float maxValue = 1.0f) const;
	float RandFloat(const float minValue = 0.f, const float maxValue = 1.0f) const;

	float AiVec3_Dot(const aiVector3D& a, const aiVector3D& b)const;
	aiVector3D AiVec3_Cross(const aiVector3D& a, const aiVector3D& b)const;

	bool m_targetFound;

	aiVector3D m_targetPos;
	aiVector3D m_endEffectorPos;

	ObjectHandle m_drawPoints_IB;

	float m_distBetweenJoints = 2.0f;
	std::vector<aiVector3D> m_jointPositions;
	std::vector<float> m_zRotations;
	std::vector<aiQuaternion> m_jointRotations;
	std::vector<aiMatrix4x4> m_jointTransforms;

	float m_walkSpeed = 1.0f;
	float m_currJointRotationTime = 0.0f;
	float m_jointRotationTime = 1.0f;

	CCD_STATES m_ccdState;
	int m_jointIndex_SM;

	static const float LengthBetween2Points(const aiVector3D& a, const aiVector3D& b);
	static const float LengthSquaredBetween2Points(const aiVector3D& a, const aiVector3D& b);

	static const int s_MADE_UP_JOINT_AMOUNT = 5;
	static const float s_TARGET_HALF_RANGE;

	friend class ImGuiStage;
	friend class ForwardRenderStage;
	friend class ImGuiStage;
	friend class GraphicsSystem;
};