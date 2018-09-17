#include <Utilities/precompiled.h>
#include <Systems/Graphics/ModelClasses/ModelManager/ModelManager.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include <Systems/Graphics/ModelClasses/Model/Model.h>
#include <Systems/Graphics/ModelClasses/PrimitiveGenerator/PrimitiveGenerator.h>


ModelManager::ModelManager(DX11Renderer * const renderer)
	:m_renderer(renderer)
	,m_primitiveGen(std::make_unique<PrimitiveGenerator>())
{
}

ModelManager::~ModelManager()
{
	EraseAllModels();
}

Model* ModelManager::GetModel(const std::string& fileName)
{
	const auto& it = m_loadedModels.find(fileName);
	if (it != m_loadedModels.end())
	{
		return it->second.get();
	}

	return LoadModel(fileName);
}

Model* ModelManager::ReLoadModel(const std::string& fileName)
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

void ModelManager::LoadPrimitiveShapes()
{
	auto box = std::make_unique<Model>();
	m_primitiveGen->CreateBox(1.0f, 1.0f, 1.0f, *box);
	box->GenerateBuffers(m_renderer);
	m_loadedModels["box"] = std::move(box);

	auto quad = std::make_unique<Model>();
	m_primitiveGen->CreateQuad(1.0f, 1.0f, 0.f, *quad);
	quad->GenerateBuffers(m_renderer);
	m_loadedModels["quad"] = std::move(quad);
	
	auto sphere = std::make_unique<Model>();
	m_primitiveGen->CreateSphere(0.5f, 60, 60, *sphere);
	sphere->GenerateBuffers(m_renderer);
	m_loadedModels["sphere"] = std::move(sphere);
}

Model* ModelManager::LoadModel(const std::string& fileName)
{
	const auto it = m_loadedModels.find(fileName);
	if (it != m_loadedModels.end())
		return it->second.get();

	auto newUniqModel = std::make_unique<Model>();

	static const unsigned int loadFlags = aiProcess_Triangulate
		| aiProcess_GenSmoothNormals
		| aiProcess_CalcTangentSpace
		| aiProcess_GenUVCoords
		| aiProcess_ConvertToLeftHanded //Make left-hand side loading, since I'm using DirectX
#ifdef _DEBUG
		| aiProcessPreset_TargetRealtime_Fast
#else
		| aiProcessPreset_TargetRealtime_MaxQuality
#endif
		;

	auto const loadedScene = newUniqModel->m_modelImporter.ReadFile((s_modelDir + fileName).c_str(), loadFlags);

	if (loadedScene)
	{
		Model* const newModel = newUniqModel.get();
		newModel->m_assimpScene = loadedScene;
		newModel->m_animationEnabled = loadedScene->HasAnimations();
		newModel->SetModelType(newModel->m_animationEnabled ? ModelType::MODEL_SKINNED : ModelType::MODEL_STATIC);
		auto inv = loadedScene->mRootNode->mTransformation;
		inv.Inverse();
		newModel->m_globalInverseTransform = std::move(inv);

		PopulateAnimationData(*newModel, loadedScene);
		PopulateMaterialData(*newModel, loadedScene);

		// Count the number of vertices and indices in the scene
		const unsigned int numberOfMeshes = loadedScene->mNumMeshes;
		unsigned int vertexCount = 0;
		unsigned int indicesCount = 0;

		newModel->m_meshEntryList.resize(numberOfMeshes);
		for (int i = 0; i < numberOfMeshes; i++)
		{
			newModel->m_meshEntryList[i].assImpMaterialIndex = loadedScene->mMeshes[i]->mMaterialIndex;
			if(!newModel->m_diffTextures.empty())
				newModel->m_meshEntryList[i].diffTextureName = newModel->m_diffTextures[loadedScene->mMeshes[i]->mMaterialIndex];

			//The * 3 is because the model is loaded as triangulated
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
			const aiMesh* const currentMesh = loadedScene->mMeshes[meshIndex];

			PopulateVertexModelData(*newModel, currentMesh);
			PopulateIndexModelData(*newModel, currentMesh);
			PopulateBoneData(*newModel, currentMesh, meshIndex);
		}

		newModel->GenerateBuffers(m_renderer);

		const size_t boneAmount = newModel->m_boneOffsetMtxVec.size();
		newModel->m_boneFinalTransformMtxVec.resize(boneAmount, DirectX::XMMatrixIdentity());
		newModel->m_boneLocations.resize(boneAmount, DirectX::XMVectorSet(0,0,0,1.0f));
		
		//Prepare the index buffer for debug info.
		newModel->m_boneLocIndBuff.resize(boneAmount * 6, 0);
		int ind = 0;
		for (unsigned int i = 0; i < newModel->m_boneLocIndBuff.size(); ++i)
		{
			newModel->m_boneLocIndBuff[i] = i;
		}

		if(!newModel->m_boneLocIndBuff.empty())
			m_renderer->CreateIndexBuffer(newModel->m_boneLocIndBufferHandle, BufferUsage::USAGE_DEFAULT, sizeof(unsigned int) * newModel->m_boneLocIndBuff.size(), newModel->m_boneLocIndBuff.data());

		m_loadedModels[fileName] = std::move(newUniqModel);

		return newModel;
	}
	
	const std::string err = newUniqModel->m_modelImporter.GetErrorString();

	return nullptr;
}

