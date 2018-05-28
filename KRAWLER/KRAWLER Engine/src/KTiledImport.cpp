#include "Tiled\KTiledImport.h"

#include <fstream>
#include "JSON\json.hpp"

#include <SFML\System\String.hpp>

using namespace Krawler;
using namespace Krawler::TiledImport;
using namespace std;

using json = nlohmann::json;

enum LayerTypes : int8
{
	ObjectLayer,
	TileLayer
};

static bool is_valid_level_map_type(const json& rootJson); //Is the 
static bool load_level_map_layer(const json& rootJson, KLevelMap* pMap);
static bool get_int_safe(int32& value, const string& name, const json& rootJson);
static LayerTypes get_layer_type(const json& layerJsonObj);

KLevelMap * Krawler::TiledImport::loadTiledJSONFile(const std::wstring filePath)
{
	KLevelMap* pMap = new KLevelMap;

	ifstream jsonFile;
	jsonFile.open(filePath, ios::in);

	if (jsonFile.fail())
	{
		return nullptr;
	}

	json rootObject;
	jsonFile >> rootObject;

	if (!is_valid_level_map_type(rootObject))
	{
		KPrintf(L"Invalid root object json type in in tiledmap %s\n", filePath.c_str());
		KFREE(pMap);
		return nullptr;
	}

	const bool load_status = load_level_map_layer(rootObject, pMap);

	if (!load_status)
	{
		KFREE(pMap);
		return nullptr;
	}
	jsonFile.close();
	rootObject.clear();
	return pMap;
}

bool is_valid_level_map_type(const json & rootJson)
{
	auto findResult = rootJson.find("type");
	if (findResult == rootJson.end())
	{
		return false;
	}
	const string p = (*findResult);
	return p == "map";
}

bool load_level_map_layer(const json& rootJson, KLevelMap* pMap)
{
	KCHECK(pMap);
	// map width
	if (!get_int_safe(pMap->width, "width", rootJson))
	{
		return false;
	}

	//map height
	if (!get_int_safe(pMap->height, "height", rootJson))
	{
		return false;
	}

	if (!get_int_safe(pMap->tileWidth, "tilewidth", rootJson))
	{
		return false;
	}

	if (!get_int_safe(pMap->tileHeight, "tileheight", rootJson))
	{
		return false;
	}

	_CrtDbgBreak();
	return true;
}

bool get_int_safe(int32 & value, const string & name, const json & rootJson)
{
	auto intProperty = rootJson.find(name);
	if (intProperty == rootJson.end())
	{
		return false;
	}

	if (!intProperty->is_number_integer())
	{
		return false;
	}
	value = *intProperty;
	return true;
}
