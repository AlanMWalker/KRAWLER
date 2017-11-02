#include "stdafx.h"	
#include "Krawler.h"

#include "KApplication.h"

using namespace Krawler;

extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

KInitStatus Krawler::StartupEngine(KApplicationInitialise * windowInit)
{
	KApplication* const app = KApplication::getApplicationInstance();
	app->setupApplication(*windowInit);

	return KInitStatus::Success;
}

void Krawler::ShutdownEngine()
{
	// Cleanup applicaiton
	auto app = KApplication::getApplicationInstance();
	app->cleanupApplication();
	KFREE(app);
}

void Krawler::RunApplication()
{
	auto app = KApplication::getApplicationInstance();
	

	KApplication::getApplicationInstance()->runApplication();
}
