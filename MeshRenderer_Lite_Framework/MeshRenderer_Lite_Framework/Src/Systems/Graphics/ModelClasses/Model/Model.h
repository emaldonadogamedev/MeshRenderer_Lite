#pragma once
#include <string>
#include <vector>
#include <Systems/Graphics/GraphicsUtilities/VertexTypes.h>
#include <Systems/Graphics/GraphicsUtilities/ObjectHandle.h>

class DX11Renderer;
struct aiScene;

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

//////////////////////////////////////////////////////////////////////////
//Can be used for positions, rotations, and scalings
struct VectorKeys
{
	double time;
	XMVECTOR value;
};

struct BoneNodeAnimation
{
	std::string name;
	std::vector<VectorKeys> positions;
	std::vector<VectorKeys> rotations;
	std::vector<VectorKeys> scalings;
};

struct Animation
{
	std::string name;
	double duration;
	double ticksPerSecond;
	std::vector<BoneNodeAnimation> channels;
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
	std::vector<Animation> m_animations;

protected:
	ModelType m_modelType;
	std::string m_modelFileName;

	friend class ModelLoader;
};