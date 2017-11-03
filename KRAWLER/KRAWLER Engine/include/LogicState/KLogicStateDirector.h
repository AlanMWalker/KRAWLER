#ifndef KLOGIC_STATE_DIRECTOR
#define KLOGIC_STATE_DIRECTOR

#include "Krawler.h" 
#include "LogicState\KLogicState.h"

namespace Krawler
{
	namespace LogicState
	{
		using KLogicStateData = std::pair<std::wstring, KLogicState*>;
		class KLogicStateDirector
		{
		public:

			KLogicStateDirector() = default;
			~KLogicStateDirector() = default;

			void registerLogicState(KLogicState* plogicState, KLogicStateInitialiser* pInitialiser);
			void tickActiveLogicState();
			void setActiveLogicState(const std::wstring& identifier);
			void cleanupLogicStateDirector();


			bool doesLogicStateExist(const std::wstring& identifier) const;

		private:

			std::map<std::wstring, KLogicState*> m_logicStateMap;
			KLogicStateData m_currentState;
		};
	}
}
#endif 
