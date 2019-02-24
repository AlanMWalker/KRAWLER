#include "Tiled\KTiledImport.h"

#include <fstream>
#include <sstream>
#include <wchar.h> 

#include "JSON\json.hpp"

#include <SFML\System\String.hpp>

using namespace Krawler;
using namespace Krawler::TiledImport;
using namespace std;

using json = nlohmann::json;

/*
@Remember String Conversion Method
json.hpp libray uses std::string, while the internals of KRAWLER use std::wstring. Therefore a horrid workaround
is to use sf::String(string_object).toWideString().
*/
//to wide string conversion via SFML
#define TO_WSTR(str) sf::String(str).toWideString()

#define MAP_PARSE_ERR KPRINTF("JSON PARSE ERROR! ")
#define DOES_ELEMENT_EXIST(name, jsonObj) (jsonObj.count(name) > 0)
#define GET_NUMBER_FLOAT(jsonElement, name, floatVar) \
if (jsonElement[name].is_number_integer())\
{\
	floatVar = (float)jsonElement[name].get<int32>();\
}\
else if( jsonElement[name].is_number_unsigned()) \
{\
	floatVar = (float)jsonElement[name].get<uint32>();\
}\
else \
{\
	floatVar = jsonElement[name].get<float>();\
}\

#define GET_NUMBER_INT(jsonElement, name, intVar) \
if (jsonElement[name].is_number_integer())\
{\
	intVar = jsonElement[name].get<int32>();\
}\
else if( jsonElement[name].is_number_unsigned()) \
{\
	intVar = (signed)jsonElement[name].get<uint32>();\
}\
else \
{\
	intVar = (int)jsonElement[name].get<float>();\
}\

// --- FUNCTION DECLERATIONS --- \\

//Desc: Is the the map file valid in terms of type (has a property called 'type' with a value of 'map') 
//Params: map json object
//Return: true if valid, false if invalid
static bool is_valid_level_map_type(const json& rootJson);

//Desc: Is there a layers array attached to the object
//Params: map json object
//Return: true if present, false if not
static bool are_map_layers_present(const json& rootJson);

//Desc: Load tiled map meta data, and handle all layer extraction & tileset data
//Params: json object of map, KTIMap pointer
//Return: true if successful, false if failed
static bool load_tiled_map(const json& rootJson, KTIMap* pMap);

//Desc: get a string value if it present on a json object
//Params: string ref of variable to be set, name of json object, json object to extract from
//Return: true if set, false if not
static bool get_string_if_present(wstring& value, const string& name, const json& jsonObj);

//Desc: get a int value if it present on a json object
//Params: float ref of variable to be set, name of json object, json object to extract from
//Return: true if set, false if not
static bool get_int_if_present(int32& value, const string& name, const json& rootJson);

//Desc: get a float value if it present on a json object
//Params: float ref of variable to be set, name of json object, json object to extract from
//Return: true if set, false if not
static bool get_float_if_present(float& value, const string& name, const json& rootJson);

//Desc: Extract properties from any json node that has properties
//Params: json object, KTIPropertiesMap ref, KTIPropertyTypesMap ref
//Return: N/A
static void extract_properties_to_map(const json& jsonObj, KTIPropertiesMap & propMap, KTIPropertyTypesMap&  typeMap);

//Desc: Extract tiled and object layers and add them to a KTIMap
//Params: json object (containing layers array member), pointer to KTI map
//Return: true if successful, false if not
static bool extract_map_layers(const json& jsonObj, KTIMap* pMap);

//Desc: Returns enum of the type of a property based on its name
//Params: string of name
//Return: KTIPropertyTypes enum of property type (will default to string if not known) 
static KTIPropertyTypes get_property_type_by_string(const std::wstring& name);

//Desc: Returns enum type of the layer 
//Params: json object which contains a field 'type' for layer type
//Return: KTILayerTypes enum 
static KTILayerTypes get_layer_type(const json& layerJsonObj);

//Desc: Is the object we've identified a template of an object
//Params: json obj
//Return: true if is template, false if not
static bool is_template_object(const json& mapObjectJson);

//Desc: Fill out a KTILayer object with the data for an Tiled 'object layer' 
//Params: json object (containing object layer data), ptr to KTILayer
//Return: N/A
static void extract_object_layer_data(const json& objectLayerJson, KTILayer* pObjLayerData);

//Desc: Fill out the relevant fields of a KTIObject
//Params: json object (containing Tiled object data), ptr to KTIObject
//Return: N/A
static void extract_object_data(const json & objectsArray, KTIObject * pObj);

//Desc: 
//Params:
//Return:
static void extract_tile_layer_data(const json& tileLayerJson, KTILayer* pTileLayerData);

