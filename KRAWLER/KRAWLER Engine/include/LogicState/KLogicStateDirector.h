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

			KInitStatus initaliseLogicStates();
			KRAWLER_API void registerLogicState(KLogicState* plogicState, KLogicStateInitialiser* pInitialiser);
			KRAWLER_API void tickActiveLogicState();
			void fixedTick();
			KRAWLER_API void setActiveLogicState(const std::wstring& identifier);
			KRAWLER_API void cleanupLogicStateDirector();


			KRAWLER_API bool doesLogicStateExist(const std::wstring& identifier) const;

			void physicsLerp(float alpha);

		private:

			std::map<std::wstring, KLogicState*> m_logicStateMap;
			std::map<std::wstring, KLogicStateInitialiser> m_logicStateInitialisers;
			KLogicStateData m_currentState;
		};
	}
}
#endif 
