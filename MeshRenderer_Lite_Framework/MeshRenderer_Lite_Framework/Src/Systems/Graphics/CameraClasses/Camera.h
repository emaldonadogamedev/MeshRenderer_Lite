/******************************************************************************/
/*!
\file  Camera.h
\project  CS562
\author Esteban Enrique Maldonado Cabán

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#pragma once

/*
From MSDN documentation:
When _XM_NO_INTRINSICS_ is defined, DirectXMath operations are implemented
without using any platform-specific intrinsics. Instead, DirectXMath uses
only standard floating point operations
*/
#define _XM_NO_INTRINSICS_
#include <DirectXMath.h>

using DirectX::XMVECTOR;
using DirectX::XMMATRIX;

class Camera
{
public:
  Camera();
  ~Camera();

  void Resize(const float fov, const float aspectratio, const float near, const float far);

  const XMMATRIX& GetView()const;
  const XMMATRIX& GetProjection()const;

  bool GetIsUsingSkybox() const;
  void SetIsUsingSkybox(const bool v);
  
  const std::string& GetSkyboxTexture() const;
  void SetSkyboxTexture(const std::string& fileName);


  //Camera controls
  void Strafe(const float dt);
  void Walk(const float dt);
  void Elevate(const float dt);
  void Pitch(const float angle);
  void RotateX(const float dt);
  void RotateY(const float dt);
  void Update();
  void ResetAxis();


  XMVECTOR m_Position;
  XMVECTOR m_LookAt;
  XMVECTOR m_Forward;
  XMVECTOR m_Up;
  XMVECTOR m_Right;

  float m_NearWindowHeight;
  float m_FarWindowHeight;
  float m_AspectRatio;
  float m_Near;
  float m_Far;
  float m_FOV;
  float m_Speed;
  float m_rotSpeed;
  float m_camYaw = 0.0f;
  float m_camPitch = 0.0f;

	float m_moveLeftRight;
  float m_moveBackForward;

protected:
	bool m_isUsingSkybox;
	std::string m_skyboxTexture;
	bool m_isSkyboxDirty;

  XMMATRIX m_camRotationMatrix;
  XMMATRIX m_View;
  XMMATRIX m_Projection;

	friend class GraphicsSystem;
};