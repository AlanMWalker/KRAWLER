#ifndef KTILED_IMPORT_STRUCTS_H
#define KTILED_IMPORT_STRUCTS_H

#include <Krawler.h>
#include <string>
#include <map>

namespace Krawler
{
	namespace TiledImport
	{
		struct KLevelMapObjectBox
		{

		};

		struct KLevelMapObjectCircle
		{

		};

		struct KLevelMapObjectPoint
		{

		};

		struct KLevelMapObjectLayer
		{

		};

		struct KLevelMapTileLayer
		{

		};

		struct KLevelMapTileset
		{

		};

		struct KLevelMap
		{
			Krawler::int32 height;
			Krawler::int32 width;
			Krawler::int32 tileWidth;
			Krawler::int32 tileHeight;
			Krawler::int32 nextObjectID;
			std::wstring orientation;
			KLevelMapTileLayer* layers = nullptr;
			Krawler::uint32 layersCount;
			KLevelMapTileset* tilesets = nullptr;
			Krawler::uint32 tilesetsCount;

			std::map<std::wstring, std::wstring> properties;
			std::map<std::wstring, std::wstring> propertyTypes;

		};

		KLevelMap* loadTiledJSONFile(const std::wstring filePath);
	}
}

#endif 