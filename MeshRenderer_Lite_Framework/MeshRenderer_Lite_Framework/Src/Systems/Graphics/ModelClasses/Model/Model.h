#pragma once
#include <string>
#include <vector>
#include <Systems/Graphics/GraphicsUtilities/VertexTypes.h>
#include <Systems/Graphics/GraphicsUtilities/ObjectHandle.h>
#include <assimp/Importer.hpp>

class GraphicsSystem;
class DX11Renderer;
class PrimitiveGenerator;

struct aiScene;
struct aiAnimation;

using DirectX::XMMATRIX;
using DirectX::XMVECTOR;
using Assimp::Importer;

enum class ModelType : char
{
	MODEL_STATIC,
	MODEL_SKINNED,
	ONLY_ANIMATION
};

//struct BoneMatrixInfo
//{
//	BoneMatrixInfo()
//	{
//		offsetMatrix = finalTransformation = DirectX::XMMatrixIdentity();
//	}
//	std::string name;
//	//Fun fact:
//	//the job of the offset matrix it to move the vertex position from the local space of
//	//the mesh into the bone space of that particular bone
//	XMMATRIX offsetMatrix;
//	XMMATRIX finalTransformation;
//};

struct BoneStructureNode;

typedef std::unique_ptr<BoneStructureNode> BoneNodePtr;
typedef std::unordered_map<std::string, BoneNodePtr> BoneNodePtrUmap;
typedef std::vector<BoneNodePtr> BoneNodePtrVec;

struct BoneStructureNode
{
	std::string name;
	XMMATRIX transformation;
	BoneStructureNode* parent = nullptr;
	//BoneNodePtrUmap children;
	BoneNodePtrVec children;
};

//Meant to indicate a specific part of the loaded scene
//Example: a humanoid model can have entries such as: HEAD, LEG1, LEG2, ARM1, ARM2, etc.
struct MeshEntry
{
	int materialIndex = 0;
	int numIndices = 0;
	int baseVertex = 0;
	int baseIndex = 0;
};

typedef std::vector<MeshEntry> MeshEntryList;

class Model
{
public:
	Model(void);
	~Model(void);

	void GenerateBuffers(DX11Renderer* const renderContext);
	const ObjectHandle GetVBufferHandle() const;
	const ObjectHandle GetIBufferHandle() const;
	const int GetVertexCount() const;
	const int GetIndicesCount()const;

	ModelType GetModelType(void) const;
	void SetModelType(ModelType type);

	bool IsAnimationActive() const;

	const std::string& GetFileName() const;
	void SetFileName(const std::string& fileName);

	MeshEntryList m_meshEntryList;

	BoneNodePtr m_rootNode;
	std::unordered_map<std::string, aiAnimation*> m_animations;

//protected:
	//Generic model info.
	ModelType m_modelType;
	std::string m_modelFileName;

	//VBuffer & IBuffer data
	std::vector<VertexAnimation> m_vertices;
	std::vector<unsigned int> m_indices;
	ObjectHandle m_vertexBufferHandle;
	ObjectHandle m_indexBufferHandle;

	//Bone-animation information
	float m_runningTime = 0.0f;
	std::string m_currentAnimName;
	bool m_animationEnabled = false;
	bool m_renderBones = false;
	XMMATRIX m_globalInverseTransform;
	std::vector<XMVECTOR> m_boneLocations;
	std::vector<XMMATRIX> m_boneOffsetMtxVec;
	std::vector<XMMATRIX> m_boneFinalTransformMtxVec;
	unsigned int m_numBones;
	std::unordered_map<std::string, unsigned int> m_boneMapping; // maps a bone name to its index

	//Assimp data
	const aiScene* m_assimpScene;
	Importer m_modelImporter;

	friend class PrimitiveGenerator;
	friend class ModelManager;
	friend class GraphicsSystem;
};