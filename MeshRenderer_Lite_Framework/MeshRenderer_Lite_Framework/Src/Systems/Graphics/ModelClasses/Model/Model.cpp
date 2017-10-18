/******************************************************************************
Copyright (C) 2015 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
/******************************************************************************/

#include <Systems/Graphics/GraphicsUtilities/IRenderer.h>

////TODO: Better way of detecting windows or not
//#if defined(_WIN32)
//#include <Systems/Graphics/DX11Renderer/DX11Renderer.h>
//#else
////TODO: #include future 'OpenGL Renderer'
//#endif
#include <Systems/Graphics/ModelClasses/Model.h>

Model::Model() :m_vertexCount(0), m_indexCount(0), m_modelType(ModelType::MODEL_STATIC)
{
}

Model::Model(const ModelData& data) : m_ModelData(data),
m_vertexCount(m_ModelData.m_vertices.size()), m_indexCount(m_ModelData.m_indices.size()), m_modelType(ModelType::MODEL_STATIC)
{
}

Model::~Model()
{
	m_ModelData.m_vertices.clear();
	m_ModelData.m_indices.clear();
}

void Model::GenerateBuffers(IRenderer* renderContext)
{
	m_vertexCount = m_ModelData.m_vertices.size();
	m_indexCount = m_ModelData.m_indices.size();

	for (unsigned i = 0; i < m_DataList.size(); ++i)
	{
		renderContext->CreateVertexBuffer(m_DataList[i].m_vertexBufferHandle, BufferUsage::USAGE_DEFAULT, sizeof(Vertex) * m_DataList[i].m_vertices.size(), m_DataList[i].m_vertices.data());
		renderContext->CreateIndexBuffer(m_DataList[i].m_indexBufferHandle, BufferUsage::USAGE_DEFAULT, sizeof(unsigned int) * m_DataList[i].m_indices.size(), m_DataList[i].m_indices.data());
	}
}

const ObjectHandle& Model::GetVertexBuffer(bool pointsOnly) const
{
	return m_ModelData.m_vertexBufferHandle;
}

unsigned int Model::GetVertexCount() const
{
	return m_vertexCount;
}

const ObjectHandle& Model::GetIndexBuffer() const
{
	return m_ModelData.m_indexBufferHandle;
}

unsigned int Model::GetIndexCount(unsigned int ID) const
{
	if (ID >= m_DataList.size())
		return 0;

	return m_DataList[ID].m_indices.size();
}

ModelType Model::GetModelType() const
{
	return m_modelType;
}

void Model::SetModelType(ModelType type)
{
	m_modelType = type;
}

const std::string& Model::GetFileName() const
{
	return m_modelFileName;
}

void Model::GetFileName(const std::string& fileName)
{
	m_modelFileName = fileName;
}