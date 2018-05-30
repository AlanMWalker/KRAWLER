#ifndef KTILED_IMPORT_STRUCTS_H
#define KTILED_IMPORT_STRUCTS_H

#include <Krawler.h>
#include <string>
#include <map>

#define MAX_PROPERTY_STRING_CHARS 3000

namespace Krawler
{
	namespace TiledImport
	{
		enum KLayerTypes : int8
		{
			ObjectLayer,
			TileLayer
		};

		enum KLevelMapPropertyTypes : int8
		{
			String,
			Int,
			Float,
			Bool,
			HexColour,
			File
		};

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

		union KLevelMapProperty
		{
			wchar_t type_string[MAX_PROPERTY_STRING_CHARS]; //Files will also be stored as str
			int type_int;
			float type_float;
			bool type_bool;
			Krawler::Colour type_colour;
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

			std::map<std::wstring, KLevelMapProperty> properties;
			std::map<std::wstring, KLevelMapPropertyTypes> propertyTypes;

		};

		KLevelMap* loadTiledJSONFile(const std::wstring filePath);

		void cleanupLevelMap(KLevelMap* pMap);
	}
}

#endif 