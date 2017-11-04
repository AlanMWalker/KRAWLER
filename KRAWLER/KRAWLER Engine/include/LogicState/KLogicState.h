#ifndef KLOGIC_STATE_H
#define KLOGIC_STATE_H

#include <string> 
#include <Windows.h>
#include "Krawler.h"
#include <vector>

namespace Krawler
{
	//forward declerations
	class KGameObject;
	namespace Physics
	{
		class KPhysicsScene;
	}
	namespace LogicState
	{

		class KLogicStateDirector;

		struct KLogicStateInitialiser
		{
			bool bIsPhysicsEngineEnabled = true;
			std::wstring stateIdentifier = TEXT("State-") + GenerateUUID();
			KLogicStateDirector* pStateDirector = nullptr;
		};

		class KLogicState
		{
		public:

			//ctor & dtor
			KRAWLER_API KLogicState() = default;
			KRAWLER_API virtual ~KLogicState();

			//Mutators
			KRAWLER_API virtual Krawler::KInitStatus setupState(const KLogicStateInitialiser&  initaliser);
			KRAWLER_API virtual void cleanupState();
			KRAWLER_API void fixedTick();
			KRAWLER_API virtual void tick();

			KRAWLER_API KGameObject* addGameObject(const Vec2f& size, bool render = true);

			//Proposed functions (yet undecided)
			//void onEnterState();
			//void onExitState();
			//void resetState();


			//Accessors
			KRAWLER_API std::wstring getStateIdentifier() const { return m_stateIdentifier; }

			void physicsLerp(float alpha);
		protected:

			KLogicStateDirector* mp_stateDirector = nullptr;
			Physics::KPhysicsScene* mp_physicsScene = nullptr;

		private:

			std::vector<KGameObject*> m_gameObjects;

			std::wstring m_stateIdentifier;
			bool mb_isPhysicsEnabled = true;
		};
	}
}
#endif 
