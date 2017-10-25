#include <Utilities/precompiled.h>
#include <Systems/Graphics/ModelClasses/ModelManager/ModelManager.h>

//#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <Systems/Graphics/ModelClasses/Model/Model.h>

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
		newModel->SetModelType(loadedScene->HasAnimations() ? ModelType::MODEL_STATIC : MODEL_STATIC);

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
		VertexAnimation newVertex;
		if (hasPositions)
		{
			newVertex.position.x = positionsPtr->x;
			newVertex.position.y = positionsPtr->y;
			newVertex.position.z = positionsPtr->z;
			++positionsPtr;
		}

		if (hasNormals)
		{
			newVertex.normal.x = normalsPtr->x;
			newVertex.normal.y = normalsPtr->y;
			newVertex.normal.z = normalsPtr->z;
			++normalsPtr;
		}

		if (hasTangentsAndBitangents)
		{
			newVertex.tangent.x = tangentsPtr->x;
			newVertex.tangent.y = tangentsPtr->y;
			newVertex.tangent.z = tangentsPtr->z;

			newVertex.biTangent.x = biTangentsPtr->x;
			newVertex.biTangent.y = biTangentsPtr->y;
			newVertex.biTangent.z = biTangentsPtr->z;
			++tangentsPtr;
			++biTangentsPtr;
		}
		
		if (hasUVs)
		{
			newVertex.uv.x = textureCoordsPtr->x;
			newVertex.uv.y = textureCoordsPtr->y;
			++textureCoordsPtr;
		}

		if (hasColors)
		{
			newVertex.color.x = colorsPtr->r;
			newVertex.color.y = colorsPtr->g;
			newVertex.color.z = colorsPtr->b;
			newVertex.color.z = colorsPtr->a;
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

void ModelManager::NormalizeFLOAT3(XMFLOAT3& v)
{
	const float l = std::sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));

	if (l != 0)
	{
		v.x /= l;
		v.y /= l;
		v.z /= l;

		return;
	}

	v.x = v.y = v.z = 0.f;
}

const std::string ModelManager::s_modelDir = "CS562_framework/Assets/Models/";