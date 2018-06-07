#pragma once

#include <Krawler.h>
#include <KComponent.h>
#include <KApplication.h>
#include <Input\KInput.h>
#include <AssetLoader\KAssetLoader.h>
#include <Components\KCAnimatedSprite.h>
using namespace Krawler;
using namespace Krawler::Input;
using namespace Krawler::Components;

class AnimationTest : public KComponentBase
{
public:
	AnimationTest(KEntity* pEntity)
		: KComponentBase(pEntity)
	{

	}
	~AnimationTest() = default;
	KEntity* pEntity;
	virtual KInitStatus init() override
	{
		KApplication* pApp = KApplication::getApp();

		KScene* pScene = pApp->getCurrentScene();
		pEntity = pScene->addEntityToScene();
		KAssetLoader& assetLoader = KAssetLoader::getAssetLoader();

		pEntity->addComponent(new KCAnimatedSprite(pEntity, assetLoader.getAnimation(KTEXT("player_run_anim"))));
		pEntity->getComponent<KCAnimatedSprite>()->setRepeatingState(true);
		pEntity->getTransformComponent()->setTranslation(Vec2f(120, 210));
		pEntity->getTransformComponent()->setScale(1, 1);
		return KComponentBase::init();
	}

	virtual void tick() override
	{
		if (KInput::Pressed(KKey::Space))
		{
			pEntity->getComponent<KCAnimatedSprite>()->play();
		}

		if (KInput::Pressed(KKey::P))
		{
			pEntity->getComponent<KCAnimatedSprite>()->pause();
		}

		if (KInput::Pressed(KKey::S))
		{
			pEntity->getComponent<KCAnimatedSprite>()->stop();
		}

		if (KInput::Pressed(KKey::Up))
		{
			pEntity->getComponent<KCAnimatedSprite>()->setAnimation(KTEXT("player_run_anim"));
		}

		if (KInput::Pressed(KKey::Down))
		{
			pEntity->getComponent<KCAnimatedSprite>()->setAnimation(KTEXT("enemy_run_anim"));
		}
		
		if (KInput::Pressed(KKey::Left))
		{
			pEntity->getComponent<KCAnimatedSprite>()->setAnimation(KTEXT("player_explode_anim"));
		}
		
		if (KInput::Pressed(KKey::Right))
		{
			pEntity->getComponent<KCAnimatedSprite>()->setAnimation(KTEXT("enemy_explode_anim"));
		}
	}

private:

};