#include<Utilities/precompiled.h>
#include<Systems/Graphics/CameraClasses/Camera.h>

using namespace DirectX;

namespace DXMathConstants {
	DirectX::XMVECTOR g_Up = DirectX::XMVectorSet(0, 1, 0, 0);
	DirectX::XMVECTOR g_Down = DirectX::XMVectorSet(0, -1, 0, 0);
	DirectX::XMVECTOR g_Right = DirectX::XMVectorSet(1, 0, 0, 0);
	DirectX::XMVECTOR g_Forward = DirectX::XMVectorSet(0, 0, 1, 0);
}

Camera::Camera() : m_Position(XMVectorSet(0.0f, 0.0f, -17.0f, 0.0f)),
m_LookAt(XMVectorSet(0.0f, 0.0f, 0.0f,0)),
m_Up(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)),
m_Right(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f)),
m_NearWindowHeight(0.0f),
m_FarWindowHeight(0.0f),
m_AspectRatio(0.0f),
m_Near(0.01f),
m_Far(1000.0f),
m_FOV(DirectX::XM_PIDIV4),
m_Speed(10)
{
	m_View = m_Projection = XMMatrixIdentity();
}

Camera::~Camera()
{
}

void Camera::Resize(const float fov, const float aspectratio, const float n, const float f)
{
  m_AspectRatio = aspectratio;
  m_Near = n;
  m_Far = f;
  m_FOV = fov;

  m_NearWindowHeight = 2.0f * m_Near * tanf(0.5f * m_FOV);
  m_FarWindowHeight = 2.0f * m_Far * tanf(0.5f * m_FOV);

  m_Projection = XMMatrixTranspose(XMMatrixPerspectiveLH(m_FOV, m_AspectRatio, m_Near, m_Far));
}

const XMMATRIX& Camera::GetView() const
{
  return m_View;
}

const XMMATRIX& Camera::GetProjection() const
{
  return m_Projection;
}

void Camera::Strafe(const float dt)
{
  m_Position += DXMathConstants::g_Right * dt * m_Speed;
}

void Camera::Walk(const float dt)
{
  m_Position += DXMathConstants::g_Forward * dt * m_Speed;
}

void Camera::Elevate(const float dt)
{
  m_Position += DXMathConstants::g_Up * dt * m_Speed;
}

void Camera::Update()
{
	m_View = XMMatrixTranspose(XMMatrixLookAtLH(m_Position, m_LookAt, m_Up));

	// Keep camera's axes orthogonal to each other and of unit length.
	//m_LookAt = XMVector3Normalize(m_LookAt);
	//m_Up = XMVector3Normalize(XMVector3Cross(m_LookAt, m_Right));
}

void Camera::ResetAxis()
{
  m_LookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0);
  m_Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0);
  m_Right = XMVectorSet(1.0f, 0.0f, 0.0f, 0);
}