//Desc: 
//Params:
//Return:
static bool extract_tile_sets(const json& tileLayerJson, KTIMap* pMap);

//Desc: 
//Params:
//Return:
static bool extract_singular_tileset(const json& tilesetJson, KTITileset* pTilesetData);

//Desc: 
//Params:
//Return:
static void extract_tile_properties(const json& jsonObj, std::map<std::wstring, KTIPropertiesMap>& tilePropMap, std::map<std::wstring, KTIPropertyTypesMap>&);

//--- PUBLIC FUNCTION DEFINITIONS --- \\

KTIMap * Krawler::TiledImport::loadTiledJSONFile(const std::wstring filePath)
{
	KTIMap* pMap = new KTIMap;

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

	const bool load_status = load_tiled_map(rootObject, pMap);

	if (!load_status)
	{
		KFREE(pMap);
		return nullptr;
	}
	jsonFile.close();
	rootObject.clear();
	return pMap;
}

void Krawler::TiledImport::cleanupLevelMap(KTIMap * pMap)
{
	pMap->layersVector.clear();
	pMap->tilesetVector.clear();
	KFREE(pMap);
}

// -- STATIC FUNCTION DEFINITIONS  -- \\

bool is_valid_level_map_type(const json & rootJson)
{
	auto findResult = rootJson.count("type");
	if (findResult == 0)
	{
		return false;
	}
	const string&& p = rootJson["type"].get<string>();
	return p == "map";
}

bool are_map_layers_present(const json & rootJson)
{
	return rootJson.count("layers") > 0;
}

bool load_tiled_map(const json& rootJson, KTIMap* pMap)
{
	KCHECK(pMap);

	if (!are_map_layers_present(rootJson))
	{
		MAP_PARSE_ERR;
		KPRINTF("No layers attached to map!\n");
		return false;
	}

	if (!get_int_if_present(pMap->width, "width", rootJson))
	{
		MAP_PARSE_ERR;
		KPRINTF("Failed to find 'width' field on map!\n");
		return false;
	}

	if (!get_int_if_present(pMap->height, "height", rootJson))
	{
		MAP_PARSE_ERR;
		KPRINTF("Failed to find 'height' field on map!\n");
		return false;
	}

	if (!get_int_if_present(pMap->tileWidth, "tilewidth", rootJson))
	{
		MAP_PARSE_ERR;
		KPRINTF("Failed to find 'tilewidth' field on map!\n");
		return false;
	}

	if (!get_int_if_present(pMap->tileHeight, "tileheight", rootJson))
	{
		MAP_PARSE_ERR;
		KPRINTF("Failed to find 'tileheight' field on map!\n");
		return false;
	}

	if (!get_int_if_present(pMap->nextObjectID, "nextobjectid", rootJson))
	{
		MAP_PARSE_ERR;
		KPRINTF("Failed to find 'nextobjectid' field on map!\n");
		return false;
	}

	extract_properties_to_map(rootJson, pMap->properties, pMap->propertyTypes);

	if (!extract_map_layers(rootJson, pMap))
	{
		return false;
	}

	if (!extract_tile_sets(rootJson, pMap))
	{
		return false;
	}

	return true;
}

bool get_int_if_present(int32 & value, const string & name, const json & rootJson)
{
	auto intProperty = rootJson.count(name);
	if (intProperty == 0)
	{
		return false;
	}

	auto result = rootJson[name];

	if (!result.is_number_integer())
	{
		return false;
	}
	value = result.get<int>();
	return true;
}

bool get_float_if_present(float & value, const string & name, const json & rootJson)
{
	auto floatProp = rootJson.count(name);
	if (floatProp == 0)
	{
		return false;
	}

	auto result = rootJson[name];

	if (!result.is_number_float())
	{
		return false;
	}
	value = result.get<float>();
	return true;
}

