#ifndef KCTILE_MAP_H
#define KCTILE_MAP_H

#include "Krawler.h"
#include "KComponent.h"
#include "KCRenderableBase.h"
#include "KEntity.h"

#include "Tiled\KTiledImport.h"

#include <vector>
#include <SFML\Graphics\VertexBuffer.hpp>

namespace Krawler
{
	namespace Components
	{
		
		class KCTileMap : public KCRenderableBase
		{
		public:

			KRAWLER_API KCTileMap(KEntity* pEntity, const std::wstring& tiledMapName);
			KRAWLER_API ~KCTileMap() = default;

			virtual void draw(sf::RenderTarget&, sf::RenderStates) const override;
			KRAWLER_API Rectf getOnscreenBounds() const override { return Rectf{}; }
			KRAWLER_API virtual KInitStatus init() override;

			KRAWLER_API const TiledImport::KTIMap* const getTiledMapImportData() const { return m_pTiledImportData; }

			enum KTileStateEnum : int32
			{
				Walkable,
				Slowdown,
				Impassable
			};


		private:

			std::vector<sf::VertexBuffer> m_layerVertexBufferVector;
			std::vector<sf::Texture*> m_texturesVector;
			std::vector<KTileStateEnum> m_tileStates;

			TiledImport::KTIMap* m_pTiledImportData;
			const KCTransform* m_pTransformComponent;

		};
	}
}

#endif