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

struct VertexWeight
{
	VertexWeight(unsigned int vID, float w):vertexID(vID), weight(w) {}

	unsigned int vertexID;
	float weight;
};

struct Bone
{
	std::string name;
	std::vector<VertexWeight> weights;

	//Fun fact:
	//the job of the offset matrix it to move the vertex position from the local space of
	//the mesh into the bone space of that particular bone
	DirectX::XMMATRIX offsetMatrix;
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

struct ModelData
{
	std::vector<VertexAnimation> m_vertices;
	ObjectHandle m_vertexBufferHandle;

	std::vector<unsigned int> m_indices;
	ObjectHandle m_indexBufferHandle;

	std::vector<Bone> m_bones;
};

typedef std::vector<ModelData> ModelDataList;

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

	ModelDataList m_modelDataList;

	BoneNodePtr m_rootNode;
	std::unordered_map<std::string, aiAnimation*> m_animations;

protected:
	ModelType m_modelType;
	std::string m_modelFileName;

	const aiScene* m_assimpScene;
	Assimp::Importer m_modelImporter;

	static const char s_MaxJointCount = 120;
	static XMMATRIX boneMatrices[s_MaxJointCount];

	friend class ModelManager;
};