void extract_properties_to_map(const json& jsonObj, KTIPropertiesMap & propMap, KTIPropertyTypesMap &  typeMap)
{
	auto property_obj_it = jsonObj.find("properties");
	auto property_types_it = jsonObj.find("propertytypes");

	if (property_obj_it == jsonObj.end() || property_types_it == jsonObj.end())
	{ // not always likely to find properties, so return true
		return;
	}

	//Get total number of properties 
	const int32 Property_Count = property_obj_it->size();
	const int32 Type_Count = property_types_it->size();

	//Make sure total number of properties matches number of types
	if (Property_Count != Type_Count)
	{
		MAP_PARSE_ERR;
		KPrintf(KTEXT("Mismatch between number of map properties (%d) and propertytypes (%d)\n"), Property_Count, Type_Count);
		return;
	}

	//@UrgentRefactor switch statement to do function calls, and also isolate out the functionality to isolate properties so its generic for other map object types which will have properties.
	json::value_type properties_obj = *property_obj_it;
	json::value_type propertyTypes_obj = *property_types_it;

	json::iterator itProperties = properties_obj.begin(), itPropertyTypes = propertyTypes_obj.begin();

	//iterate through all properties & property-types objects
	while (itProperties != properties_obj.end() && itPropertyTypes != propertyTypes_obj.end())
	{
		const std::wstring Type_Name = sf::String(itPropertyTypes.value().get<string>()).toWideString();
		const KTIPropertyTypes Type_Enum = get_property_type_by_string(Type_Name); //isolate the data type of this property 

		KTIProperty mapPropertyUnion{ 0 };
		bool bLoadedCorrectly = true;
		std::wstring key, valueWideStr, type;
		key = sf::String(itProperties.key()).toWideString();
		switch (Type_Enum)
		{
		case String:
		{
			valueWideStr = sf::String(itProperties.value().get<string>()).toWideString();
			if (valueWideStr.size() < MAX_PROPERTY_STRING_CHARS)
			{
				wcsncpy_s(mapPropertyUnion.type_string, valueWideStr.c_str(), valueWideStr.size());
				//propMap->propertyTypes.emplace(key, Type_Enum);
				propMap.emplace(key, mapPropertyUnion);
			}
			else
			{
				bLoadedCorrectly = false;
				MAP_PARSE_ERR;
				KPRINTF_A("Unable to load string property from JSON as string is too large!: %s\n", valueWideStr.c_str());
			}
		}
		break;

		case Int:
		{
			mapPropertyUnion.type_int = itProperties.value().get<int>();
			propMap.emplace(key, mapPropertyUnion);
		}
		break;

		case Float:
		{
			mapPropertyUnion.type_float = itProperties.value().get<float>();
			propMap.emplace(key, mapPropertyUnion);
		}
		break;

		case Bool:
		{
			mapPropertyUnion.type_bool = itProperties.value().get<bool>();
			propMap.emplace(key, mapPropertyUnion);
		}
		break;

		case HexColour:
		{
			//@Rethink Horrible way of parsing hex value of colours 
			key = sf::String(itProperties.key()).toWideString();
			valueWideStr = sf::String(itProperties.value().get<string>()).toWideString();

			/*
			Colours are represented by tiled as
			# ALPHA_BITS RED_BITS BLUE_BITS GREEN_BITS
			Example
			#ff2d0044 => # FF 2d 00 44 (or 45,0,68,255 as RGBA)

			To parse this correctly 4 strings contain each channels hex string representation of the value.
			This is then put through a stringstream to allow conversion directly to int
			*/

			std::wstring alphaChannelChars, redChannelHexChars, greenChannelHexChars, blueChannelHexChars;
			alphaChannelChars.push_back(valueWideStr[1]);
			alphaChannelChars.push_back(valueWideStr[2]);

			redChannelHexChars.push_back(valueWideStr[3]);
			redChannelHexChars.push_back(valueWideStr[4]);

			greenChannelHexChars.push_back(valueWideStr[5]);
			greenChannelHexChars.push_back(valueWideStr[6]);

			blueChannelHexChars.push_back(valueWideStr[7]);
			blueChannelHexChars.push_back(valueWideStr[8]);

			int32 AlphaValue, RedValue, BlueValue, GreenValue;
			wstringstream ss;
			ss << std::hex << alphaChannelChars;
			ss >> AlphaValue;
			ss.clear();

			ss << std::hex << redChannelHexChars;
			ss >> RedValue;
			ss.clear();

			ss << std::hex << blueChannelHexChars;
			ss >> BlueValue;
			ss.clear();

			ss << std::hex << greenChannelHexChars;
			ss >> GreenValue;
			ss.clear();
			mapPropertyUnion.type_colour = Colour((int8)RedValue, (int8)GreenValue, (int8)BlueValue, (int8)AlphaValue);

			propMap.emplace(key, mapPropertyUnion);
			break;
		}

		case File:
		{
			bLoadedCorrectly = false;
			MAP_PARSE_ERR;
			KPRINTF("File property not supported in this parser!\n");
		}
		break;

		default:
			break;
		}

		if (bLoadedCorrectly)
		{
			typeMap.emplace(key, Type_Enum);
		}
		++itProperties;
		++itPropertyTypes;
	}
}

