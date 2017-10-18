#include <Utilities/precompiled.h>
#include <Systems/Graphics/ModelClasses/ModelManager.h>

//#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <Systems/Graphics/ModelClasses/Model.h>

ModelManager::ModelManager()
{

}

ModelManager::~ModelManager()
{
	EraseAllModels();
}

const Model* ModelManager::GetModel(const std::string& fileName)
{
	const auto& it = m_loadedModels.find(fileName);
	if (it != m_loadedModels.end())
	{
		return it->second.get();
	}

	return LoadModel(fileName);
}

const Model* ModelManager::ReLoadModel(const std::string& fileName)
{
	EraseModel(fileName);
	return LoadModel(fileName);
}

void ModelManager::EraseModel(const std::string& fileName)
{
	const auto& it = m_loadedModels.find(fileName);
	if (it != m_loadedModels.end())
	{
		it->second.reset();
		m_loadedModels.erase(it);
	}
}

void ModelManager::EraseAllModels()
{
	for (auto&& it : m_loadedModels)
	{
		it.second.reset();
	}
	m_loadedModels.clear();
}

Model* ModelManager::LoadModel(const std::string& fileName)
{
	static Assimp::Importer importer;

	const unsigned int loadFlags = aiProcess_Triangulate
		| aiProcess_GenNormals
		| aiProcess_GenUVCoords
#ifdef _WIN32
		//| aiProcess_MakeLeftHanded //Make left-hand side loading if using DirectX
#endif
		| aiProcessPreset_TargetRealtime_MaxQuality;

	const aiScene* const loadedScene = importer.ReadFile((s_modelDir + fileName).c_str(), loadFlags);
	//const aiScene* scene = aiImportFile((s_modelDir + fileName).c_str(), loadFlags);
	
	if (loadedScene)
	{
		auto newUniqModel = std::make_unique<Model>();
		Model* const newModel = newUniqModel.get();

		const unsigned int numberOfMeshes = loadedScene->mNumMeshes;
		auto& newModelData = newModel->m_ModelData;

		for (unsigned int meshIndex = 0; meshIndex < numberOfMeshes; ++meshIndex)
		{
			const aiMesh* const currentMesh = loadedScene->mMeshes[meshIndex];

			PopulateVertexModelData(newModelData, currentMesh);
			PopulateIndexModelData(newModelData, currentMesh);
			PopulateAnimationData(newModelData, currentMesh);

			newModel->m_DataList.emplace_back(newModelData);
		}

		m_loadedModels[fileName] = std::move(newUniqModel);
		return newModel;
	}

	return nullptr;
}

void ModelManager::PopulateVertexModelData(ModelData& modelData, const aiMesh* assimpMesh)
{
	const unsigned int numberOfVertices = assimpMesh->mNumVertices;
	modelData.m_vertices.reserve(numberOfVertices);

	const bool hasPositions = assimpMesh->HasPositions();
	const bool hasNormals = assimpMesh->HasNormals();
	const bool hasTangentsAndBitangents = assimpMesh->HasTangentsAndBitangents();
	const bool hasUVs = assimpMesh->HasTextureCoords(0);
	const bool hasColors = assimpMesh->HasVertexColors(0);

	auto positionsPtr = assimpMesh->mVertices;
	auto normalsPtr = assimpMesh->mNormals;
	auto tangentsPtr = assimpMesh->mTangents;
	auto biTangentsPtr = assimpMesh->mBitangents;
	auto textureCoordsPtr = *assimpMesh->mTextureCoords;
	auto colorsPtr = *assimpMesh->mColors;

	for (unsigned int vertexIndex = 0; vertexIndex < numberOfVertices; ++vertexIndex)
	{
		Vertex newVertex;
		if (hasPositions)
		{
			newVertex.position.Set(positionsPtr->x, positionsPtr->y, positionsPtr->z);
			++positionsPtr;
		}

		if (hasNormals)
		{
			newVertex.normal.Set(normalsPtr->x, normalsPtr->y, normalsPtr->z);
			++normalsPtr;
		}

		if (hasTangentsAndBitangents)
		{
			newVertex.tangent.Set(tangentsPtr->x, tangentsPtr->y, tangentsPtr->z);
			newVertex.biTangent.Set(biTangentsPtr->x, biTangentsPtr->y, biTangentsPtr->z);
			++tangentsPtr;
			++biTangentsPtr;
		}
		
		if (hasUVs)
		{
			newVertex.uv.Set(textureCoordsPtr->x, textureCoordsPtr->y);
			++textureCoordsPtr;
		}

		if (hasColors)
		{
			newVertex.color.Set(colorsPtr->r, colorsPtr->g, colorsPtr->b, colorsPtr->a);
		}

		modelData.m_vertices.emplace_back(std::move(newVertex));
	}
}

void ModelManager::PopulateIndexModelData(ModelData& modelData, const aiMesh* assimpMesh)
{
	if (assimpMesh->HasFaces())
	{
		const unsigned int numberOfFaces = assimpMesh->mNumFaces;
		modelData.m_indices.resize(numberOfFaces * 3);

		auto facesPtr = assimpMesh->mFaces;

		for (unsigned int faceIndex = 0; faceIndex < numberOfFaces; ++faceIndex)
		{
			unsigned int currentBaseIndex = faceIndex * 3;
			modelData.m_indices[currentBaseIndex] = facesPtr->mIndices[0];
			modelData.m_indices[currentBaseIndex + 1] = facesPtr->mIndices[1];
			modelData.m_indices[currentBaseIndex + 2] = facesPtr->mIndices[2];
			++facesPtr;
		}
	}
}

void ModelManager::PopulateAnimationData(ModelData& modelData, const aiMesh* assimpMesh)
{
	if (assimpMesh->HasBones())
	{
	}
}

const string ModelManager::s_modelDir = "CS562_framework/Assets/Models/";