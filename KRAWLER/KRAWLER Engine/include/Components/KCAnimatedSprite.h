#ifndef KCANIMATED_SPRITE
#define KCANIMATED_SPRITE

#include "KCRenderableBase.h"
#include "Animation\KAnimation.h"
#include "KCTransform.h"
#include <SFML\Graphics\VertexArray.hpp>

namespace Krawler
{
	namespace Components
	{
		class KCAnimatedSprite : public KCRenderableBase
		{
		public:

			KRAWLER_API KCAnimatedSprite(Krawler::KEntity *pEntity, Animation::KAnimation* pAnimation);
			KRAWLER_API ~KCAnimatedSprite() = default;

			KRAWLER_API virtual void draw(sf::RenderTarget& rTarget, sf::RenderStates rStates) const override;
			KRAWLER_API virtual void tick() override;


			KRAWLER_API virtual Rectf getOnscreenBounds() const override;

			KRAWLER_API void play();
			KRAWLER_API void pause();
			KRAWLER_API void stop();
			KRAWLER_API void setFrameIndex(Krawler::int32);

			KRAWLER_API void setAnimation(const std::wstring& animName);

			KRAWLER_API bool isRepeatingAnimation() const { return m_bRepeat; }
			KRAWLER_API void setRepeatingState(bool bRepeat) { m_bRepeat = bRepeat; }

		private:

			void updateTextureRect(const Rectf& texRect);
			void setupVertArray();

			Animation::KAnimation * m_pCurrentAnim = nullptr;
			KCTransform* m_pTransformComponent = nullptr;
			
			sf::VertexArray m_verts;

			float m_animTimer;
			Krawler::uint32 m_frameIdx;
			
			bool m_bIsPlaying;
			bool m_bRepeat;
		};
	}
}

#endif 