// KRAWLER.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "vld.h"

#include "Krawler.h"
#include "KApplication.h"
#include "LogicState\KLogicStateDirector.h"


using namespace Krawler;
using namespace Krawler::LogicState;

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

	KGameObject objA, objB, objC;

	auto inst = KApplication::getApplicationInstance();

	KLogicStateInitialiser stateInit;
	stateInit.stateIdentifier = L"play";
	inst->getLogicStateDirector()->registerLogicState(new KLogicState, &stateInit);
	inst->getLogicStateDirector()->setActiveLogicState(TEXT("play"));

	objA.setSize(Vec2f(100.f, 100.f));
	objB.setSize(Vec2f(150, 200));
	objC.setSize(Vec2f(500, 500));
	objC.setFillColour(sf::Color::Red);
	objC.setRenderLayer(-2);
	
	inst->getRenderer()->addToRendererQueue(&objA);
	inst->getRenderer()->addToRendererQueue(&objB);
	inst->getRenderer()->addToRendererQueue(&objC);

	RunApplication();

	ShutdownEngine();

	return EXIT_SUCCESS;
}


