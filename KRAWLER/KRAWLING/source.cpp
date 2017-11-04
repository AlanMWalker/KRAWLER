#include "stdafx.h"
#include "vld.h"

#include <Krawler.h>
#include <KApplication.h>
#include <LogicState\KLogicStateDirector.h>
#include <LogicState\KLogicState.h>

using namespace Krawler;
using namespace Krawler::LogicState;

int main(void)
{
	KApplicationInitialise initApp;
	initApp.consoleWindow = true;
	initApp.width = 1024;
	initApp.height = 720;
	initApp.windowStyle = Windowed_Fixed_Size;
	initApp.gameFps = 60;
	initApp.physicsFps = 100;
	initApp.windowTitle = TEXT("Hello World!");

	StartupEngine(&initApp);

	KLogicState state;
	auto application = KApplication::getApplicationInstance();

	KLogicStateInitialiser initState;

	application->getLogicStateDirector()->registerLogicState(&state, &initState);
	application->getLogicStateDirector()->setActiveLogicState(initState.stateIdentifier);

	RunApplication();
	ShutdownEngine();

	return 0;
}

