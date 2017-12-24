#ifndef KCOMPONENT_H
#define KCOMPONENT_H

#include "Krawler.h"

namespace Krawler
{
	//Forward declarations 
	class KEntity;

	/*Component base class -- Top of all component hierachy.
	To create a custom component extend from this class.*/
	class KComponentBase
	{
	public:


		//Param: KEntity* 
		//Return: n/a
		//Info: Construct component with a pointer to entity it is attatched to 
		KRAWLER_API KComponentBase(KEntity* pEntity);
		KRAWLER_API virtual ~KComponentBase();

		//Param: n/a
		//Return: KInit status for initialation result
		//Info: Init component (to be overridden if a component requires delayed initialisation)
		KRAWLER_API virtual KInitStatus init();

		//Param: n/a
		//Return: KInit 
		//Info: Cleanup component (to be overridden if a component requires own cleanup routine)
		KRAWLER_API virtual void cleanUp();

		//Param: n/a
		//Return: n/a 
		//Info: Fixed tick component (to be overridden if a component interacts with the physics engine)
		KRAWLER_API virtual void fixedTick();

		//Param: n/a
		//Return: n/a 
		//Info: Tick component (to be overridden if a component updates)
		KRAWLER_API virtual void tick();

		//Param: n/a
		//Return: n/a 
		//Info: onEnterScene setup component (to be overridden if a component updates)
		KRAWLER_API virtual void onEnterScene(); 
		

		//Param: n/a
		//Return: n/a 
		//Info: onExitScene setup cleanup component (to be overridden if a component updates)
		KRAWLER_API virtual void onExitScene(); // TODO requires scene class!!

		//Param: wstring tag 
		//Return: n/a
		//Info: git
		KRAWLER_API void setComponentTag(const std::wstring& tag) { m_componentTag = tag; }

		//Param: n/a
		//Return: n/a 
		//Info: Tick component (to be overridden if a component updates)
		KRAWLER_API const std::wstring& getComponentTag() const { return m_componentTag; }

	protected:

		//Param: n/a
		//Return: KEntity pointer 
		//Info: Allows derived access to a pointer to the component base
		KRAWLER_API KEntity* const getEntity() { return m_pEntity; }

	private:

		KEntity* const m_pEntity;
		std::wstring m_componentTag;
	};
}

#endif 