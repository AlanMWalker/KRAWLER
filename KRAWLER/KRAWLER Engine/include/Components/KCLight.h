#pragma once

#include <Krawler.h>
#include <Components/KCRenderableBase.h>

namespace Krawler
{
	namespace Components
	{

		enum KLightType
		{ // Omly support point until others work
			Point,
			Directional
		};

		enum KLightCastType
		{
			StaticLightCasting,
			DynamicLightCasting
		};

		class KCLight : public KCRenderableBase
		{
		public:

			KCLight(KEntity* pEntity);
			~KCLight() = default;

			virtual KInitStatus init() override;
			virtual void tick() override; 
			
			virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;



		private:

			KLightType m_lightType; 
			KLightCastType m_castType;

		};
	}
}