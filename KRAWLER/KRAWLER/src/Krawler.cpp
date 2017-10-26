#include "stdafx.h"	
#include "Krawler.h"

#include "KApplication.h"

using namespace Krawler;

KInitStatus Krawler::StartupEngine(KApplicationInitialise * windowInit)
{
	KApplication* const app = KApplication::getApplicationInstance();
	app->setupApplication(*windowInit);

	return KInitStatus::Success;
}

void Krawler::ShutdownEngine()
{
}

void Krawler::RunApplication()
{
	auto app = KApplication::getApplicationInstance();
	SAFE_FREE(app);

	KApplication::getApplicationInstance()->runApplication();
}
