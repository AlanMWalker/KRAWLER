#include "Components\KCRenderableBase.h"
#include "AssetLoader/KAssetLoader.h"

using namespace Krawler;
using namespace Krawler::Components;

#include "AssetLoader\KAssetLoader.h"

KCRenderableBase::KCRenderableBase(Krawler::KEntity* pEntity)
	: KComponentBase(pEntity)
{
	this->setShader(ASSET().getShader(KTEXT("default")));
}

