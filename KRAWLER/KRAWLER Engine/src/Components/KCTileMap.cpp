#include "Components\KCTileMap.h"
#include "Components\KCTransform.h"
#include "AssetLoader\KAssetLoader.h"

using namespace Krawler;
using namespace Krawler::Components;
using namespace Krawler::TiledImport;

/*
VertexBuffer usage static because tiledmaps are intended to be created at init and swapped out for other entities containing
a tiled map. Since swapping vertices should not be considered a positive habit with these.
*/

KCTileMap::KCTileMap(KEntity * pEntity, const std::wstring & tiledMapName)
	: KCRenderableBase(pEntity)
{
	setComponentTag(KTEXT("KCTiledMap"));
	m_pTransformComponent = getEntity()->getTransformComponent();
	m_pTiledImportData = KAssetLoader::getAssetLoader().getLevelMap(tiledMapName);

	KCHECK(m_pTiledImportData);// if there's no tiled map found assert
}

void KCTileMap::draw(sf::RenderTarget & rTarget, sf::RenderStates rStates) const
{
	rStates.transform *= m_pTransformComponent->getTransform();
	rStates.shader = getShader();
	rStates.texture = m_texturesVector.back();

	for (auto& vb : m_layerVertexBufferVector)
	{
		rTarget.draw(vb, rStates);
	}
}

KInitStatus Krawler::Components::KCTileMap::init()
{
	if (!m_pTiledImportData)
	{
		return KInitStatus::MissingResource;
	}

	std::vector<sf::Vertex> vertices;
	int32 tile_layer_count = 0;
	for (auto& layer : m_pTiledImportData->layersVector)
	{
		if (layer.layerType == KTILayerTypes::TileLayer)
		{
			++tile_layer_count;
		}
	}

	m_layerVertexBufferVector.resize(tile_layer_count);
	m_tileStates.resize(tile_layer_count, KTileStateEnum::Walkable);

	int32 layerIdx = 0;
	auto& tileset = m_pTiledImportData->tilesetVector[0];
	m_texturesVector.push_back(KAssetLoader::getAssetLoader().getTexture(tileset.name));


	for (auto& layer : m_pTiledImportData->layersVector) //for each layer that is found on this map 
	{
		if (layer.layerType == KTILayerTypes::ObjectLayer) // if it's an object layer, continue cause it's not rendered
		{
			continue;
		}

		const uint32 TOTAL_TILES = (uint32)(layer.width * layer.height);
		m_layerVertexBufferVector[layerIdx].setPrimitiveType(sf::PrimitiveType::Quads);
		m_layerVertexBufferVector[layerIdx].setUsage(sf::VertexBuffer::Usage::Static);
		m_layerVertexBufferVector[layerIdx].create(TOTAL_TILES * 4);
		std::vector<sf::Vertex> vertices(TOTAL_TILES * 4);
		const Vec2f TileDim((float)m_pTiledImportData->tileWidth, (float)m_pTiledImportData->tileHeight);


		for (uint32 i = 0; i < (uint32)layer.width; ++i)
		{
			for (uint32 j = 0; j < (uint32)layer.height; ++j)
			{
				const uint32 tileIdx = i + j * (uint32)layer.width;
				sf::Vertex* pQuad = &vertices[tileIdx * 4];


				pQuad[0].position = Vec2f(TileDim.x * i, TileDim.y * j);
				pQuad[1].position = Vec2f(TileDim.x * (i + 1), TileDim.y *  j);
				pQuad[2].position = Vec2f(TileDim.x *(i + 1), TileDim.y  *  (j + 1));
				pQuad[3].position = Vec2f(TileDim.x * i, TileDim.y *  (j + 1));

				if (layer.tileData[tileIdx] == 0)
				{
					pQuad[0].color = sf::Color::Transparent;
					pQuad[1].color = sf::Color::Transparent;
					pQuad[2].color = sf::Color::Transparent;
					pQuad[3].color = sf::Color::Transparent;
				}
				else
				{
					int32 texX, texY;
					const int32 localID = layer.tileData[tileIdx] - tileset.firstGID;
					texX = localID % (((int)tileset.width) / tileset.tileWidth);
					texY = localID / (((int)tileset.width) / tileset.tileWidth);
					pQuad[0].texCoords = Vec2f(texX * tileset.tileWidth, texY * tileset.tileHeight);
					pQuad[1].texCoords = Vec2f((texX + 1) * tileset.tileWidth, texY * tileset.tileHeight);
					pQuad[2].texCoords = Vec2f((texX + 1) * tileset.tileWidth, (texY + 1) * tileset.tileHeight);
					pQuad[3].texCoords = Vec2f(texX * tileset.tileWidth, (texY + 1) * tileset.tileHeight);
				}
				//Check if tile is blocked
				if (tileset.tilePropertiesMap.count(std::to_wstring(tileIdx)) > 0)
				{
					//tileset.tilePropertiesMap[std::to_wstring(tileIdx)]
					//KPrintf(KTEXT("Found blocked tile local id: %d \n"), tileIdx, );
				}

			}
		}

		m_layerVertexBufferVector[layerIdx].update(&vertices[0]);

		++layerIdx;

	}
	return KInitStatus::Success;
}
