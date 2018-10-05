#pragma once

#include<DirectXMath.h>
#include<unordered_map>

using DirectX::XMFLOAT4;
using DirectX::XMFLOAT3;

//In this cool website they list some cool material values:
//https://www.3dgep.com/texturing-lighting-directx-11/#Materials_Properties

enum class PredefinedMaterials : int
{
		Emerald,
		Jade,
		GreenPlastic,
		RedPlastic,
		YellowRubber,
		CyanRubber
};

struct MaterialProperties
{
		//Just packing all of the variables like that in order to align them(4 32 bit elements at a time)
		XMFLOAT4 ambientKa;
		XMFLOAT4 diffuseKd;

		XMFLOAT3 specularKs;
		float specularPowerNs = 200;

		XMFLOAT4 emissiveKe;

		int useDiffuseTexture = 0;
		int useNormalMap = 0;
		int useAlphaBlending = 0;
		int padding;
};

struct MeshEntryMaterial
{

public:
		MeshEntryMaterial(const XMFLOAT4& ka, const XMFLOAT4& kd, const XMFLOAT3& ks, const float ns, 
				const XMFLOAT4& ke = XMFLOAT4(0, 0, 0, 0), const int useDiffText = 1, const int useNormMap = 1)
		{
				m_materialProperties.ambientKa = ka;
				m_materialProperties.diffuseKd = kd;
				m_materialProperties.specularKs = ks;
				m_materialProperties.specularPowerNs = ns;
				m_materialProperties.emissiveKe = ke;
				m_materialProperties.useDiffuseTexture = useDiffText;
				m_materialProperties.useNormalMap = useNormMap;
		}

		MeshEntryMaterial& operator=(const MeshEntryMaterial& rhs);

		MaterialProperties m_materialProperties;

		void SetToPresetMaterial(const PredefinedMaterials preset);
		static const MeshEntryMaterial& GetPresetMaterial(const PredefinedMaterials preset);

private:
		const static std::unordered_map<PredefinedMaterials, MeshEntryMaterial> s_PresetMaterials;
};