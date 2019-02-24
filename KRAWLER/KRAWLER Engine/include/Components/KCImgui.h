#pragma once

#include "Krawler.h"
#include "KComponent.h"
#include "KCRenderableBase.h"
#include <atomic>

namespace Krawler
{
	namespace Components
	{
		enum KImguiState : int8
		{
			DrawnLastFrame,
			Updated,
			BeginDone,
			BeginEndDone
		};

		class KCImgui : public KCRenderableBase
		{
		public:

			KRAWLER_API KCImgui(KEntity* pEntity);
			KRAWLER_API ~KCImgui();

			KRAWLER_API virtual KInitStatus init() override;
			KRAWLER_API virtual void tick() override;
			virtual void draw(sf::RenderTarget& rTarget, sf::RenderStates rStates) const override;

			KRAWLER_API virtual Krawler::Rectf getOnscreenBounds() const;

			KRAWLER_API void begin(const std::string& name);
			KRAWLER_API void end();
			KRAWLER_API virtual void postRenderEvent()override;


		private:

			//KImguiState m_imguiState = KImguiState::Updated;


			std::atomic_int_fast8_t m_imguiState= KImguiState::Updated;
		};
	}
}