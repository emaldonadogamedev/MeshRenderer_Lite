#include <Utilities/precompiled.h>

#include <Systems/Graphics/TextureClasses/TextureManager/TextureManager.h>
#include <Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include <Systems/Graphics/DX11Renderer/DX11RendererData.h>
#include <Systems/Graphics/GraphicsUtilities/D3DObjects.h>

TextureManager::TextureManager(DX11Renderer * const renderer)
	:m_renderer(renderer)
{

}

TextureManager::~TextureManager()
{

}

Texture2D* TextureManager::GetTexture2D(const std::string& fileName)
{
	const auto it = m_loaded2DTextures.find(fileName);
	if (it != m_loaded2DTextures.end())
	{
		return &m_renderer->GetRendererData().textures2D[ it->second];
	}

	return nullptr;
}

Texture2D* TextureManager::ReLoadTexture2D(const std::string& fileName)
{
	EraseTexture2D(fileName);
	return LoadTexture2D(fileName);
}

void TextureManager::EraseTexture2D(const std::string& fileName)
{
	const auto it = m_loaded2DTextures.find(fileName);
	if (it != m_loaded2DTextures.end())
	{
		m_loaded2DTextures.erase(it);
	}
}

void TextureManager::EraseAllTextures()
{
	m_loaded2DTextures.clear();
}

Texture2D* TextureManager::LoadTexture2D(const std::string& fileName)
{
	ObjectHandle handle;
	m_renderer->CreateTexture2D(handle, s_textureDir + fileName);

	if (handle)
	{
		return &m_renderer->m_renderData->textures2D[*handle];
	}

	return nullptr;
}

const std::string TextureManager::s_textureDir = "../MeshRenderer_Lite_Framework/Assets/Textures/";
