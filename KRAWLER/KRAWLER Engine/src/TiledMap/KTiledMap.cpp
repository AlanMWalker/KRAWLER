#include "TiledMap\KTiledMap.h"
#include "AssetLoader\KAssetLoader.h"
#include <SFML\Graphics\Texture.hpp>

#include <fstream>

using namespace Krawler;
using namespace Krawler::TiledMap;

using namespace std;
using namespace sf;

KTiledMap::KTiledMap()
	: mp_mapData(nullptr), m_vertArray(PrimitiveType::Quads)
{
}

void Krawler::TiledMap::KTiledMap::draw(sf::RenderTarget & rTarget, RenderStates rStates) const
{
	rStates.transform *= getTransform();
	rStates.texture = mp_texture;
	rStates.shader = m_pShader;
	rTarget.draw(m_vertArray, rStates);
}

void KTiledMap::cleanupTiledMap()
{
	KFREE_ARR(mp_mapData);

}

KTiledMapLoadResult KTiledMap::setupTiledMap(const std::wstring & filename)
{
	ifstream mapFile;

	if (filename.find(MAP_FILE_FORMAT) == wstring::npos)
	{
		return KTiledMapLoadResult::Map_Load_Fail_Invalid_Map_File;
	}

	mapFile.open(filename, ios::in);

	if (mapFile.fail())
	{
		return KTiledMapLoadResult::Map_Load_Fail_No_File_Found;
	}

	mapFile >> m_mapWidth;
	mapFile >> m_mapHeight;
	mapFile >> m_tileWidth;
	mapFile >> m_tileHeight;

	mp_mapData = new KMapTile[m_mapWidth * m_mapHeight];
	m_vertArray.resize(m_mapWidth * m_mapHeight * 4);

	for (int32 j = 0; j < m_mapHeight; ++j)
	{
		for (int32 i = 0; i < m_mapWidth; ++i)
		{
			const int index = i + j * m_mapWidth;

			int tileId = 0;

			mapFile >> tileId;
			if (!mapFile.eof())
			{
				mapFile.get();

			}

			Vertex* const vert = &m_vertArray[index * 4];

			vert[0].position = Vec2f((float)i  * m_tileWidth, (float)j * m_tileHeight);
			vert[1].position = Vec2f((float)(i + 1) * m_tileWidth, (float)j * m_tileHeight);
			vert[2].position = Vec2f((float)(i + 1)* m_tileWidth, (float)(j + 1)* m_tileHeight);
			vert[3].position = Vec2f((float)i  * m_tileWidth, (float)(j + 1)* m_tileHeight);

			mp_mapData[index].xPos = i;
			mp_mapData[index].yPos = j;
			KPrintf(KTEXT("TileID: %i\n"), tileId);
			switch (tileId)
			{
			default:
			case  0:
				vert[0].color = Color::Green;
				vert[1].color = Color::Green;
				vert[2].color = Color::Green;
				vert[3].color = Color::Green;
				mp_mapData[index].blocked = false;
				break;
			case 1:
				vert[0].color = Color(36, 38, 36);
				vert[1].color = Color(36, 38, 36);
				vert[2].color = Color(36, 38, 36);
				vert[3].color = Color(36, 38, 36);
				mp_mapData[index].blocked = true;
				break;
			}

		}
	}

	mapFile.close();
	return KTiledMapLoadResult::Map_Load_Success;
}

KRAWLER_API KTiledMapLoadResult Krawler::TiledMap::KTiledMap::setupTiledMapFromArray(int * tileIDs, Vec2i mapDimensions, Vec2i tileDimensions)
{
	m_mapWidth = mapDimensions.x;
	m_mapHeight = mapDimensions.y;
	m_tileWidth = tileDimensions.x;
	m_tileHeight = tileDimensions.y;;

	mp_mapData = new KMapTile[m_mapWidth * m_mapHeight];
	m_vertArray.resize(m_mapWidth * m_mapHeight * 4);

	for (int32 j = 0; j < m_mapHeight; ++j)
	{
		for (int32 i = 0; i < m_mapWidth; ++i)
		{
			const int index = i + j * m_mapWidth;

			int tileId = 0;

			tileId = tileIDs[index];

			Vertex* const pQuad = &m_vertArray[index * 4];

			pQuad[0].position = Vec2f((float)i  * m_tileWidth, (float)j * m_tileHeight);
			pQuad[1].position = Vec2f((float)(i + 1) * m_tileWidth, (float)j * m_tileHeight);
			pQuad[2].position = Vec2f((float)(i + 1)* m_tileWidth, (float)(j + 1)* m_tileHeight);
			pQuad[3].position = Vec2f((float)i  * m_tileWidth, (float)(j + 1)* m_tileHeight);

			mp_mapData[index].xPos = i;
			mp_mapData[index].yPos = j;

			int tu = tileId % (mp_texture->getSize().x / m_tileWidth);
			int tv = tileId / (mp_texture->getSize().x / m_tileWidth);

			pQuad[0].texCoords = sf::Vector2f(tu * m_tileWidth, tv * m_tileHeight);
			pQuad[1].texCoords = sf::Vector2f((tu + 1) * m_tileWidth, tv * m_tileHeight);
			pQuad[2].texCoords = sf::Vector2f((tu + 1) * m_tileWidth, (tv + 1) * m_tileHeight);
			pQuad[3].texCoords = sf::Vector2f(tu * m_tileWidth, (tv + 1) * m_tileHeight);

			pQuad[0].color = Color::Magenta;
			pQuad[1].color = Color::Magenta;
			pQuad[2].color = Color::Magenta;
			pQuad[3].color = Color::Magenta;
		}
	}


	return KTiledMapLoadResult::Map_Load_Success;
}

void Krawler::TiledMap::KTiledMap::setTexture(const std::wstring & textureFileName)
{
	mp_texture = KAssetLoader::getAssetLoader().loadTexture(textureFileName);
	//mp_texture->setSmooth(true);
}
