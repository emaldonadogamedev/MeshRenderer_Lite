#pragma once

#include <Engine/IApplication/IApplication.h>

class TestApp : public IApplication {

public:
	TestApp();
	~TestApp();

	virtual bool Initialize(HINSTANCE hInstance) override;
	virtual void Shutdown(void) override;

protected:
	std::unique_ptr<GameObject> testObj;
};