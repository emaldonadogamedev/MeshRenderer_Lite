#include<Utilities/precompiled.h>
#include<Systems/Window/WindowSystem.h>
#include <Engine/IApplication/IApplication.h>

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return IApplication::s_engineApp->MessageHandler(hwnd, msg, wParam, lParam);
}

WindowSystem::WindowSystem(IApplication* const eng, HINSTANCE hInstance) : ISystem(SystemType::ST_WINDOW, eng),
m_hInstance(hInstance),
m_isFullScreen(false),
m_hwnd(nullptr),
m_windowWidth(1024),
m_windowHeight(700),
m_windowCaption(L" Framework CS 541 / CS 562")
{
	//The size passed to CreateWindow is the full size including the windows border and caption 
	//AdjustWindowRect will adjust the provided rect so that the client size of the window is the desired size
	RECT fullWinRect = { 0, 0, m_windowWidth, m_windowHeight };
	AdjustWindowRect(&fullWinRect,			//The rectangle for the full size of the window
		WS_OVERLAPPEDWINDOW /*^ WS_BORDER*/,	//The style of the window, which must match what is passed in to CreateWindow below
		FALSE);					//Does this window have a menu?

								//WNDXCLASSEX
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.hInstance = m_hInstance;
	wcex.lpfnWndProc = MainWndProc;
	wcex.hIcon = LoadIcon(NULL, IDI_HAND);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = (LPCSTR)m_windowCaption.c_str();
	wcex.hIconSm = LoadIcon(NULL, IDI_HAND);
	//Register the window class for the game.
	//WNDCLASSEX wc = { sizeof(WNDCLASSEX),	//The size of this structure (passing the size allows Microsoft to update their interfaces and maintain backward compatibility)
	//  CS_CLASSDC,							//The style of the window class--this is the base type (one device context for all windows in the process)
	//  MainWndProc,						//The name of the message handling function
	//  0L, 0L,								//The amount of extra memory to allocate for this class and window
	//  m_hInstance,//GetModuleHandle(NULL),				//Handle to the instance that has the windows procedure--NULL means use this file.
	//  LoadIcon(0, IDI_APPLICATION),		//Add an Icon as a resource and add them here
	//  LoadCursor(NULL, IDC_ARROW),		//Use the default arrow cursor
	//  (HBRUSH)GetStockObject(BLACK_BRUSH),
	//  NULL,								//The background brush and menu--these can be NULL
	//  (LPCSTR)m_windowCaption.c_str(), NULL };			//The class name and the small icon (NULL just uses the default)

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(0, "RegisterClass Failed.", 0, 0);
		return;
	}

	//Store the handle to the instance
	//m_hInstance = wc.hInstance;

	UINT width = fullWinRect.right - fullWinRect.left;
	UINT height = fullWinRect.bottom - fullWinRect.top;
	UINT x = GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2;
	UINT y = GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2;

	//Create the game's window
	m_hwnd = CreateWindow(
		(LPCSTR)m_windowCaption.c_str(),	//The class name
		(LPCSTR)m_windowCaption.c_str(),	//The name for the title bar
		WS_OVERLAPPEDWINDOW /*^ WS_BORDER*/,	//The style of the window (WS_BORDER, WS_MINIMIZEBOX, WS_MAXIMIZE, etc.)
		x,
		y,						//The x and y position of the window (screen coords for base windows, relative coords for child windows)
		width,					//Width of the window, including borders
		height,				//Height of the window, including borders and caption
		GetDesktopWindow(),				//The parent window
		NULL,								//The menu for the window
		m_hInstance,						//The handle to the instance of the window (ignored in NT/2K/XP)
		NULL);								//The lParam for the WM_CREATE message of this window

}

WindowSystem::~WindowSystem()
{

}

bool WindowSystem::Initialize()
{
	if (!m_hwnd)
	{
		MessageBox(0, "Failed to create window.", 0, 0);
		return m_isInitialized = false;
	}

	ShowWindow(m_hwnd, SW_SHOW);
	UpdateWindow(m_hwnd);

	return m_isInitialized = true;
}

void WindowSystem::Update(const float dt)
{

}

void WindowSystem::Shutdown()
{

}

HWND WindowSystem::GetWindowsHandler() const
{
	return m_hwnd;
}

float WindowSystem::GetAspectRatio(void) const
{
	return (float)m_windowWidth / (float)m_windowHeight;
}

int WindowSystem::GetWindowWidth() const
{
	return m_windowWidth;
}

int WindowSystem::GetWindowHeight() const
{
	return m_windowHeight;
}

bool WindowSystem::IsFullScreen() const
{
	return m_isFullScreen;
}

void WindowSystem::SetWindowWidth(unsigned int width)
{
	m_windowWidth = width;
}

void WindowSystem::SetWindowHeight(unsigned int height)
{
	m_windowHeight = height;
}

void WindowSystem::ReceiveMessage(const IMessage& msg)
{
}
