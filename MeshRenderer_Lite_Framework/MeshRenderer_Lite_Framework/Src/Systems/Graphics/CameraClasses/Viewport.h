#pragma once

class Viewport
{
public:
	Viewport(float x = 0, float y = 0, float w = 0, float h = 0, float minZ = 0.0f, float maxZ = 1.0f) {}
	~Viewport() {}

  float m_xPos, m_yPos, m_width, m_height, m_minZ, m_maxZ;
};