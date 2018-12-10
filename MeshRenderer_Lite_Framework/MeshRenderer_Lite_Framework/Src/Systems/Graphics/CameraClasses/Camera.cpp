#include<Utilities/precompiled.h>
#include<Systems/Graphics/CameraClasses/Camera.h>

using namespace DirectX;

Camera::Camera() : m_Position(XMVectorSet(0.0f, 0.0f, -156.0f, 0.0f)),
m_LookAt(s_ZERO),
m_Up(s_defaultUp),
m_Right(s_defaultRight),
m_Forward(s_defaultForward),
m_NearWindowHeight(0.0f),
m_FarWindowHeight(0.0f),
m_AspectRatio(0.0f),
m_Near(0.03f),
m_Far(500.0f),
m_FOV(DirectX::XM_PIDIV4),
m_Speed(45.0f),
m_rotSpeed(10.0f),
m_moveLeftRight(0.f),
m_moveBackForward(0.f),
m_isUsingSkybox(false),
m_skyboxTexture(),
m_isSkyboxDirty(false)
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

  m_Projection = XMMatrixTranspose(XMMatrixPerspectiveFovLH(m_FOV, m_AspectRatio, m_Near, m_Far));
}

const XMMATRIX& Camera::GetView() const
{
  return m_View;
}

const XMMATRIX& Camera::GetProjection() const
{
  return m_Projection;
}

bool Camera::GetIsUsingSkybox() const
{
		return m_isUsingSkybox;
}

void Camera::SetIsUsingSkybox(const bool v)
{
		m_isUsingSkybox = v;
}

const std::string& Camera::GetSkyboxTexture() const
{
	return m_skyboxTexture;
}

void Camera::SetSkyboxTexture(const std::string& fileName)
{
	m_skyboxTexture = fileName;
}

void Camera::Strafe(const float dt)
{
  //m_Position += s_defaultRight * dt * m_Speed;
  m_moveLeftRight += dt * m_Speed;
}

void Camera::Walk(const float dt)
{
  //m_Position += s_defaultForward * dt * m_Speed;
	m_moveBackForward += dt * m_Speed;
}

void Camera::Elevate(const float dt)
{
  m_Position += s_defaultUp * dt * m_Speed;
}

void Camera::RotateX(const float dt)
{
	m_camPitch += dt * m_rotSpeed;
}

void Camera::RotateY(const float dt)
{
	m_camYaw += dt * m_rotSpeed;
}

void Camera::Update()
{
	m_camRotationMatrix = XMMatrixRotationRollPitchYaw(m_camPitch, m_camYaw, 0);
	m_LookAt = XMVector3TransformCoord(s_defaultForward, m_camRotationMatrix);
	m_LookAt = XMVector3Normalize(m_LookAt);

	XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = XMMatrixRotationY(m_camYaw);

	m_Right = XMVector3TransformCoord(s_defaultRight, RotateYTempMatrix);
	m_Up = XMVector3TransformCoord(m_Up, RotateYTempMatrix);
	m_Forward = XMVector3TransformCoord(s_defaultForward, RotateYTempMatrix);

	m_Position += m_moveLeftRight * m_Right;
	m_Position += m_moveBackForward * m_Forward;

	m_moveLeftRight = 0.0f;
	m_moveBackForward = 0.0f;

	m_LookAt = m_Position + m_LookAt;

	m_View = XMMatrixTranspose(XMMatrixLookAtLH(m_Position, m_LookAt, m_Up));
}

void Camera::ResetAxis()
{
	m_Position = XMVectorSet(0.0f, 0.0f, -156.0f, 1.0f);
	m_Right = s_defaultRight;
	m_Up = s_defaultUp;
	m_Forward = s_defaultForward;
	m_LookAt = s_ZERO;

	m_moveLeftRight = 0.0f;
	m_moveBackForward = 0.0f;
	m_camYaw = 0.f;
	m_camPitch = 0.f;
}