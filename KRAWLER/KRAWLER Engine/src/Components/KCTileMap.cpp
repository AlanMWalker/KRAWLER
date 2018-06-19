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
	m_tileDimensions.x = m_pTiledImportData->tilesetVector[0].tileWidth;
	m_tileDimensions.y = m_pTiledImportData->tilesetVector[0].tileHeight;

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
		vertices.resize(TOTAL_TILES * 4);
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

// -------- KCTileMapSplit -------- 
KCTileMapSplit::KCTileMapSplit(KEntity * pEntity, const std::wstring & tiledMapName)
	: KCRenderableBase(pEntity)
{
	setComponentTag(KTEXT("KCTileMapSplit"));
	m_pTiledImportData = KAssetLoader::getAssetLoader().getLevelMap(tiledMapName);
	KCHECK(m_pTiledImportData);
	m_pTransformComponent = getEntity()->getTransformComponent();
	m_gridDimensions = Vec2i((int32)m_pTiledImportData->width, (int32)m_pTiledImportData->height);
	m_tileDimensions.x = m_pTiledImportData->tilesetVector[0].tileWidth;
	m_tileDimensions.y = m_pTiledImportData->tilesetVector[0].tileHeight;


}

KInitStatus KCTileMapSplit::init()
{
	if (!m_pTiledImportData)
	{
		return KInitStatus::MissingResource;
	}

	std::vector<sf::Vertex> vertices;
	int32 tile_layer_count = 0;
	for (auto& layer : m_pTiledImportData->layersVector)
	{
		if (layer.layerType == KTILayerTypes::TileLayer && layer.propertiesMap.find(KTEXT("pre_draw")) == layer.propertiesMap.end())
		{
			++tile_layer_count;
		}
	}
	m_pTexture = KAssetLoader::getAssetLoader().getTexture(m_pTiledImportData->tilesetVector[0].name);
	//default tile line struct state setup with map grid width & texture pointer 
	KCHorizontalTileLine defaultTileLine;
	defaultTileLine.pTileset = m_pTiledImportData->tilesetVector[0];
	defaultTileLine.pTexture = m_pTexture;
	defaultTileLine.pTransform = &m_pTransformComponent->getTransform();
	defaultTileLine.horizontalGridSize = m_pTiledImportData->width;
	defaultTileLine.vertexBuffersByLayerVector.resize(tile_layer_count);
	m_tileMapVec.resize(m_pTiledImportData->height, defaultTileLine);

	const int32 HorizontalLineVertexCount = 4 * m_pTiledImportData->width;
	vertices.resize(4 * m_pTiledImportData->width* m_pTiledImportData->height);

	int32 vbLayerIndex = 0;
	int32 verticesIndex = 0;
	auto& tileset = m_pTiledImportData->tilesetVector[0];

	for (auto& layer : m_pTiledImportData->layersVector)
	{
		if (layer.layerType != KTILayerTypes::TileLayer)
		{
			continue;
		}



		for (int j = 0; j < m_pTiledImportData->height; ++j)
		{
			verticesIndex = 0;
			for (int i = 0; i < m_pTiledImportData->width; ++i)
			{
				const int32 TileIndex = i + j * m_pTiledImportData->width;
				sf::Vertex* quad = &vertices[TileIndex * 4];//[verticesIndex];


				if (layer.tileData[TileIndex] == 0)
				{
					quad[0].position = Vec2f(0.0f, 0.0f);
					quad[1].position = Vec2f(0.0f, 0.0f);
					quad[2].position = Vec2f(0.0f, 0.0f);
					quad[3].position = Vec2f(0.0f, 0.0f);

					quad[0].color = sf::Color::Transparent;
					quad[1].color = sf::Color::Transparent;
					quad[2].color = sf::Color::Transparent;
					quad[3].color = sf::Color::Transparent;

				}
				else
				{
					quad[0].position = Vec2f(i * m_tileDimensions.x, j * m_tileDimensions.y);
					quad[1].position = Vec2f((i + 1) * m_tileDimensions.x, j * m_tileDimensions.y);
					quad[2].position = Vec2f((i + 1) * m_tileDimensions.x, (j + 1)* m_tileDimensions.y);
					quad[3].position = Vec2f(i * m_tileDimensions.x, (j + 1) * m_tileDimensions.y);

					int32 texX, texY;
					const int32 localID = layer.tileData[TileIndex] - tileset.firstGID;
					texX = localID % (((int)tileset.width) / tileset.tileWidth);
					texY = localID / (((int)tileset.width) / tileset.tileWidth);
					quad[0].texCoords = Vec2f(texX * tileset.tileWidth, texY * tileset.tileHeight);
					quad[1].texCoords = Vec2f((texX + 1) * tileset.tileWidth, texY * tileset.tileHeight);
					quad[2].texCoords = Vec2f((texX + 1) * tileset.tileWidth, (texY + 1) * tileset.tileHeight);
					quad[3].texCoords = Vec2f(texX * tileset.tileWidth, (texY + 1) * tileset.tileHeight);
				}

				verticesIndex += 4;
			}
			if (layer.propertiesMap.find(KTEXT("pre_render")) != layer.propertiesMap.end())
			{
				m_preDrawLayers.push_back(sf::VertexBuffer());
				m_preDrawLayers.back().create(vertices.size());
				m_preDrawLayers.back().setUsage(sf::VertexBuffer::Usage::Static);
				m_preDrawLayers.back().setPrimitiveType(sf::PrimitiveType::Quads);
				m_preDrawLayers.back().update(&vertices[0]);
			}
			else
			{
				m_tileMapVec[j].vertexBuffersByLayerVector[vbLayerIndex].setPrimitiveType(sf::PrimitiveType::Quads);
				m_tileMapVec[j].vertexBuffersByLayerVector[vbLayerIndex].setUsage(sf::VertexBuffer::Usage::Static);
				m_tileMapVec[j].vertexBuffersByLayerVector[vbLayerIndex].create(HorizontalLineVertexCount);
				m_tileMapVec[j].vertexBuffersByLayerVector[vbLayerIndex].update(&vertices[j * 4 * m_pTiledImportData->width]);
				m_tileMapVec[j].topLeft = Vec2f(0, j * m_pTiledImportData->tileHeight);
			}
		}
		++vbLayerIndex;
	}
	return KInitStatus::Success;
}

void KCTileMapSplit::draw(sf::RenderTarget & rTarget, sf::RenderStates rStates) const
{
	rStates.transform *= m_pTransformComponent->getTransform();
	rStates.texture = m_pTexture;
	rStates.shader = getShader();
	for (auto& layer : m_preDrawLayers)
	{
		rTarget.draw(layer, rStates);
	}

	return;
}

Rectf KCTileMapSplit::getOnscreenBounds() const
{
	Vec2f scaledSize;
	scaledSize.x = (float)(m_gridDimensions.x * m_tileDimensions.x);
	scaledSize.y = (float)(m_gridDimensions.y * m_tileDimensions.y);
	scaledSize.x *= m_pTransformComponent->getScale().x;
	scaledSize.y *= m_pTransformComponent->getScale().y;

	return Rectf(m_pTransformComponent->getTransform().transformPoint(0, 0), scaledSize);
}

void KCHorizontalTileLine::draw(sf::RenderTarget & rTarget, sf::RenderStates rStates) const
{
	rStates.transform *= (*pTransform);
	rStates.texture = pTexture;
	rStates.shader = getShader();
	for (auto it = vertexBuffersByLayerVector.begin(); it != vertexBuffersByLayerVector.end(); ++it)
	{
		rTarget.draw(*it, rStates);
	}
}
