#pragma once

#include<unordered_map>
#include<memory>
#include<string.h>

class GraphicsSystem;
class Model;
struct MeshEntry;
class DX11Renderer;

namespace DirectX {
	struct XMFLOAT3;
}

using DirectX::XMFLOAT3;

struct aiMesh;
struct aiScene;

typedef std::unordered_map<std::string, std::unique_ptr<Model>> ModelUmap;

class ModelManager
{
public:
	ModelManager(DX11Renderer * const renderer);
	~ModelManager();

	const Model* GetModel(const std::string& fileName);
	const Model* ReLoadModel(const std::string& fileName);
	void EraseModel(const std::string& fileName);
	void EraseAllModels();

private:
	Model* LoadModel(const std::string& fileName);
	void PopulateAnimationData(Model& model, const aiScene* const assimpScene);
	void PopulateVertexModelData(Model& model, const aiMesh* const assimpMesh);
	void PopulateIndexModelData(Model& model, const aiMesh* const assimpMesh);
	void PopulateBoneData(Model& model, const aiMesh* const assimpMesh, const unsigned int meshIndex);

	ModelUmap m_loadedModels;
	DX11Renderer * const m_renderer;

	static const std::string s_modelDir;

	friend GraphicsSystem;
};