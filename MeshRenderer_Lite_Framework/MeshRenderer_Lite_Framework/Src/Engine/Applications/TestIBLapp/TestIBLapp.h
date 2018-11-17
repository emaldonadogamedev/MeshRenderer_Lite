#pragma once

#include <Engine/Applications/IApplication/IApplication.h>

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
};