#ifndef KENTITY_H
#define KENTITY_H

#include <vector>

#include "Krawler.h"
#include "KComponent.h"

namespace Krawler
{
	/*Entity class which components are attached to.
	All entities automatically have a transform component attatched upon construction.*/

	const int32 MAX_NUMBER_OF_ENTITIES{ 120 };

	class KEntity
	{
	public:

		KRAWLER_API KEntity();
		KRAWLER_API ~KEntity() = default;

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
		KRAWLER_API void setEntityTag(const std::wstring& tag) { m_entityTag = tag; }

		//Param: Type of component being requested
		//Return: Pointer to the component requested, if not found will return nullptr
		//Info: Query by component types uses a dynamic_cast<> to evaluate whether a pointer of the requested type is 
		//stored.
		template<typename TComponent>
		TComponent* const getComponent();

		//Param: n/a 
		//Return: wstring tag of entity
		//Info:   
		KRAWLER_API const std::wstring& getEntityTag() const { return m_entityTag; }

		//todo change to enabled
		KRAWLER_API bool isEntityInUse() const { return m_bIsInUse; }

		KRAWLER_API void setIsInUse(bool bInUse) { m_bIsInUse = bInUse; }

	private:
		std::vector<KComponentBase*> m_componentVector;
		std::wstring m_entityTag;
		bool m_bIsInUse;
	};

	template<typename TComponent>
	inline TComponent * const KEntity::getComponent()
	{
		const auto result = std::find_if(m_componentVector.begin(), m_componentVector.end(), [](KComponentBase* pBase) -> bool
		{
			return dynamic_cast<TComponent*>(pBase) != nullptr;
		});

		if (result == m_componentVector.end())
			return nullptr;

		return dynamic_cast<TComponent*>(*result);
	}
}

#endif 