bool extract_map_layers(const json & jsonObj, KTIMap * pMap)
{
	KCHECK(pMap);
	auto layersObject = jsonObj["layers"];

	KCHECK(layersObject.is_array()); // layers must always be an array 

	const int32 LayerCount = layersObject.size();

	// No layers == invalid file 
	if (LayerCount == 0)
	{
		MAP_PARSE_ERR;
		KPRINTF("No layers found on tiledmap file!\n");
		return false;
	}

	pMap->layersVector.resize(LayerCount);

	for (uint32 i = 0; i < LayerCount; ++i)
	{
		auto& individual_layer_struct = pMap->layersVector[i];
		pMap->layersVector[i].layerType = get_layer_type(layersObject[i]);

		if (!get_string_if_present(individual_layer_struct.name, "name", layersObject[i]))
		{
			MAP_PARSE_ERR;
			KPRINTF("No name found on tiledmap layer!\n");
			continue;
		}

		//float handle positions
		if (!DOES_ELEMENT_EXIST("x", layersObject[i]))
		{
			MAP_PARSE_ERR;
			KPRINTF("No x position found on tiledmap layer!\n");
		}

		GET_NUMBER_FLOAT(layersObject[i], "x", individual_layer_struct.x);

		if (!DOES_ELEMENT_EXIST("y", layersObject[i]))
		{
			MAP_PARSE_ERR;
			KPRINTF("No y position found on tiledmap layer!\n");
		}

		GET_NUMBER_FLOAT(layersObject[i], "y", individual_layer_struct.y);

		//handle offsets
		if (DOES_ELEMENT_EXIST("offsetx", layersObject[i]))
		{
			GET_NUMBER_FLOAT(layersObject[i], "offsetx", individual_layer_struct.offsetX);
		}

		if (DOES_ELEMENT_EXIST("offsety", layersObject[i]))
		{
			GET_NUMBER_FLOAT(layersObject[i], "offsety", individual_layer_struct.offsetY);
		}

		switch (pMap->layersVector[i].layerType)
		{
		case TileLayer:
			extract_tile_layer_data(layersObject[i], &individual_layer_struct);
			break;

		case ObjectLayer:

			extract_object_layer_data(layersObject[i], &individual_layer_struct);
			break;

		default:
			break;
		}
	}
	return true;
}

KTIPropertyTypes get_property_type_by_string(const std::wstring & name)
{
	KTIPropertyTypes type = KTIPropertyTypes::String; // default type

	if (name == KTEXT("string"))
	{
		return KTIPropertyTypes::String;
	}
	else if (name == KTEXT("int"))
	{
		return KTIPropertyTypes::Int;
	}
	else if (name == KTEXT("float"))
	{
		return KTIPropertyTypes::Float;
	}
	else if (name == KTEXT("bool"))
	{
		return KTIPropertyTypes::Bool;
	}
	else if (name == KTEXT("color")) //silly American spelling.. 
	{
		return KTIPropertyTypes::HexColour;
	}
	else if (name == KTEXT("file"))
	{
		return KTIPropertyTypes::File;
	}
	return type;
}

KTILayerTypes get_layer_type(const json & layerJsonObj)
{
	if (layerJsonObj["type"].get<string>() == "objectgroup")
	{
		return KTILayerTypes::ObjectLayer;
	}
	return KTILayerTypes::TileLayer;
}

bool is_template_object(const json & mapObjectJson)
{
	mapObjectJson.count("template") > 0;
}

void extract_object_layer_data(const json & objectLayerJson, KTILayer * pObjLayerData)
{
	//extract layer level properties 
	extract_properties_to_map(objectLayerJson, pObjLayerData->propertiesMap, pObjLayerData->propertyTypesMap);

	if (!objectLayerJson.is_object())
	{
		return;
	}

	if (!objectLayerJson["objects"].is_array())
	{
		return;
	}

	const int32 ObjectCount = objectLayerJson["objects"].size();
	const json& mapObjectsArrayJson = objectLayerJson["objects"];

	if (ObjectCount < 1)
	{
		return;
	}

	pObjLayerData->objectsVector.resize(ObjectCount);

	KCHECK(pObjLayerData->objectsVector.size() > 0);

	for (int32 i = 0; i < ObjectCount; ++i)
	{
		extract_object_data(mapObjectsArrayJson[i], &pObjLayerData->objectsVector[i]);
	}
}

