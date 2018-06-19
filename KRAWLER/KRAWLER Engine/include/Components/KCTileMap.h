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
		enum KTileStateEnum : int32
		{
			Walkable,
			Slowdown,
			Impassable
		};

		/*
		With a split TileMap the map is drawn as individual horizontal lines
		each line may contain multiple layers rendered after another.

		*/
		struct KCHorizontalTileLine : KCRenderableBase
		{
			KCHorizontalTileLine()
				: KCRenderableBase(nullptr)
			{
			}

			virtual void draw(sf::RenderTarget& target, sf::RenderStates) const override;

			KRAWLER_API Krawler::Rectf getOnscreenBounds() const
			{
				return Rectf(pTransform->transformPoint(topLeft), Vec2f((float)horizontalGridSize * pTileset.tileWidth, (float)pTileset.tileHeight));
			};

			std::vector<sf::VertexBuffer> vertexBuffersByLayerVector;
			const sf::Transform* pTransform = nullptr;
			int32 horizontalGridSize;
			Vec2f topLeft;
			sf::Texture* pTexture;
			TiledImport::KTITileset pTileset;
		};

		class KCTileMapSplit : public KCRenderableBase
		{
		public:

			KRAWLER_API KCTileMapSplit(KEntity* pEntity, const std::wstring& tiledMapName);
			KRAWLER_API ~KCTileMapSplit() = default;

			KRAWLER_API virtual KInitStatus init() override;
			KRAWLER_API virtual void draw(sf::RenderTarget& target, sf::RenderStates) const override;
			KRAWLER_API const TiledImport::KTIMap* const getTiledMapImportData() const { return m_pTiledImportData; }

			KRAWLER_API virtual Rectf getOnscreenBounds() const override;
			KRAWLER_API const std::vector<KCHorizontalTileLine>& getHorizontalTileLines() const { return m_tileMapVec; }
		private:

			TiledImport::KTIMap* m_pTiledImportData;
			
			Vec2i m_gridDimensions;
			Vec2i m_tileDimensions;

			std::vector<KCHorizontalTileLine> m_tileMapVec;
			std::vector<sf::VertexBuffer> m_preDrawLayers;
			const KCTransform* m_pTransformComponent;
			sf::Texture* m_pTexture;


		};

		class KCTileMap : public KCRenderableBase
		{
		public:

			KRAWLER_API KCTileMap(KEntity* pEntity, const std::wstring& tiledMapName);
			KRAWLER_API ~KCTileMap() = default;

			virtual void draw(sf::RenderTarget&, sf::RenderStates) const override;
			KRAWLER_API Rectf getOnscreenBounds() const override { return Rectf{}; }
			KRAWLER_API virtual KInitStatus init() override;

			KRAWLER_API const TiledImport::KTIMap* const getTiledMapImportData() const { return m_pTiledImportData; }


		private:

			std::vector<sf::VertexBuffer> m_layerVertexBufferVector;
			std::vector<sf::Texture*> m_texturesVector;
			std::vector<KTileStateEnum> m_tileStates;

			TiledImport::KTIMap* m_pTiledImportData;
			const KCTransform* m_pTransformComponent;
			Vec2i m_gridDimensions;
			Vec2i m_tileDimensions;

		};
	}
}

#endif