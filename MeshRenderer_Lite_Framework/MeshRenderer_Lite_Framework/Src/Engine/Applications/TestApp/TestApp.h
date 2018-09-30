#pragma once

#include <Engine/Applications/IApplication/IApplication.h>

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

	std::unique_ptr<GameObject> testMiniLights[16];
};