#pragma once

#include <Engine/Applications/IApplication/IApplication.h>

class TestAmbOcclussionApp : public IApplication
{
public:
	TestAmbOcclussionApp();
	~TestAmbOcclussionApp();

	virtual bool Initialize(HINSTANCE hInstance) override;
	virtual void Shutdown(void) override;

protected:
	std::unique_ptr<GameObject> testFloor;
	std::unique_ptr<GameObject> testWall1;
	std::unique_ptr<GameObject> testWall2;

	std::unique_ptr<GameObject> testLight;
	std::unique_ptr<GameObject> testLight2;
};