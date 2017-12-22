#include <Utilities/precompiled.h>

#include <Engine/IApplication/IApplication.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
  IApplication engine;
  
  if (engine.Initialize(hInstance))
    engine.Run();
  
  engine.Shutdown();

  return 0;
}