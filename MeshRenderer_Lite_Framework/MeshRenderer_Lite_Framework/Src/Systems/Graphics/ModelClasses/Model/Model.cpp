#include <Utilities/precompiled.h>
#include <Systems/Graphics/DX11Renderer/DX11Renderer.h>

#include <Systems/Graphics/ModelClasses/Model/Model.h>

Model::Model()
	:m_modelType(ModelType::MODEL_STATIC)
	,m_rootNode(nullptr)
	,m_numBones(0)
	,m_ticksPerSecond(24.0f)
	, m_debugDrawEnabled(false)
{
	m_boneOffsetMtxVec.reserve(s_maxBoneCount);
	m_boneFinalTransformMtxVec.reserve(s_maxBoneCount);
}

Model::~Model()
{
	m_modelImporter.FreeScene();
}

void Model::GenerateBuffers(DX11Renderer* renderContext)
{
	renderContext->CreateVertexBuffer(m_vertexBufferHandle, BufferUsage::USAGE_DEFAULT, sizeof(VertexAnimation) * m_vertices.size(), m_vertices.data());
	renderContext->CreateIndexBuffer(m_indexBufferHandle, BufferUsage::USAGE_DEFAULT, sizeof(unsigned int) * m_indices.size(), m_indices.data());
}

const ObjectHandle Model::GetVBufferHandle() const
{
	return m_vertexBufferHandle;
}

const ObjectHandle Model::GetIBufferHandle() const
{
	return m_indexBufferHandle;
}

const int Model::GetVertexCount() const
{
	return (int)m_vertices.size();
}

const int Model::GetIndicesCount() const
{
	return (int)m_indices.size();
}

ModelType Model::GetModelType() const
{
	return m_modelType;
}

void Model::SetModelType(ModelType type)
{
	m_modelType = type;
}

bool Model::IsAnimationActive() const
{
	return m_animationEnabled;
}

void Model::SetDiffTextureFileName(const std::string& fileName, const unsigned int meshIdx, const bool useNow)
{
#if _DEBUG
	if (meshIdx < m_meshEntryList.size())
	{
		m_meshEntryList[meshIdx].diffTextureName = fileName;
		m_meshEntryList[meshIdx].meshMaterial.m_phongMaterial.useDiffuseTexture = useNow;
	}
	else
		throw std::exception("Error: meshIdx out of range!");
#else
	m_meshEntryList[meshIdx].diffTextureName = fileName;
	m_meshEntryList[meshIdx].meshMaterial.m_phongMaterial.useDiffuseTexture = useNow;
#endif
}

void Model::SetNormalMapFileName(const std::string& fileName, const unsigned int meshIdx, const bool useNow)
{
#if _DEBUG
		if (meshIdx < m_meshEntryList.size())
		{
				m_meshEntryList[meshIdx].diffTextureName = fileName;
				m_meshEntryList[meshIdx].meshMaterial.m_phongMaterial.useNormalMap = useNow;
		}
		else
				throw std::exception("Error: meshIdx out of range!");
#else
		m_meshEntryList[meshIdx].diffTextureName = fileName;
		m_meshEntryList[meshIdx].meshMaterial.m_phongMaterial.useNormalMap = useNow;
#endif
}

const std::string& Model::GetFileName() const
{
	return m_modelFileName;
}

void Model::SetFileName(const std::string& fileName)
{
	m_modelFileName = fileName;
}