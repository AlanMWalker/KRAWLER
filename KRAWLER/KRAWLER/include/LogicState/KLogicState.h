#ifndef KLOGIC_STATE_H
#define KLOGIC_STATE_H

#include "Krawler.h"

namespace Krawler
{
	namespace LogicState
	{

		class KLogicStateDirector;

		struct KLogicStateInitialiser
		{
			bool bIsPhysicsEngineEnabled = true;
			std::wstring stateIdentifier = TEXT("State");
			KLogicStateDirector* pStateDirector = nullptr;
		};

		class KLogicState
		{
		public:

			//ctor & dtor
			KLogicState() = default;
			virtual ~KLogicState() = default;

			//Mutators
			virtual Krawler::KInitStatus setupState(const KLogicStateInitialiser&  initaliser);
			virtual void cleanupState();

			virtual void tick();

			//Proposed functions (yet undecided)
			//void onEnterState();
			//void onExitState();
			//void resetState();


			//Accessors
			std::wstring getStateIdentifier() const { return m_stateIdentifier; }
		protected:

			KLogicStateDirector* mp_stateDirector = nullptr;

		private:

			std::wstring m_stateIdentifier;
			bool mb_isPhysicsEnabled = true;
		};
	}
}
#endif 
