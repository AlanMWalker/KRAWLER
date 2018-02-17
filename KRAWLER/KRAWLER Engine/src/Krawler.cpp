
#include "Krawler.h"
#include "KApplication.h"
#include "AssetLoader\KAssetLoader.h"

#include <future>

#define UUID_PREFIX L"18-5E-0F-45-8D-BA"

using namespace Krawler;
using namespace Krawler::Maths;

extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

KInitStatus Krawler::StartupEngine(KApplicationInitialise * windowInit)
{
	KApplication* const app = KApplication::getApp();
	app->setupApplication(*windowInit);
	KAssetLoader::getAssetLoader();

	InitRand();

	return KInitStatus::Success;
}

KRAWLER_API KInitStatus Krawler::InitialiseSubmodules()
{
	return KApplication::getApp()->initialiseScenes();
}

void Krawler::ShutdownEngine()
{
	KAssetLoader::getAssetLoader().cleanupAssetLoader();
	// Cleanup applicaiton
	auto app = KApplication::getApp();
	app->cleanupApplication();
	KFREE(app);
}

void Krawler::RunApplication()
{
	auto app = KApplication::getApp();
	app->runApplication();
}

std::wstring Krawler::GenerateUUID()
{
	const auto epochTime = std::chrono::system_clock::now().time_since_epoch(); // time since unix epoch
	const sf::Int64 timeInMS = std::chrono::duration_cast<std::chrono::nanoseconds>(epochTime).count();
	//std::this_thread::sleep_for(std::chrono::nanoseconds(5));//sleep to ensure time elapse
	return UUID_PREFIX + std::wstring(L"-") + std::to_wstring(timeInMS);
}
