#pragma once

#include <Engine/Applications/IApplication/IApplication.h>

#define TEST_SPHERE_COUNT 6

class TestIBLapp : public IApplication {

public:
	TestIBLapp();
	~TestIBLapp();

	virtual bool Initialize(HINSTANCE hInstance) override;
	virtual void Shutdown(void) override;

protected:
	std::unique_ptr<GameObject> testObj;
	std::unique_ptr<GameObject> testFloor;
	std::unique_ptr<GameObject> testLight;
	std::unique_ptr<GameObject> testSpheres[TEST_SPHERE_COUNT];
};