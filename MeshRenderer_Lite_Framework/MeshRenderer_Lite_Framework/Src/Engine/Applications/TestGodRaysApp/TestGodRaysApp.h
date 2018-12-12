#pragma once

#include <Engine/Applications/IApplication/IApplication.h>

#define BOX_GRID_DIM 3

class TestGodRaysApp : public IApplication
{

public:
	TestGodRaysApp();
	~TestGodRaysApp();

	virtual bool Initialize(HINSTANCE hInstance) override;
	virtual void Shutdown(void) override;

protected:
	std::unique_ptr<GameObject> testHorizontalBoxes[BOX_GRID_DIM];
	std::unique_ptr<GameObject> testVerticalBoxes[BOX_GRID_DIM];

	std::unique_ptr<GameObject> testFloor;
	std::unique_ptr<GameObject> testLight;
};