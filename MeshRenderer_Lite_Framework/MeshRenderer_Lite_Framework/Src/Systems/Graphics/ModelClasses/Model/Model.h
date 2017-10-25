#pragma once
#include <string>
#include <vector>
#include <Systems/Graphics/GraphicsUtilities/VertexTypes.h>
#include <Systems/Graphics/GraphicsUtilities/ObjectHandle.h>

class DX11Renderer;
struct aiScene;

enum ModelType
{
	MODEL_STATIC,
	MODEL_SKINNED,
	ONLY_ANIMATION
};

struct ModelData
{
	std::vector<VertexAnimation> m_vertices;
	ObjectHandle m_vertexBufferHandle;

	std::vector<unsigned int> m_indices;
	ObjectHandle m_indexBufferHandle;
};

typedef std::vector<ModelData> ModelDataList;

class Model
{
public:
	Model(void);
	Model(const ModelData& data);
	~Model(void);

	void GenerateBuffers(DX11Renderer* renderContext);

	const ObjectHandle& GetVertexBuffer(bool pointsOnly = false) const;
	unsigned int GetVertexCount(void) const;

	const ObjectHandle& GetIndexBuffer(void) const;
	unsigned int GetIndexCount(unsigned int ID) const;

	ModelType GetModelType(void) const;
	void SetModelType(ModelType type);

	const std::string& GetFileName() const;
	void GetFileName(const std::string& fileName);

	ModelData m_ModelData;
	ModelDataList m_DataList;

protected:
	unsigned int m_vertexCount;
	unsigned int m_indexCount;
	ModelType m_modelType;
	std::string m_modelFileName;
	aiScene* m_assimpScene;

	friend class ModelLoader;
};