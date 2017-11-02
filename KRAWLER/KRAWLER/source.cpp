// KRAWLER.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "vld.h"

#include "Krawler.h"
#include "KApplication.h"


using namespace Krawler;

#define ENABLE_WIN32 1

#ifndef ENABLE_WIN32
int main(void)
#else
int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
#endif
{
	KApplicationInitialise init;
	init.width = 1024;
	init.height = 720;
	init.gameFps = 60;
	init.physicsFps = 100;
	init.windowTitle = "Hello KRAWLER!";
	init.windowStyle = Windowed_Resizeable;

	StartupEngine(&init);

	RunApplication();

	ShutdownEngine();

	return EXIT_SUCCESS;
}