const std::string ExtractFileName(const aiString& assPath)
{
	std::string result(assPath.data);

	const size_t offset = result.find_last_of('\\') + 1;

	if (offset != std::string::npos) {
		result = result.substr(offset, result.size() - offset);
	}

	return result;
}

void ModelManager::PopulateMaterialData(Model& model, const aiScene* const assimpScene)
{
	if (assimpScene->HasMaterials()) 
	{
		const unsigned int numberOfMaterials = assimpScene->mNumMaterials;
		aiMaterial** const materialPtr = assimpScene->mMaterials;
		for (unsigned int materialIndex = 0; materialIndex < numberOfMaterials; ++materialIndex)
		{
			aiString assPath;
			static aiTextureMapping mapping = aiTextureMapping_UV;
			const auto diffTexture = materialPtr[materialIndex]->GetTexture(aiTextureType_DIFFUSE,0, &assPath, &mapping);

			//0 means success
			if (diffTexture == 0)
				model.m_diffTextures[materialIndex] = ExtractFileName(assPath);

			const auto normalTexture = materialPtr[materialIndex]->GetTexture(aiTextureType_NORMALS, 0, &assPath);
			if (normalTexture == 0)
				model.m_normalTextures[materialIndex] = ExtractFileName(assPath);
		}
	}
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
			model.m_animations.emplace_back(assimpScene->mAnimations[animIndex]);
			model.m_animationNameCharPtrs.emplace_back(assimpScene->mAnimations[animIndex]->mName.data);
		}

		//by default use the first animation available
		model.m_currentAnimIndex = 0;
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
	//auto textureCoordsPtr = *assimpMesh->mTextureCoords;
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
			newVertex.uv.x = assimpMesh->mTextureCoords[0][vertexIndex].x;
			newVertex.uv.y = assimpMesh->mTextureCoords[0][vertexIndex].y;
		}

		if (hasColors)
		{
			newVertex.color.x = colorsPtr->r;
			newVertex.color.y = colorsPtr->g;
			newVertex.color.z = colorsPtr->b;
			newVertex.color.w = colorsPtr->a;
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
			//Since we load the model triangulated, all faces will have 3 indices
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
			unsigned int BoneIndex = 0;
			const std::string boneName(bonesPtr[boneIndex]->mName.C_Str());
			const auto it = model.m_boneMapping.find(boneName);
			//if no bone mapped, we'll add a new one
			if (it == model.m_boneMapping.end())
			{
				// Allocate an index for a new bone
				BoneIndex = model.m_numBones;
				model.m_numBones++;
				model.m_boneOffsetMtxVec.emplace_back(bonesPtr[boneIndex]->mOffsetMatrix);
				model.m_boneMapping[boneName] = BoneIndex;
			}
			else
			{
				//Now it's getting the old Bone Index
				BoneIndex = it->second;
			}

			for (unsigned int weightIndex = 0; weightIndex < assimpMesh->mBones[boneIndex]->mNumWeights; ++weightIndex) {
				const unsigned int VertexID = model.m_meshEntryList[meshIndex].baseVertex + bonesPtr[boneIndex]->mWeights[weightIndex].mVertexId;
				const float Weight = bonesPtr[boneIndex]->mWeights[weightIndex].mWeight;

				//Check if we still can add bone id and weight to the vertex
				// Max bone transformation influence is 4
				char& ptrOffset = vertexWeightCountTracker[VertexID];
				if (ptrOffset < 4)
				{
					//Give the vertex the bone ID as stored in the Bones container of the model data
					auto& vertex = model.m_vertices[VertexID];
					vertex.boneIDs[ptrOffset] = BoneIndex;
					vertex.boneWeights[ptrOffset] = Weight;
					++ptrOffset;
				}
			}
		}
	}
}

const std::string ModelManager::s_modelDir = "../MeshRenderer_Lite_Framework/Assets/Models/";