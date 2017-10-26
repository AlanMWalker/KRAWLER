// KRAWLER.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "vld.h"

#include "Krawler.h"
#include "KApplication.h"

#include "source.h"
using namespace Krawler;

int main(void)
{
	KApplicationInitialise init;
	init.width = 640; 
	init.height = 480; 
	init.maxFps = 60; 
	init.windowTitle = "Hello KRAWLER!";
	init.windowStyle = Fullscreen;
	StartupEngine(&init);
	RunApplication();
	ShutdownEngine();
	return 0;
}


