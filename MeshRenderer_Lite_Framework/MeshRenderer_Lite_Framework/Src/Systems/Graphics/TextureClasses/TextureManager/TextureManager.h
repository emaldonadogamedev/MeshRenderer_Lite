#pragma once

#include <Systems/Graphics/GraphicsUtilities/ObjectHandle.h>
#include <unordered_map>
#include <memory>
#include <string>

class DX11Renderer;
class GraphicsSystem;
struct Texture1D;
struct Texture2D;
struct Texture3D;


class TextureManager 
{
public:
	TextureManager(DX11Renderer * const renderer);
	~TextureManager();

	Texture1D* GetTexture1D(const std::string& fileName);
	Texture1D* ReLoadTexture1D(const std::string& fileName);
	void EraseTexture1D(const std::string& fileName);

	Texture2D* GetTexture2D(const std::string& fileName);
	Texture2D* ReLoadTexture2D(const std::string& fileName);
	void EraseTexture2D(const std::string& fileName);

	Texture3D* GetTexture3D(const std::string& fileName);
	Texture3D* ReLoadTexture3D(const std::string& fileName);
	void EraseTexture3D(const std::string& fileName);

	void EraseAllTextures();

private:
	Texture1D* LoadTexture1D(const std::string& fileName);
	Texture2D* LoadTexture2D(const std::string& fileName);
	Texture3D* LoadTexture3D(const std::string& fileName);

	std::unordered_map<std::string, ObjectHandle> m_loaded1DTextures;
	std::unordered_map<std::string, ObjectHandle> m_loaded2DTextures;
	std::unordered_map<std::string, ObjectHandle> m_loaded3DTextures;
	DX11Renderer * const m_renderer;

	static const std::string s_textureDir;

	friend class GraphicsSystem;
};