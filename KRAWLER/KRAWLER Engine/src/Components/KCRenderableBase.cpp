#include "Components\KCRenderableBase.h"
#include "AssetLoader/KAssetLoader.h"

using namespace Krawler;
using namespace Krawler::Components;

KCRenderableBase::KCRenderableBase(Krawler::KEntity* pEntity)
	: KComponentBase(pEntity)
{
	m_pShader = KAssetLoader::getAssetLoader().getShader(KTEXT("default"));
}

