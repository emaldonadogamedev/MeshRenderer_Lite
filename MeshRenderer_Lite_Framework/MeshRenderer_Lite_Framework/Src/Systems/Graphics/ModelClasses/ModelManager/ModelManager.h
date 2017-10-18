#pragma once

#include<unordered_map>
#include<memory>
#include<string.h>

class GraphicsSystem;
class Model;
class ModelData;
struct aiMesh;

typedef std::unordered_map<std::string, std::unique_ptr<Model>> ModelUmap;

class ModelManager
{
public:
	ModelManager();
	~ModelManager();

	const Model* GetModel(const std::string& fileName);
	const Model* ReLoadModel(const std::string& fileName);
	void EraseModel(const std::string& fileName);
	void EraseAllModels();

private:
	Model* LoadModel(const std::string& fileName);
	void PopulateVertexModelData(ModelData& modelData, const aiMesh* assimpMesh);
	void PopulateIndexModelData(ModelData& modelData, const aiMesh* assimpMesh);
	void PopulateAnimationData(ModelData& modelData, const aiMesh* assimpMesh);

	ModelUmap m_loadedModels;

	static const string s_modelDir;

	friend GraphicsSystem;
};