void extract_object_data(const json & objectsArray, KTIObject * pObj)
{
	if (!objectsArray.count("name"))
	{
		MAP_PARSE_ERR;
		KPRINTF("Object lacking name field!\n");
		return;
	}

	pObj->name = sf::String(objectsArray["name"].get<string>()).toWideString();

	if (!objectsArray.count("x"))
	{
		MAP_PARSE_ERR;
		KPRINTF_A("Object lacking x position (name %s)!\n", pObj->name.c_str());
		return;
	}

	GET_NUMBER_FLOAT(objectsArray, "x", pObj->x);

	if (!objectsArray.count("y"))
	{
		MAP_PARSE_ERR;
		KPRINTF_A("Object lacking y position (name %s)!\n", pObj->name.c_str());
		return;
	}
	GET_NUMBER_FLOAT(objectsArray, "y", pObj->y);

	if (objectsArray.count("gid") > 0)
	{
		if (objectsArray["gid"].is_number_integer())
		{
			pObj->gid = objectsArray["gid"].get<int>();
		}
	}

	if (objectsArray["id"].is_number_integer())
	{
		pObj->id = objectsArray["id"].get<int>();
	}

	GET_NUMBER_FLOAT(objectsArray, "rotation", pObj->rotation);

	extract_properties_to_map(objectsArray, pObj->propertiesMap, pObj->propertyTypesMap);

	// Extract the objects type 

	if (objectsArray.count("point") > 0)
	{
		pObj->objectType = KTIObjectTypes::Point;
	}
	else if (objectsArray.count("ellipse") > 0)
	{
		pObj->objectType = KTIObjectTypes::Circle;
		GET_NUMBER_FLOAT(objectsArray, "width", pObj->width);
		GET_NUMBER_FLOAT(objectsArray, "height", pObj->height);
	}
	else if (objectsArray.count("polygon") > 0)
	{
		pObj->objectType = KTIObjectTypes::Polygon;
		//@Remember Polygon logic no implemented
	}
	else
	{
		pObj->objectType = KTIObjectTypes::Rect;
		GET_NUMBER_FLOAT(objectsArray, "width", pObj->width);
		GET_NUMBER_FLOAT(objectsArray, "height", pObj->height);
	}
}

void extract_tile_layer_data(const json & tileLayerJson, KTILayer * pTileLayerData)
{
	if (!DOES_ELEMENT_EXIST("width", tileLayerJson))
	{
		MAP_PARSE_ERR;
		KPRINTF("No width on tile layer!\n");
		return;

	}

	GET_NUMBER_FLOAT(tileLayerJson, "width", pTileLayerData->width);

	if (!DOES_ELEMENT_EXIST("height", tileLayerJson))
	{
		MAP_PARSE_ERR;
		KPRINTF("No height on tile layer!\n");
		return;
	}

	GET_NUMBER_FLOAT(tileLayerJson, "height", pTileLayerData->height);

	const Krawler::int32 TILE_TOTAL = pTileLayerData->width* pTileLayerData->height;

	if (tileLayerJson["data"].size() != TILE_TOTAL)
	{
		MAP_PARSE_ERR;
		KPRINTF("Tile array size doesn't match width * height \n");
		return;
	}

	auto&& arr = tileLayerJson["data"].get<std::vector<Krawler::int32>>();
	pTileLayerData->tileData.resize(TILE_TOTAL);
	memcpy_s(&pTileLayerData->tileData[0], sizeof(int32) * TILE_TOTAL, &arr[0], sizeof(int32) * TILE_TOTAL);

	extract_properties_to_map(tileLayerJson, pTileLayerData->propertiesMap, pTileLayerData->propertyTypesMap);
}

bool extract_tile_sets(const json & mapJson, KTIMap * pMap)
{
	if (mapJson.count("tilesets") == 0)
	{
		return true;
	}

	auto tilesetJson = mapJson["tilesets"];

	if (!tilesetJson.is_array())
	{
		MAP_PARSE_ERR;
		KPRINTF("Invalid tileset array!\n");
		return false;
	}

	const int32 TILESET_COUNT = tilesetJson.size();

	if (TILESET_COUNT == 0)
	{
		return true;
	}
	pMap->tilesetVector.resize(TILESET_COUNT);

	for (int32 i = 0; i < TILESET_COUNT; ++i)
	{
		if (!extract_singular_tileset(tilesetJson[i], &pMap->tilesetVector[i]))
		{
			return false;
		}
	}

	return true;
}

