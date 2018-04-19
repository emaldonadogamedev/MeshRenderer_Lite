#include <Utilities/precompiled.h>

#include <Engine/Applications/TestApp/TestApp.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
  TestApp testApp;
  
  if (testApp.Initialize(hInstance))
	  testApp.Run();
  
  testApp.Shutdown();

  return 0;
}