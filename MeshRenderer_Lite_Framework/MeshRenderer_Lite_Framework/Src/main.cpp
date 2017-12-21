#include <Utilities/precompiled.h>

#include <IEngine/IEngine.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
  IEngine engine;
  
  if (engine.Initialize(hInstance))
    engine.Run();
  
  engine.Shutdown();

  return 0;
}