#include <Utilities/precompiled.h>

#include <Engine/Engine.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
  Engine engine;
  
  if (engine.Initialize(hInstance))
    engine.Run();
  
  engine.Shutdown();

  return 0;
}