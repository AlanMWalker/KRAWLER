#ifndef KENTITY_H
#define KENTITY_H

#include <vector>
#include <codecvt>
#include <locale>
#include <string>
#include <Krawler.h>
#include <KComponent.h>

namespace Krawler
{
	/*Entity class which components are attached to.
	All entities automatically have a transform component attatched upon construction.*/
	namespace Components
	{
		class KCTransform;
	}

	//Entities default to Dynamic
	//Dynamic = Added to a Quadtree that is regenerated per-frame
	//Static = Added to Quadtree generated at initialise and never updated again
	enum EntitySceneInteractivity
	{
		Dynamic,
		Static
	};
	constexpr int32 CHUNK_POOL_SIZE{ 500 };

	class KEntity
	{
	public:

		KRAWLER_API KEntity();
		KRAWLER_API ~KEntity();

		//Param: n/a 
		//Return: KInitStatus (success if init fine)
		//Info: initialise all components attached to an entity, if one retuns a failure entire init sequence will be halted
		KRAWLER_API KInitStatus init();

		//Param: n/a 
		//Return: n/a
		//Info: Cleanup all components attached to an entity
		KRAWLER_API void cleanUp();

		//Param: n/a 
		//Return: n/a
		//Info: tick all components attached to an entity
		KRAWLER_API void tick();

		//Param: n/a 
		//Return: n/a
		//Info: fixed-tick all components attached to an entity
		KRAWLER_API void fixedTick();

		//Param: n/a 
		//Return: n/a
		//Info: call onEnterScene for all components attached to an entity, upon entering a new game scene
		KRAWLER_API void onEnterScene();

		//Param: n/a 
		//Return: n/a
		//Info: call onExitScene for all components attached to an entity, upon exiting a game scene
		KRAWLER_API void onExitScene();

		//Param: pointer to componenet to be added
		//Return: true if component is added, false if it wasn't
		//Info: 
		KRAWLER_API bool addComponent(KComponentBase* pComponent);

		//Param: pointer to componenet to be removed 
		//Return: true if component removed, false if it wasn't
		//Info: 
		KRAWLER_API bool removeComponent(KComponentBase* pComponent);

		//Param: string containing new tag
		//Return: n/a
		//Info: 
		KRAWLER_API void setTag(const std::wstring& tag) { m_entityTag = tag; }

		//Param: Type of component being requested
		//Return: Pointer to the component requested, if not found will return nullptr
		//Info: Query by component types uses a dynamic_cast<> to evaluate whether a pointer of the requested type is 
		//stored.
		template<typename TComponent>
		TComponent* const getComponent();

		//Param: n/a 
		//Return: wstring tag of entity
		//Info:   
		KRAWLER_API const std::wstring& getTag() const { return m_entityTag; }

		//todo change to enabled
		KRAWLER_API bool isEntityActive() const { return m_bIsActive; }

		KRAWLER_API void setActive(bool bActive) { m_bIsActive = bActive; }

		KRAWLER_API  KDEPRECATED_FUNC(Components::KCTransform)* const getTransformComponent() { return m_pTransform; }

		KRAWLER_API void setEntityInteraction(EntitySceneInteractivity interactivity);


		KRAWLER_API EntitySceneInteractivity getInteractivity() const;
		
		KRAWLER_API Components::KCTransform* getTransform() const { return m_pTransform; }

	private:
		std::vector<KComponentBase*> m_componentVector;
		std::wstring m_entityTag;
		bool m_bIsActive;
		Components::KCTransform* m_pTransform = nullptr;
		EntitySceneInteractivity m_interactivity;
	};

	template<typename TComponent>
	inline TComponent * const KEntity::getComponent()
	{
		const auto result = std::find_if(m_componentVector.begin(), m_componentVector.end(), [](KComponentBase* pBase) -> bool
		{
			return dynamic_cast<TComponent*>(pBase) != nullptr;
		});

		if (result == m_componentVector.end())
		{
			return nullptr;
		}
		return dynamic_cast<TComponent*>(*result);
	}
}

#endif 