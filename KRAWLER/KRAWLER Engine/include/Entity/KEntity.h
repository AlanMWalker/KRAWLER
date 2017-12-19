#ifndef KENTITY_H
#define KENTITY_H

#include <vector>

#include "Krawler.h"
#include "Component\KComponent.h"

namespace Krawler
{
	/*Entity class which components are attached to.
	All entities automatically have a transform component attatched upon construction.*/

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

		// void onEnterScene(); 
		// void onExitScene();

		//Param: pointer to componenet to be added
		//Return: true if component is added, false if it wasn't
		//Info: 
		KRAWLER_API bool addComponent(KComponentBase* pComponent);

		//Param: pointer to componenet to be removed 
		//Return: true if component removed, false if it wasn't
		//Info: 
		KRAWLER_API bool removeComponent(KComponentBase* pComponent); 

		//Param: Type of component being requested
		//Return: Pointer to the component requested, if not found will return nullptr
		//Info: Query by component types uses a dynamic_cast<> to evaluate whether a pointer of the requested type is 
		//stored.
		template<typename TComponent>
		KRAWLER_API TComponent* const getComponent();

		
	private:
		std::vector<KComponentBase*> m_componentVector; 
		std::wstring m_entityTag;
	};
	
	template<typename TComponent>
	inline TComponent * const KEntity::getComponent()
	{
		// Early out if TComponent == transform
		for (auto& pComp : m_componentVector)
		{
			auto pTComp = dynamic_cast<TComponent*>(pComp);
			if (pTComp == nulltpr)
			{
				continue;
			}
			else
			{
				return pTComp;
			}
		}

		return nullptr;
	}
}

#endif 