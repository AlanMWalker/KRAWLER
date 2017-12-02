#ifndef KSTATE_LOGIC_UNIT_H
#define KSTATE_LOGIC_UNIT_H

#include "Krawler.h"
#include "GameObject\KGameObject.h"

#include <memory>

#define NO_LOGIC_UNIT_TAG KTEXT("no_tag")
#define CLASS_NAME_TO_TAG(CLASS) KTEXT(#CLASS)

namespace Krawler
{
	namespace LogicState
	{
		class KLogicState;
	}
	namespace SLU
	{
		class KStateLogicUnitAdministrator;

		class KStateLogicUnit
		{
		public:

			KRAWLER_API KStateLogicUnit(const std::wstring& unitTag, KStateLogicUnitAdministrator& rAdmin);
			KRAWLER_API virtual ~KStateLogicUnit();

			KRAWLER_API virtual KInitStatus initialiseUnit();
			KRAWLER_API virtual void tickUnit() = 0;
			KRAWLER_API virtual void fixedTickUnit() = 0;
			KRAWLER_API virtual void cleanupUnit() = 0;

			KRAWLER_API std::wstring getUnitTag() const { return m_unitTag; }
			KRAWLER_API bool hasUnitBeenInitialised() const { return m_bHasBeenInitialised; }

		protected:

			KRAWLER_API KStateLogicUnitAdministrator& getStateAdmin() { return mr_logicUnitAdmin; }

		private:

			std::wstring m_unitTag;
			bool m_bHasBeenInitialised = false;
			KStateLogicUnitAdministrator& mr_logicUnitAdmin;

		};

		class KGameObjectLogicUnit : public KStateLogicUnit
		{
		public:

			KRAWLER_API KGameObjectLogicUnit(const std::wstring& unitTag, KStateLogicUnitAdministrator& rAdmin);
			KRAWLER_API virtual ~KGameObjectLogicUnit();

			KRAWLER_API virtual KInitStatus initialiseUnit() override { return Success; }
			KRAWLER_API virtual void tickUnit() override {};
			KRAWLER_API virtual void fixedTickUnit() override {};
			KRAWLER_API virtual void cleanupUnit() override {};

			KRAWLER_API void setGameObject(KGameObject* pGameObj) { mp_gameObj = pGameObj; }

		protected:

			KRAWLER_API KGameObject& getGameObj() { return *mp_gameObj; }

		private:

			KGameObject* mp_gameObj;

		};

		class KStateLogicUnitAdministrator
		{
		public:

			KRAWLER_API KStateLogicUnitAdministrator(LogicState::KLogicState* pLogicState);
			KRAWLER_API ~KStateLogicUnitAdministrator();

			KRAWLER_API KInitStatus initAllUnits();
			KRAWLER_API void addUnit(KStateLogicUnit* pUnit);
			KRAWLER_API void removeUnit(KStateLogicUnit* pUnit);
			KRAWLER_API void cleanupAllUnits();

			KRAWLER_API void tickAllUnits();
			KRAWLER_API void fixedTickAllUnits();

			template<typename T = KStateLogicUnit>
			T* getStateLogicUnit(std::wstring tag = NO_LOGIC_UNIT_TAG) const;

			template<typename T = KStateLogicUnit>
			std::vector<T*> getStateLogicUnitsList(std::wstring tag = NO_LOGIC_UNIT_TAG) const;

			LogicState::KLogicState& getLogicState() { return *mp_logicState; }

		private:

			std::map<std::wstring, KStateLogicUnit*> m_logicUnits;
			LogicState::KLogicState* mp_logicState;
		};


		template<typename T>
		inline T* KStateLogicUnitAdministrator::getStateLogicUnit(std::wstring tag) const
		{
			if (tag == NO_LOGIC_UNIT_TAG)
			{
				// find by cast
				for (auto& mapPair : m_logicUnits)
				{
					auto ptr = dynamic_cast<T*>(mapPair.second);
					if (ptr != nullptr)
					{
						return ptr;
					}
				}
			}
			else
			{
				// find by tag
				auto pos = m_logicUnits.find(tag);
				if (pos != m_logicUnits.end())
				{
					return dynamic_cast<T*>((*pos).second);
				}
			}

			return nullptr;
		}

		template<typename T>
		inline std::vector<T*> KStateLogicUnitAdministrator::getStateLogicUnitsList(std::wstring tag) const
		{
			std::vector<T> results(0);
			if (tag == NO_LOGIC_UNIT_TAG)
			{
				// find by cast
				for (auto& mapPair : m_logicUnits)
				{
					T* ptr = dynamic_cast<T*>(mapPair.second);
					if (ptr != nullptr)
					{
						results.push_back(ptr);
					}
				}
			}
			else
			{
				// find by tag
				for (auto& mapPair : m_logicUnits)
				{
					if (mapPair.second->getUnitTag() == tag)
					{
						results.push_back(dynamic_cast<T*>(mapPair.second));
					}
				}
			}
			return results;
		}

	}
}
#endif
