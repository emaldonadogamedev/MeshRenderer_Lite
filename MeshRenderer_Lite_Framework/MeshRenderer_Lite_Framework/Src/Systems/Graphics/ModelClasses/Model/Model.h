#pragma once
#include <string>
#include <vector>
#include <Systems/Graphics/GraphicsUtilities/VertexTypes.h>
#include <Systems/Graphics/GraphicsUtilities/ObjectHandle.h>
#include <Systems/Graphics/ModelClasses/MeshMaterial/MeshEntryMaterial.h>
#include <assimp/Importer.hpp>

class GraphicsSystem;
class DX11Renderer;
class PrimitiveGenerator;

//assimp forward declarations
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


struct BoneStructureNode;

typedef std::unique_ptr<BoneStructureNode> BoneNodePtr;
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
	MeshEntryMaterial meshMaterial = MeshEntryMaterial::GetPresetMaterial(PredefinedMaterials::RedPlastic);
	std::string diffTextureName = "";
	std::string normalMapName = "";
	int assImpMaterialIndex = 0;
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

	void SetDiffTextureFileName(const std::string& fileName = "", const unsigned int meshIdx = 0);

	const std::string& GetFileName() const;
	void SetFileName(const std::string& fileName);

//For TESTING ONLY!!
//private:

	MeshEntryList m_meshEntryList;

	BoneNodePtr m_rootNode;
	std::vector<const char*> m_animationNameCharPtrs;
	std::vector<aiAnimation*> m_animations;

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
	float m_ticksPerSecond = 24.0f;
	int m_currentAnimIndex;
	bool m_animationEnabled = false;
	aiMatrix4x4 m_globalInverseTransform;
	std::vector<aiMatrix4x4> m_boneOffsetMtxVec;
	std::vector<XMMATRIX> m_boneFinalTransformMtxVec;
	unsigned int m_numBones;
	std::unordered_map<std::string, unsigned int> m_boneMapping; // maps a bone name to its index
	
	//FOR DEBUG DRAW!!
	bool m_drawSkin = true;
	bool m_debugDrawEnabled;
	std::vector<XMVECTOR> m_boneLocations;
	std::vector<unsigned int> m_boneLocIndBuff;
	ObjectHandle m_boneLocIndBufferHandle;

	std::unordered_map<int, std::string> m_diffTextures;
	std::unordered_map<int, std::string> m_normalTextures;

	//Assimp data
	const aiScene* m_assimpScene;
	Importer m_modelImporter;

	static const unsigned char s_maxBoneCount = 120;
	static const unsigned short s_maxBoneLocCount = 120 * 6;

	friend class PrimitiveGenerator;
	friend class ModelManager;
	friend class GraphicsSystem;
};