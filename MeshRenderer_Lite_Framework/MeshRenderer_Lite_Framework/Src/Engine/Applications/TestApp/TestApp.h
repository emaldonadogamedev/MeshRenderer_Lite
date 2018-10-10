#pragma once

#include <Engine/Applications/IApplication/IApplication.h>

#define MINI_LIGHT_COUNT 100

class TestApp : public IApplication {

public:
	TestApp();
	~TestApp();

	virtual bool Initialize(HINSTANCE hInstance) override;
	virtual void Shutdown(void) override;

protected:
	std::unique_ptr<GameObject> testObj;
	std::unique_ptr<GameObject> testFloor;
	std::unique_ptr<GameObject> testLight;
	std::unique_ptr<GameObject> testLight2;

	std::unique_ptr<GameObject> testMiniLights[MINI_LIGHT_COUNT];
};