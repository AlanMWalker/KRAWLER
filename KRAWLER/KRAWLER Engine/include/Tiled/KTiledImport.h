#ifndef KTILED_IMPORT_STRUCTS_H
#define KTILED_IMPORT_STRUCTS_H

#include <Krawler.h>
#include <string>
#include <map>
#include <vector>

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
		// @Remember to add a failed enum flag
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
			Circle,
			Polygon //@Remember Polygon not currently supported
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
			std::wstring name;
			Krawler::int32 gid;
			Krawler::int32 id;
			float height;
			float width;
			float x;
			float y;
			float rotation; //degrees
			KTIPropertiesMap propertiesMap;
			KTIPropertyTypesMap propertyTypesMap;
			KTIObjectTypes objectType;
		};

		struct KTILayer
		{
			std::vector<KTIObject> objectsVector;
			std::vector<Krawler::int32> tileData; //1D array of tile guid's
			std::wstring name;
			float x;
			float y;
			float offsetX;
			float offsetY;
			float width; // 0 for object layer, grid width for tile layer
			float height; // 0 for object layer, grid height for tile layer
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

			std::vector<KTILayer> layersVector;

			KTITileset* tilesets = nullptr;
			Krawler::uint32 tilesetsCount;

			KTIPropertiesMap properties;
			KTIPropertyTypesMap propertyTypes;

		};

		KTIMap* loadTiledJSONFile(const std::wstring filePath);

		void cleanupLevelMap(KTIMap* pMap); // @Remember Cleanup function implementation 
	}
}

#endif 