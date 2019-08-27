#include <Utilities/precompiled.h>

#include <Engine/Applications/TestApp/TestApp.h>
#include <Engine/Applications/TestIBLapp/TestIBLapp.h>
#include <Engine/Applications/TestAmbOccApp/TestAOapp.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	//insert new random seed based on the time
	srand(time(NULL));

	TestApp testApp;
  
	if (testApp.Initialize(hInstance))
		testApp.Run();
	
	testApp.Shutdown();

	return 0;
}