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

			KRAWLER_API KLogicStateDirector() = default;
			KRAWLER_API ~KLogicStateDirector() = default;

			KRAWLER_API void registerLogicState(KLogicState* plogicState, KLogicStateInitialiser* pInitialiser);
			KRAWLER_API void tickActiveLogicState();
			KRAWLER_API void setActiveLogicState(const std::wstring& identifier);
			KRAWLER_API void cleanupLogicStateDirector();


			KRAWLER_API bool doesLogicStateExist(const std::wstring& identifier) const;

		private:

			std::map<std::wstring, KLogicState*> m_logicStateMap;
			KLogicStateData m_currentState;
		};
	}
}
#endif 
