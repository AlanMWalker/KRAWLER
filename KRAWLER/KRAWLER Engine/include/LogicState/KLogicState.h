#ifndef KLOGIC_STATE_H
#define KLOGIC_STATE_H

#include <string> 
#include <Windows.h>
#include "Krawler.h"

namespace Krawler
{
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
			KRAWLER_API virtual ~KLogicState() = default;

			//Mutators
			KRAWLER_API virtual Krawler::KInitStatus setupState(const KLogicStateInitialiser&  initaliser);
			KRAWLER_API virtual void cleanupState();

			KRAWLER_API virtual void tick();

			//Proposed functions (yet undecided)
			//void onEnterState();
			//void onExitState();
			//void resetState();


			//Accessors
			KRAWLER_API std::wstring getStateIdentifier() const { return m_stateIdentifier; }
		protected:

			KLogicStateDirector* mp_stateDirector = nullptr;

		private:

			std::wstring m_stateIdentifier;
			bool mb_isPhysicsEnabled = true;
		};
	}
}
#endif 
