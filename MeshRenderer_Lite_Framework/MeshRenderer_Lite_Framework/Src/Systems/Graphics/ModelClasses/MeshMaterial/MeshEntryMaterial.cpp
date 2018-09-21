#include <Utilities/precompiled.h>
#include <Systems/Graphics/ModelClasses/MeshMaterial/MeshEntryMaterial.h>

MeshEntryMaterial& MeshEntryMaterial::operator=(const MeshEntryMaterial& rhs)
{
		m_phongMaterial.ambientKa = rhs.m_phongMaterial.ambientKa;
		m_phongMaterial.diffuseKd = rhs.m_phongMaterial.diffuseKd;
		m_phongMaterial.specularKs = rhs.m_phongMaterial.specularKs;
		m_phongMaterial.specularPowerNs = rhs.m_phongMaterial.specularPowerNs;
		m_phongMaterial.emissiveKe = rhs.m_phongMaterial.emissiveKe;
		m_phongMaterial.useDiffuseTexture = rhs.m_phongMaterial.useDiffuseTexture;
		m_phongMaterial.useNormalMap = rhs.m_phongMaterial.useNormalMap;

		return *this;
}

void MeshEntryMaterial::SetToPresetMaterial(const PredefinedMaterials preset)
{
	*this = s_PresetMaterials.at(preset);
}

const MeshEntryMaterial & MeshEntryMaterial::GetPresetMaterial(const PredefinedMaterials preset)
{
		return s_PresetMaterials.at(preset);
}

const std::unordered_map<PredefinedMaterials, MeshEntryMaterial> MeshEntryMaterial::s_PresetMaterials =
{
		{
				PredefinedMaterials::Emerald,
				MeshEntryMaterial(
						XMFLOAT4(0.0215f, 0.1745f, 0.0215f, 1.0f),
						XMFLOAT4(0.07568f, 0.61424f, 0.07568f, 1.0f),
						XMFLOAT3(0.633f, 0.727811f, 0.633f),
						76.8f
				)
		},
		{
				PredefinedMaterials::Jade,
				MeshEntryMaterial(
						XMFLOAT4(0.135f, 0.2225f, 0.1575f, 1.0f),
						XMFLOAT4(0.54f, 0.89f, 0.63f, 1.0f),
						XMFLOAT3(0.316228f, 0.316228f, 0.316228f),
						12.8f
				)
		},
		{
				PredefinedMaterials::GreenPlastic,
				MeshEntryMaterial(
						XMFLOAT4(0.f, 0.f, 0.f, 1.0f),
						XMFLOAT4(0.1f, 0.99f, 0.1f, 1.0f),
						XMFLOAT3(0.45f, 0.55f, 0.45f),
						32.f
				)
		},
		{
				PredefinedMaterials::RedPlastic,
				MeshEntryMaterial(
						XMFLOAT4(0.f, 0.f, 0.f, 1.0f),
						XMFLOAT4(0.5f, 0.f, 0.f, 1.0f),
						XMFLOAT3(0.7f, 0.6f, 0.6f),
						32.f
				)
		},
		{
				PredefinedMaterials::YellowRubber,
				MeshEntryMaterial(
						XMFLOAT4(0.05f, 0.05f, 0.f, 1.0f),
						XMFLOAT4(0.5f, 0.5f, 0.4f, 1.0f),
						XMFLOAT3(0.7f, 0.7f, 0.04f),
						10.f
				)
		},
		{
				PredefinedMaterials::CyanRubber,
				MeshEntryMaterial(
						XMFLOAT4(0.f, 0.05f, 0.05f, 1.0f),
						XMFLOAT4(0.4f, 0.5f, 0.5f, 1.0f),
						XMFLOAT3(0.04f, 0.7f, 0.7f),
						10.f
				)
		}

		//TODO: Add more materials here
};
