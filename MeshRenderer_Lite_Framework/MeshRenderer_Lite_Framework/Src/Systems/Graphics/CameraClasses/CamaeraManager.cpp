#include <Utilities/precompiled.h>
#include <Systems/Graphics/CameraClasses/CameraManager.h>
#include <Systems/Graphics/CameraClasses/Camera.h>
#include <Systems/Graphics/CameraClasses/Viewport.h>

CameraManager::CameraManager()
{
	AddCamera();
	AddViewport();
}

CameraManager::~CameraManager()
{
	for (Camera* cam : m_cameras)
	{
		SafeDelete(cam);
	}
	for (Viewport* v : m_viewports)
	{
		SafeDelete(v);
	}
}

Camera* CameraManager::GetActiveCamera() const
{
	return m_cameras[m_activeCamera];
}

Camera* CameraManager::GetCamera(int index) const
{
#if _DEBUG
	if (index < 0 || index >= m_cameras.size())
		return nullptr;
#endif
	return m_cameras[index];
}

int CameraManager::GetCurrentCamIndex() const
{
	return m_activeCamera;
}

Viewport* CameraManager::GetActiveViewport() const
{
	return m_viewports[m_activeViewport];
}

Viewport* CameraManager::GetViewport(int index) const
{
#if _DEBUG
	if (index < 0 || index >= m_viewports.size())
		return nullptr;
#endif

	return m_viewports[index];
}

int CameraManager::GetCurrentViewportIndex() const
{
	return m_activeViewport;
}

Camera* CameraManager::AddCamera(bool setActive)
{
	Camera* result = new Camera();

	if (result)
	{
		m_cameras.push_back(result);
		if (setActive)
		{
			m_activeCamera = m_cameras.size() - 1;
		}

		return result;
	}

	return nullptr;
}

Camera* CameraManager::SetActiveCamera(int index)
{
#if _DEBUG
	if (index < 0 || index >= m_cameras.size())
	{
		m_activeCamera = index;
		return m_cameras[index];
	}

	return nullptr;
#else
	m_activeCamera = index;
	return m_cameras[index];
#endif
}

Viewport* CameraManager::AddViewport(bool setActive)
{
	Viewport* result = new Viewport();

	if (result)
	{
		m_viewports.push_back(result);
		if (setActive)
		{
			m_activeViewport = m_viewports.size() - 1;
		}

		return result;
	}

	return nullptr;
}

Viewport* CameraManager::SetActiveViewport(int index)
{
#if _DEBUG
	if (index < 0 || index >= m_viewports.size())
	{
		m_activeViewport = index;
		return m_viewports[index];
	}

	return nullptr;
#else
	m_activeViewport = index;
	return m_viewports[index];
#endif
}