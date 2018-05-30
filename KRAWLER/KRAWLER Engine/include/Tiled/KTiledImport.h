#ifndef KTILED_IMPORT_STRUCTS_H
#define KTILED_IMPORT_STRUCTS_H

#include <Krawler.h>
#include <string>
#include <map>

namespace Krawler
{
	namespace TiledImport
	{
		enum KLayerTypes : int8
		{
			ObjectLayer,
			TileLayer
		};

		enum KPropertyTypes : int8
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
			KLevelMapProperty() {};
			wchar_t* type_string;
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
			std::map<std::wstring, KPropertyTypes> propertyTypes;

		};

		KLevelMap* loadTiledJSONFile(const std::wstring filePath);
	}
}

#endif 