bool extract_singular_tileset(const json & tilesetJson, KTITileset * pTilesetData)
{
	if (!DOES_ELEMENT_EXIST("tilewidth", tilesetJson))
	{
		MAP_PARSE_ERR;
		KPRINTF("No tilewidth field on tileset!\n");
		return false;
	}
	GET_NUMBER_INT(tilesetJson, "tilewidth", pTilesetData->tileWidth);

	if (!DOES_ELEMENT_EXIST("tileheight", tilesetJson))
	{
		MAP_PARSE_ERR;
		KPRINTF("No tileheight field on tileset!\n");
		return false;
	}
	GET_NUMBER_INT(tilesetJson, "tileheight", pTilesetData->tileHeight);

	if (!DOES_ELEMENT_EXIST("firstgid", tilesetJson))
	{
		MAP_PARSE_ERR;
		KPRINTF("No firstgid field on tileset!\n");
		return false;
	}
	GET_NUMBER_INT(tilesetJson, "firstgid", pTilesetData->firstGID);

	if (!DOES_ELEMENT_EXIST("imagewidth", tilesetJson))
	{
		MAP_PARSE_ERR;
		KPRINTF("No imagewidth field on tileset!\n");
		return false;
	}
	GET_NUMBER_FLOAT(tilesetJson, "imagewidth", pTilesetData->width);

	if (!DOES_ELEMENT_EXIST("imageheight", tilesetJson))
	{
		MAP_PARSE_ERR;
		KPRINTF("No imageheight field on tileset!\n");
		return false;
	}
	GET_NUMBER_FLOAT(tilesetJson, "imageheight", pTilesetData->height);



	if (tilesetJson.count("name") > 0)
	{
		pTilesetData->name = sf::String(tilesetJson["name"].get<string>()).toWideString();
	}
	extract_properties_to_map(tilesetJson, pTilesetData->propertiesMap, pTilesetData->propertyTypesMap);
	extract_tile_properties(tilesetJson, pTilesetData->tilePropertiesMap, pTilesetData->tilePropertyTypesMap);
	return true;
}

