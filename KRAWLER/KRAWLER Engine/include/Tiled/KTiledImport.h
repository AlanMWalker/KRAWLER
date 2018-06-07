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

		struct KTIMapBase
		{
			virtual ~KTIMapBase() = default;
			std::wstring name;
			float x;
			float y;
			float width;
			float height;
			KTIPropertiesMap propertiesMap;
			KTIPropertyTypesMap propertyTypesMap;
		};

		struct KTIObject : public KTIMapBase
		{
			int32 gid;
			int32 id;
			float rotation; //degrees
			KTIObjectTypes objectType;
		};

		struct KTILayer : public KTIMapBase
		{
			std::vector<KTIObject> objectsVector;
			std::vector<Krawler::int32> tileData; //1D array of tile guid's
			std::wstring name;
			float offsetX;
			float offsetY;
			// width = 0 for object layer, grid width for tile layer
			// height = 0 for object layer, grid height for tile layer
			KTILayerTypes layerType;
		};

		struct KTITileset : public KTIMapBase
		{
			int32 tileWidth;
			int32 tileHeight;
			int32 margin;
			int32 spacing;
			int32 firstGID;
			int32 tileCount;
		};

		struct KTIMap
		{
			int32 height;
			int32 width;
			int32 tileWidth;
			int32 tileHeight;
			int32 nextObjectID;

			std::wstring orientation;

			std::vector<KTILayer> layersVector;
			std::vector<KTITileset> tilesetVector;

			KTIPropertiesMap properties;
			KTIPropertyTypesMap propertyTypes;

		};

		KTIMap* loadTiledJSONFile(const std::wstring filePath);

		void cleanupLevelMap(KTIMap* pMap); // @Remember Cleanup function implementation 
	}
}

#endif 