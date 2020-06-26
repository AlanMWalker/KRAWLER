
#include "Krawler.h"
#include "KApplication.h"
#include "AssetLoader\KAssetLoader.h"


#include <future>
#include <SFML\Graphics\VertexBuffer.hpp>

#pragma comment(lib, "rpcrt4.lib")  // UuidCreate - Minimum supported OS Win 2000

#define UUID_PREFIX L"18-5E-0F-45-8D-BA"

#ifndef LINUX
#include <Windows.h>	
#endif

using namespace Krawler;
using namespace Krawler::Maths;


KInitStatus Krawler::StartupEngine(KApplicationInitialise* windowInit)
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
	UUID uuid;
	UuidCreate(&uuid);
	wchar_t* str = nullptr;
	UuidToString(&uuid, (RPC_WSTR*)&str);
	std::wstring a(str);
	RpcStringFreeW((RPC_WSTR*)&str);
	return a;
}
