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
		//TI denoting TiledImport
		enum KTILayerTypes : int8
		{
			ObjectLayer,
			TileLayer
		};

		enum KTIPropertyTypes : int8
		{
			String,
			Int,
			Float,
			Bool,
			HexColour,
			File
		};

		enum KTIObjectTypes
		{
			Point,
			Rect,
			Ellipse
		};

		union KTIProperty
		{
			wchar_t type_string[MAX_PROPERTY_STRING_CHARS]; //Files will also be stored as str
			int type_int;
			float type_float;
			bool type_bool;
			Krawler::Colour type_colour;
		};

		using KTIPropertiesMap = std::map<std::wstring, KTIProperty>;
		using KTIPropertyTypesMap = std::map<std::wstring, KTIPropertyTypes>;

		struct KTIObject
		{
		};

		struct KTILayer
		{
			KTIObject * pObjectList;
			uint32 objectCount;
			std::wstring name;
			int x;
			int y;
			int* data = nullptr;
			KTIPropertiesMap propertiesMap;
			KTIPropertyTypesMap propertTypesMap;
			KTILayerTypes layerType;
		};

		struct KTITileset
		{

		};

		struct KTIMap
		{
			Krawler::int32 height;
			Krawler::int32 width;
			Krawler::int32 tileWidth;
			Krawler::int32 tileHeight;
			Krawler::int32 nextObjectID;

			std::wstring orientation;

			KTILayer* pLayers = nullptr;
			Krawler::uint32 layerCount;

			KTITileset* tilesets = nullptr;
			Krawler::uint32 tilesetsCount;

			KTIPropertiesMap properties;
			KTIPropertyTypesMap propertyTypes;

		};

		KTIMap* loadTiledJSONFile(const std::wstring filePath);

		void cleanupLevelMap(KTIMap* pMap);
	}
}

#endif 