void extract_tile_properties(const json & jsonObj, std::map<std::wstring, KTIPropertiesMap>& tilePropMap, std::map<std::wstring, KTIPropertyTypesMap>& tilePropTypeMap)
{
	auto tile_property_obj_iterator = jsonObj.find("tileproperties");
	auto tile_property_types_obj_iterator = jsonObj.find("tilepropertytypes");

	if (tile_property_obj_iterator == jsonObj.end() || tile_property_types_obj_iterator == jsonObj.end())
	{ // not always likely to find properties, so return true
		return;
	}

	//Get total number of properties 
	const int32 Tile_Property_Count = tile_property_obj_iterator->size();
	const int32 Tile_Property_Type_Count = tile_property_types_obj_iterator->size();

	//Make sure total number of properties matches number of types
	if (Tile_Property_Count != Tile_Property_Type_Count)
	{
		MAP_PARSE_ERR;
		KPrintf(KTEXT("Mismatch between number of map properties (%d) and propertytypes (%d)\n"), Tile_Property_Count, Tile_Property_Type_Count);
		return;
	}

	KTIPropertiesMap temporary_property_map;
	KTIPropertyTypesMap temporary_property_types_map;
	auto tile_property_iterator = tile_property_obj_iterator->begin();
	auto tile_property_types_iterator = tile_property_types_obj_iterator->begin();
	while (tile_property_iterator != tile_property_obj_iterator->end() &&
		tile_property_types_iterator != tile_property_types_obj_iterator->end())
	{
		//@UrgentRefactor switch statement to do function calls, and also isolate out the functionality to isolate properties so its generic for other map object types which will have properties.
		json::value_type tile_properties_obj = *tile_property_obj_iterator;
		json::value_type tile_property_types_obj = *tile_property_types_obj_iterator;

		auto object = tile_properties_obj.object();
		//_CrtDbgBreak();
		//@Fix load tile properties for collision data 
		const std::wstring TILE_KEY = TO_WSTR(tile_property_iterator.key());
		const auto beginProp = tile_property_iterator.value().begin(), beginPropType = tile_property_types_iterator.value().begin();
		const auto endProp = tile_property_iterator.value().end(), endPropType = tile_property_types_iterator.value().end();

		auto properties_iterator = beginProp;
		auto propertyTypes_iterator = beginPropType;
		while (properties_iterator != endProp && propertyTypes_iterator != endPropType)
		{

			const std::wstring Type_Name = sf::String(propertyTypes_iterator.value().get<string>()).toWideString();
			const KTIPropertyTypes Type_Enum = get_property_type_by_string(Type_Name); //isolate the data type of this property 

			KTIProperty mapPropertyUnion{ 0 };
			bool bLoadedCorrectly = true;
			std::wstring key, valueWideStr, type;
			key = sf::String(properties_iterator.key()).toWideString();
			switch (Type_Enum)
			{
			case String:
			{
				valueWideStr = sf::String(properties_iterator.value().get<string>()).toWideString();
				if (valueWideStr.size() < MAX_PROPERTY_STRING_CHARS)
				{
					wcsncpy_s(mapPropertyUnion.type_string, valueWideStr.c_str(), valueWideStr.size());
					//propMap->propertyTypes.emplace(key, Type_Enum);
					temporary_property_map.emplace(key, mapPropertyUnion);
				}
				else
				{
					bLoadedCorrectly = false;
					MAP_PARSE_ERR;
					KPRINTF_A("Unable to load string property from JSON as string is too large!: %s\n", valueWideStr.c_str());
				}
			}
			break;

			case Int:
			{
				mapPropertyUnion.type_int = properties_iterator.value().get<int>();
				temporary_property_map.emplace(key, mapPropertyUnion);
			}
			break;

			case Float:
			{
				mapPropertyUnion.type_float = properties_iterator.value().get<float>();
				temporary_property_map.emplace(key, mapPropertyUnion);
			}
			break;

			case Bool:
			{
				mapPropertyUnion.type_bool = properties_iterator.value().get<bool>();
				temporary_property_map.emplace(key, mapPropertyUnion);
			}
			break;

			case HexColour:
			{
				//@Rethink Horrible way of parsing hex value of colours 
				key = sf::String(properties_iterator.key()).toWideString();
				valueWideStr = sf::String(properties_iterator.value().get<string>()).toWideString();

				/*
				Colours are represented by tiled as
				# ALPHA_BITS RED_BITS BLUE_BITS GREEN_BITS
				Example
				#ff2d0044 => # FF 2d 00 44 (or 45,0,68,255 as RGBA)

				To parse this correctly 4 strings contain each channels hex string representation of the value.
				This is then put through a stringstream to allow conversion directly to int
				*/

				std::wstring alphaChannelChars, redChannelHexChars, greenChannelHexChars, blueChannelHexChars;
				alphaChannelChars.push_back(valueWideStr[1]);
				alphaChannelChars.push_back(valueWideStr[2]);

				redChannelHexChars.push_back(valueWideStr[3]);
				redChannelHexChars.push_back(valueWideStr[4]);

				greenChannelHexChars.push_back(valueWideStr[5]);
				greenChannelHexChars.push_back(valueWideStr[6]);

				blueChannelHexChars.push_back(valueWideStr[7]);
				blueChannelHexChars.push_back(valueWideStr[8]);

				int32 AlphaValue, RedValue, BlueValue, GreenValue;
				wstringstream ss;
				ss << std::hex << alphaChannelChars;
				ss >> AlphaValue;
				ss.clear();

				ss << std::hex << redChannelHexChars;
				ss >> RedValue;
				ss.clear();

				ss << std::hex << blueChannelHexChars;
				ss >> BlueValue;
				ss.clear();

				ss << std::hex << greenChannelHexChars;
				ss >> GreenValue;
				ss.clear();
				mapPropertyUnion.type_colour = Colour((int8)RedValue, (int8)GreenValue, (int8)BlueValue, (int8)AlphaValue);

				temporary_property_map.emplace(key, mapPropertyUnion);
				break;
			}

			case File:
			{
				bLoadedCorrectly = false;
				MAP_PARSE_ERR;
				KPRINTF("File property not supported in this parser!\n");
			}
			break;

			default:
				break;
			}

			if (bLoadedCorrectly)
			{
				temporary_property_types_map.emplace(key, Type_Enum);
			}
			++properties_iterator;
			++propertyTypes_iterator;
		}

		tilePropMap.emplace(TILE_KEY, temporary_property_map);
		tilePropTypeMap.emplace(TILE_KEY, temporary_property_types_map);

		//clear to be re-used in next loop iteration
		temporary_property_map.clear();
		temporary_property_types_map.clear();
		++tile_property_iterator;
		++tile_property_types_iterator;

	}
}

