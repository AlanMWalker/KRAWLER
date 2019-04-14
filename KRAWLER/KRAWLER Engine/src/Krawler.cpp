
#include "Krawler.h"
#include "KApplication.h"
#include "AssetLoader\KAssetLoader.h"


#include <future>
#include <SFML\Graphics\VertexBuffer.hpp>

#define UUID_PREFIX L"18-5E-0F-45-8D-BA"

using namespace Krawler;
using namespace Krawler::Maths;


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
	if (!sf::VertexBuffer::isAvailable())
	{
		KPRINTF("ERROR! VertexBuffers are not available on this machine!");
		return KInitStatus::Failure;
	}
	
	if (!sf::Shader::isAvailable())
	{
		KPRINTF("ERROR! Shaders are not available on this machine!");
		return KInitStatus::Failure;
	}

	Profiler::SetupProfiler();

	return KApplication::getApp()->initialiseScenes();
}

void Krawler::ShutdownEngine()
{
	KAssetLoader::getAssetLoader().cleanupAssetLoader();
	// Cleanup applicaiton
	auto app = KApplication::getApp();
	app->cleanupApplication();
	Profiler::ShutdownProfiler();
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
