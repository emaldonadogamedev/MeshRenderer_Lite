#pragma once
#include <string>
#include <vector>
#include <Systems/Graphics/GraphicsUtilities/VertexTypes.h>
#include <Systems/Graphics/GraphicsUtilities/ObjectHandle.h>
#include <assimp/Importer.hpp>

class DX11Renderer;
struct aiScene;
struct aiAnimation;

using DirectX::XMMATRIX;
using DirectX::XMVECTOR;

enum ModelType
{
	MODEL_STATIC,
	MODEL_SKINNED,
	ONLY_ANIMATION
};

struct BoneMatrixInfo
{
	BoneMatrixInfo()
	{
		offsetMatrix = finalTransformation = DirectX::XMMatrixIdentity();
	}
	std::string name;
	//Fun fact:
	//the job of the offset matrix it to move the vertex position from the local space of
	//the mesh into the bone space of that particular bone
	XMMATRIX offsetMatrix;
	XMMATRIX finalTransformation;
};

struct BoneNode;

typedef std::unique_ptr<BoneNode> BoneNodePtr;
typedef std::unordered_map<std::string, BoneNodePtr> BoneNodePtrUmap;
typedef std::vector<BoneNodePtr> BoneNodePtrVec;

struct BoneNode
{
	std::string name;
	XMMATRIX transformation;
	BoneNode* parent = nullptr;
	//BoneNodePtrUmap children;
	BoneNodePtrVec children;
};

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

	ModelType GetModelType(void) const;
	void SetModelType(ModelType type);

	const std::string& GetFileName() const;
	void GetFileName(const std::string& fileName);

	MeshEntryList m_meshEntryList;

	BoneNodePtr m_rootNode;
	std::unordered_map<std::string, aiAnimation*> m_animations;

protected:
	//Generic model info.
	ModelType m_modelType;
	std::string m_modelFileName;

	//VBuffer & IBuffer data
	std::vector<VertexAnimation> m_vertices;
	std::vector<unsigned int> m_indices;
	ObjectHandle m_vertexBufferHandle;
	ObjectHandle m_indexBufferHandle;

	//Bone-animation information
	std::vector<BoneMatrixInfo> m_boneMatrices;
	unsigned int m_numBones;
	std::unordered_map<std::string, unsigned int> m_boneMapping; // maps a bone name to its index

	//Assimp data
	const aiScene* m_assimpScene;
	Assimp::Importer m_modelImporter;

	//bone matrices for rendering
	static const unsigned char s_MaxJointCount = 200;
	static XMMATRIX boneMatrices[s_MaxJointCount];

	friend class PrimitiveGenerator;
	friend class ModelManager;
};