void extract_tile_properties(const json & jsonObj, KTIPropertiesMap & propMap, KTIPropertyTypesMap & typeMap)
{
	auto property_obj_it = jsonObj.find("tileproperties");
	auto property_types_it = jsonObj.find("tilepropertytypes");

	if (property_obj_it == jsonObj.end() || property_types_it == jsonObj.end())
	{ // not always likely to find properties, so return true
		return;
	}

	//Get total number of properties 
	const int32 Property_Count = property_obj_it->size();
	const int32 Type_Count = property_types_it->size();

	//Make sure total number of properties matches number of types
	if (Property_Count != Type_Count)
	{
		MAP_PARSE_ERR;
		KPrintf(KTEXT("Mismatch between number of map properties (%d) and propertytypes (%d)\n"), Property_Count, Type_Count);
		return;
	}

	//@UrgentRefactor switch statement to do function calls, and also isolate out the functionality to isolate properties so its generic for other map object types which will have properties.
	json::value_type properties_obj = *property_obj_it;
	properties_obj = *properties_obj.begin();
	json::value_type propertyTypes_obj = *property_types_it;
	propertyTypes_obj = *propertyTypes_obj.begin();

	json::iterator itProperties = properties_obj.begin(), itPropertyTypes = propertyTypes_obj.begin();

	//iterate through all properties & property-types objects
	while (itProperties != properties_obj.end() && itPropertyTypes != propertyTypes_obj.end())
	{
		const std::wstring Type_Name = sf::String(itPropertyTypes.value().get<string>()).toWideString();
		const KTIPropertyTypes Type_Enum = get_property_type_by_string(Type_Name); //isolate the data type of this property 

		KTIProperty mapPropertyUnion{ 0 };
		bool bLoadedCorrectly = true;
		std::wstring key, valueWideStr, type;
		key = sf::String(itProperties.key()).toWideString();
		switch (Type_Enum)
		{
		case String:
		{
			valueWideStr = sf::String(itProperties.value().get<string>()).toWideString();
			if (valueWideStr.size() < MAX_PROPERTY_STRING_CHARS)
			{
				wcsncpy_s(mapPropertyUnion.type_string, valueWideStr.c_str(), valueWideStr.size());
				//propMap->propertyTypes.emplace(key, Type_Enum);
				propMap.emplace(key, mapPropertyUnion);
			}
			else
			{
				bLoadedCorrectly = false;
				MAP_PARSE_ERR;
				KPRINTF_A("Unable to load string property from JSON as string is too large!: %s\n", valueWideStr.c_str());
			}
		}
		break;

		case Int:
		{
			mapPropertyUnion.type_int = itProperties.value().get<int>();
			propMap.emplace(key, mapPropertyUnion);
		}
		break;

		case Float:
		{
			mapPropertyUnion.type_float = itProperties.value().get<float>();
			propMap.emplace(key, mapPropertyUnion);
		}
		break;

		case Bool:
		{
			mapPropertyUnion.type_bool = itProperties.value().get<bool>();
			propMap.emplace(key, mapPropertyUnion);
		}
		break;

		case HexColour:
		{
			//@Rethink Horrible way of parsing hex value of colours 
			key = sf::String(itProperties.key()).toWideString();
			valueWideStr = sf::String(itProperties.value().get<string>()).toWideString();

			/*
			Colours are represented by tiled as
			# ALPHA_BITS RED_BITS BLUE_BITS GREEN_BITS
			Example
			#ff2d0044 => # FF 2d 00 44 (or 45,0,68,255 as RGBA)

			To parse this correctly 4 strings contain each channels hex string representation of the value.
			This is then put through a stringstream to allow conversion directly to int
			*/

			std::wstring alphaChannelChars, redChannelHexChars, greenChannelHexChars, blueChannelHexChars;
			alphaChannelChars.push_back(valueWideStr[1]);
			alphaChannelChars.push_back(valueWideStr[2]);

			redChannelHexChars.push_back(valueWideStr[3]);
			redChannelHexChars.push_back(valueWideStr[4]);

			greenChannelHexChars.push_back(valueWideStr[5]);
			greenChannelHexChars.push_back(valueWideStr[6]);

			blueChannelHexChars.push_back(valueWideStr[7]);
			blueChannelHexChars.push_back(valueWideStr[8]);

			int32 AlphaValue, RedValue, BlueValue, GreenValue;
			wstringstream ss;
			ss << std::hex << alphaChannelChars;
			ss >> AlphaValue;
			ss.clear();

			ss << std::hex << redChannelHexChars;
			ss >> RedValue;
			ss.clear();

			ss << std::hex << blueChannelHexChars;
			ss >> BlueValue;
			ss.clear();

			ss << std::hex << greenChannelHexChars;
			ss >> GreenValue;
			ss.clear();
			mapPropertyUnion.type_colour = Colour((int8)RedValue, (int8)GreenValue, (int8)BlueValue, (int8)AlphaValue);

			propMap.emplace(key, mapPropertyUnion);
			break;
		}

		case File:
		{
			bLoadedCorrectly = false;
			MAP_PARSE_ERR;
			KPRINTF("File property not supported in this parser!\n");
		}
		break;

		default:
			break;
		}

		if (bLoadedCorrectly)
		{
			typeMap.emplace(key, Type_Enum);
		}
		++itProperties;
		++itPropertyTypes;
	}
	_CrtDbgBreak();
}

bool get_string_if_present(wstring & value, const string & name, const json & jsonObj)
{
	auto strProperty = jsonObj.count(name);
	if (strProperty == 0)
	{
		return false;
	}

	auto result = jsonObj[name];

	if (!result.is_string())
	{
		return false;
	}
	value = sf::String(result.get<string>()).toWideString();
	return true;
}
