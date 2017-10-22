#pragma once

#include<vector>

class GraphicsManager;
class Camera;
class Viewport;

typedef std::vector<Camera*> CameraList;
typedef std::vector<Viewport*> ViewportList;

class CameraManager {
	
public:
	CameraManager();
	~CameraManager();

	//Getters
	Camera* GetActiveCamera()const;
	Camera* GetCamera(int index)const;
	int GetCurrentCamIndex()const;

	Viewport* GetActiveViewport()const;
	Viewport* GetViewport(int index)const;
	int GetCurrentViewportIndex()const;

	//Setters
	Camera* AddCamera(bool setActive = true);
	Camera* SetActiveCamera(int index);

	Viewport* AddViewport(bool setActive = true);
	Viewport* SetActiveViewport(int index);

private:
	
	int m_activeCamera;
	CameraList m_cameras;

	int m_activeViewport;
	ViewportList m_viewports;

	friend GraphicsManager;
};