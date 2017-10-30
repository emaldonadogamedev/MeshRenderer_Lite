#include <Utilities/precompiled.h>
#include <Systems/Graphics/ModelClasses/ModelManager/ModelManager.h>

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
	for (auto& it : m_loadedModels)
	{
		it.second.reset();
	}
	m_loadedModels.clear();
}

Model* ModelManager::LoadModel(const std::string& fileName)
{
	auto newUniqModel = std::make_unique<Model>();

	const unsigned int loadFlags = aiProcess_Triangulate
		| aiProcess_GenNormals
		| aiProcess_GenUVCoords
		| aiProcess_MakeLeftHanded //Make left-hand side loading, we're using DirectX
		| aiProcessPreset_TargetRealtime_MaxQuality;

	auto loadedScene = newUniqModel->m_modelImporter.ReadFile((s_modelDir + fileName).c_str(), loadFlags);
	//const aiScene* scene = aiImportFile((s_modelDir + fileName).c_str(), loadFlags);
	
	if (loadedScene)
	{
		Model* const newModel = newUniqModel.get();
		newModel->m_assimpScene = loadedScene;

		newModel->SetModelType(loadedScene->HasAnimations() ? ModelType::MODEL_STATIC : MODEL_STATIC);

		PopulateAnimationData(*newModel, loadedScene);

		const unsigned int numberOfMeshes = loadedScene->mNumMeshes;
		ModelData newModelData;

		for (unsigned int meshIndex = 0; meshIndex < numberOfMeshes; ++meshIndex) 
		{
			const aiMesh* const currentMesh = loadedScene->mMeshes[meshIndex];

			PopulateVertexModelData(newModelData, currentMesh);
			PopulateIndexModelData(newModelData, currentMesh);
			PopulateBoneData(newModelData, currentMesh);

			newModel->m_modelDataList.emplace_back(newModelData);
		}

		m_loadedModels[fileName] = std::move(newUniqModel);

		return newModel;
	}

	return nullptr;
}

void PopulateBoneNodeData(BoneNodePtr& root, const aiNode* const assimpRoot, const std::string& assimpRootName)
{
	if (!assimpRoot)
		return;

	root->name = assimpRootName;
	root->transformation = XMMATRIX(&assimpRoot->mTransformation.a1);
	const unsigned int numberOfChildren = assimpRoot->mNumChildren;
	for (unsigned int i = 0; i < numberOfChildren; ++i)
	{
		BoneNodePtr newChildNode = std::make_unique<BoneNode>();
		newChildNode->parent = root.get();
		const std::string assimpChildName = std::string(assimpRoot->mChildren[i]->mName.C_Str());
		PopulateBoneNodeData(newChildNode, assimpRoot->mChildren[i], assimpChildName);
		//root->children[assimpChildName] = std::move(newChildNode);
		root->children.emplace_back(std::move(newChildNode));
	}
}

void ModelManager::PopulateAnimationData(Model& model, const aiScene* const assimpScene)
{
	if (assimpScene->HasAnimations())
	{
		model.m_rootNode = std::make_unique<BoneNode>();

		//LOAD ROOT NODE
		PopulateBoneNodeData(model.m_rootNode, assimpScene->mRootNode, std::string(assimpScene->mRootNode->mName.C_Str()));

		//LOAD SCENE ANIMATIONS
		const unsigned int numberOfAnimations = assimpScene->mNumAnimations;

		for (unsigned int animIndex = 0; animIndex < numberOfAnimations; ++animIndex)
		{
			const std::string animName = assimpScene->mAnimations[animIndex]->mName.C_Str();
			model.m_animations[animName] = assimpScene->mAnimations[animIndex];
		}
	}
}

void ModelManager::PopulateVertexModelData(ModelData& modelData, const aiMesh* const assimpMesh)
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

void ModelManager::PopulateIndexModelData(ModelData& modelData, const aiMesh* const assimpMesh)
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

void ModelManager::PopulateBoneData(ModelData& modelData, const aiMesh* const assimpMesh)
{
	if (assimpMesh->HasBones())
	{
		std::vector<char> vertexWeightCountTracker(modelData.m_vertices.size(), 0);

		const unsigned int numberOfBones = assimpMesh->mNumBones;
		auto bonesPtr = assimpMesh->mBones;

		for (unsigned int boneIndex = 0; boneIndex < numberOfBones; ++boneIndex)
		{
			Bone newBone;
			newBone.name = std::string(bonesPtr[boneIndex]->mName.C_Str());
			newBone.offsetMatrix = XMMATRIX(&bonesPtr[boneIndex]->mOffsetMatrix.a1);
			
			const unsigned int numberOfWeights = bonesPtr[boneIndex]->mNumWeights;
			auto weightPtr = bonesPtr[boneIndex]->mWeights;
			for (unsigned int weightIndex = 0; weightIndex < numberOfWeights; ++weightIndex)
			{
				//See if the vertex has space left to store weights (MAX is 4)
				const char ptrOffset = vertexWeightCountTracker[weightPtr->mVertexId];
				if (ptrOffset < 4)
				{
					auto& vertex = modelData.m_vertices[weightPtr->mVertexId];
					int* newBoneID = &vertex.boneIDs.x + vertexWeightCountTracker[weightPtr->mVertexId];
					float* newWeight = &vertex.boneWeights.x + vertexWeightCountTracker[weightPtr->mVertexId];

					//Give the vertex the bone ID as stored in the Bones container of the model data
					*newBoneID = static_cast<int>(modelData.m_bones.size());
					*newWeight = weightPtr->mWeight;

					//Report that a weight has been added
					++vertexWeightCountTracker[weightPtr->mVertexId];
				}

				newBone.weights.emplace_back(VertexWeight(weightPtr->mVertexId, weightPtr->mWeight));
				++weightPtr;
			}
			modelData.m_bones.emplace_back(std::move(newBone));
		}
	}
}

const std::string ModelManager::s_modelDir = "../MeshRenderer_Lite_Framework/Assets/Models/";