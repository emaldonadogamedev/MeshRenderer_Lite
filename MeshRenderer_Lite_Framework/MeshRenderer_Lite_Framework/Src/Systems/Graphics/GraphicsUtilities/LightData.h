#include <DirectXMath.h>

using DirectX::XMFLOAT4;

enum class LightType : int
{
	DirectionalLight,
	PointLight,
	SpotLight
};

struct Light
{
	Light()
		: m_Position(0.0f, 0.0f, 0.0f, 1.0f)
		, m_Direction(0.0f, 0.0f, 1.0f, 0.0f)
		, m_Ia(0.0f, 0.0f, 0.0f, 1.0f)
		, m_Id(0.8f, 0.8f, 0.8f, 1.0f)
		, m_Is(1.0f, 1.0f, 1.0f, 1.0f)
		, m_SpotAngleInner(DirectX::XM_PIDIV4 / 3.0f)
		, m_SpotAngleOuter(DirectX::XM_PIDIV4)
		, m_SpotFallOff(1.0f)
		, m_ConstantAttenuation(1.0f)
		, m_LinearAttenuation(0.1f)
		, m_QuadraticAttenuation(0.0f)
		, m_lightType((int)LightType::PointLight)
		, m_Enabled(false)
	{
	}

	XMFLOAT4    m_Position;
	XMFLOAT4    m_Direction;
	XMFLOAT4    m_Ia;
	XMFLOAT4    m_Id;
	XMFLOAT4    m_Is;

	float       m_SpotAngleInner;
	float       m_SpotAngleOuter;
	float       m_SpotFallOff;
	float       m_ConstantAttenuation;

	float       m_LinearAttenuation;
	float       m_QuadraticAttenuation;
	int   m_lightType;
	int   m_Enabled;
	// Add some padding to make this struct size a multiple of 16 bytes.
	//int         Padding;
};