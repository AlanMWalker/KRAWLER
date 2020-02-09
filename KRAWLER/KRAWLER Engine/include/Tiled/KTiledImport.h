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
		enum class KTILayerTypes : int8
		{
			ObjectLayer,
			TileLayer
		};
		// @Remember to add a failed enum flag
		enum class KTIPropertyTypes : int8
		{
			String,
			Int,
			Float,
			Bool,
			HexColour,
			File
		};

		enum class KTIObjectTypes : int8
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
			float x = 0.0f;
			float y = 0.0f;
			float width = 0.0f;
			float height = 0.0f;
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
			uint32 tileWidth = 0;
			uint32 tileHeight = 0;
			uint32 margin = 0;
			uint32 spacing = 0;
			uint32 firstGID = 0;
			uint32 tileCount = 0;
			std::map<std::wstring, KTIPropertiesMap> tilePropertiesMap;
			std::map<std::wstring, KTIPropertyTypesMap> tilePropertyTypesMap;
			//width & height represet image width & image height
		};

		struct KTIMap
		{
			uint32 height = 0;
			uint32 width = 0;
			uint32 tileWidth = 0;
			uint32 tileHeight = 0;
			uint32 nextObjectID = 0;

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