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
		| aiProcess_GenSmoothNormals
		| aiProcess_GenUVCoords
		| aiProcess_ConvertToLeftHanded //Make left-hand side loading, we're using DirectX
		//| aiProcessPreset_TargetRealtime_MaxQuality <-- not using it for now
		;

	auto loadedScene = newUniqModel->m_modelImporter.ReadFile((s_modelDir + fileName).c_str(), loadFlags);
	//const aiScene* scene = aiImportFile((s_modelDir + fileName).c_str(), loadFlags);
	
	if (loadedScene)
	{
		Model* const newModel = newUniqModel.get();
		newModel->m_assimpScene = loadedScene;
		newModel->SetModelType(loadedScene->HasAnimations() ? ModelType::MODEL_SKINNED: MODEL_STATIC);

		PopulateAnimationData(*newModel, loadedScene);
		
		// Count the number of vertices and indices in the scene
		const unsigned int numberOfMeshes = loadedScene->mNumMeshes;
		unsigned int vertexCount = 0;
		unsigned int indicesCount = 0;

		newModel->m_meshEntryList.resize(numberOfMeshes);
		for (int i = 0; i < newModel->m_meshEntryList.size(); i++) {
			newModel->m_meshEntryList[i].materialIndex = loadedScene->mMeshes[i]->mMaterialIndex;
			//The * 3 is because it's assumed that the faces are triangulated
			newModel->m_meshEntryList[i].numIndices = loadedScene->mMeshes[i]->mNumFaces * 3;
			newModel->m_meshEntryList[i].baseVertex = vertexCount;
			newModel->m_meshEntryList[i].baseIndex = indicesCount;

			vertexCount += loadedScene->mMeshes[i]->mNumVertices;
			indicesCount += newModel->m_meshEntryList[i].numIndices;
		}

		//reserve the memory for vertices and indices
		newModel->m_vertices.reserve(vertexCount);
		newModel->m_indices.reserve(indicesCount);

		//Populate all vertex, index, and bone data
		for (unsigned int meshIndex = 0; meshIndex < numberOfMeshes; ++meshIndex) 
		{
			MeshEntry newMeshEntry;
			const aiMesh* const currentMesh = loadedScene->mMeshes[meshIndex];

			PopulateVertexModelData(*newModel, currentMesh);
			PopulateIndexModelData(*newModel, currentMesh);
			PopulateBoneData(*newModel, currentMesh, meshIndex);
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
		BoneNodePtr newChildNode = std::make_unique<BoneStructureNode>();
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
		model.m_rootNode = std::make_unique<BoneStructureNode>();

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

void ModelManager::PopulateVertexModelData(Model& model, const aiMesh* const assimpMesh)
{
	const unsigned int numberOfVertices = assimpMesh->mNumVertices;

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

	//Vertex with animation data(bone IDs and weights)
	VertexAnimation newVertex;

	for (unsigned int vertexIndex = 0; vertexIndex < numberOfVertices; ++vertexIndex)
	{
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

		model.m_vertices.emplace_back(newVertex);
	}
}

void ModelManager::PopulateIndexModelData(Model& model, const aiMesh* const assimpMesh)
{
	if (assimpMesh->HasFaces())
	{
		const unsigned int numberOfFaces = assimpMesh->mNumFaces;
		const auto* facesPtr = assimpMesh->mFaces;

		for (unsigned int faceIndex = 0; faceIndex < numberOfFaces; ++faceIndex)
		{
			model.m_indices.emplace_back(facesPtr->mIndices[0]);
			model.m_indices.emplace_back(facesPtr->mIndices[1]);
			model.m_indices.emplace_back(facesPtr->mIndices[2]);
			++facesPtr;
		}
	}
}

void ModelManager::PopulateBoneData(Model& model, const aiMesh* const assimpMesh, const unsigned int meshIndex)
{
	if (assimpMesh->HasBones())
	{
		std::vector<char> vertexWeightCountTracker(model.m_vertices.size(), 0);

		const unsigned int numberOfBones = assimpMesh->mNumBones;
		auto bonesPtr = assimpMesh->mBones;

		for (unsigned int boneIndex = 0; boneIndex < numberOfBones; ++boneIndex)
		{
			unsigned int newOrOldBoneIndex = 0;
			const std::string boneName = std::string(bonesPtr[boneIndex]->mName.C_Str());
			const auto it = model.m_boneMapping.find(boneName);
			//if no bone mapped, we'll add a new one
			if (it == model.m_boneMapping.end())
			{
				// Allocate an index for a new bone
				newOrOldBoneIndex = model.m_numBones;
				model.m_numBones++;
				BoneMatrixInfo boneMtxInfo;
				boneMtxInfo.offsetMatrix = DirectX::XMMATRIX(&bonesPtr[boneIndex]->mOffsetMatrix.a1);
				model.m_boneMatrices.emplace_back(boneMtxInfo);
				model.m_boneMapping[boneName] = newOrOldBoneIndex;
			}
			else
			{
				//Now it's getting the old Bone Index
				newOrOldBoneIndex = model.m_boneMapping[boneName];
			}

			for (unsigned int weightIndex = 0; weightIndex < assimpMesh->mBones[boneIndex]->mNumWeights; weightIndex++) {
				const unsigned int VertexID = model.m_meshEntryList[meshIndex].baseVertex + bonesPtr[boneIndex]->mWeights[weightIndex].mVertexId;
				const float Weight = bonesPtr[boneIndex]->mWeights[weightIndex].mWeight;

				//Check if we still can add bone id and weight to the vertex
				// Max bone transformation influence is 4
				const char ptrOffset = vertexWeightCountTracker[VertexID];
				if (ptrOffset < 4)
				{
					auto& vertex = model.m_vertices[VertexID];
					int* const newBoneID = &vertex.boneIDs.x + ptrOffset;
					float* const newWeight = &vertex.boneWeights.x + ptrOffset;

					//Give the vertex the bone ID as stored in the Bones container of the model data
					*newBoneID = static_cast<int>(newOrOldBoneIndex);
					*newWeight = Weight;

					++vertexWeightCountTracker[VertexID];
				}
			}
		}
	}
}

const std::string ModelManager::s_modelDir = "../MeshRenderer_Lite_Framework/Assets/Models/";