#include "Tiled\KTiledImport.h"

#include <fstream>
#include <sstream>

#include "JSON\json.hpp"

#include <SFML\System\String.hpp>

using namespace Krawler;
using namespace Krawler::TiledImport;
using namespace std;

using json = nlohmann::json;



#define MAP_PARSE_ERR KPRINTF("JSON PARSE ERROR! ")

static bool is_valid_level_map_type(const json& rootJson); //Is the 
static bool load_level_map_layer(const json& rootJson, KLevelMap* pMap);
static bool get_int_safe(int32& value, const string& name, const json& rootJson);
static void get_map_properties(const json& mapJson, KLevelMap* pMap);

static KPropertyTypes get_property_type_by_string(const std::wstring& name);
static KLayerTypes get_layer_type(const json& layerJsonObj);

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
		MAP_PARSE_ERR;
		KPRINTF("Failed to find 'width' field on map!\n");
		return false;
	}

	//map height
	if (!get_int_safe(pMap->height, "height", rootJson))
	{
		MAP_PARSE_ERR;
		KPRINTF("Failed to find 'height' field on map!\n");
		return false;
	}

	if (!get_int_safe(pMap->tileWidth, "tilewidth", rootJson))
	{
		MAP_PARSE_ERR;
		KPRINTF("Failed to find 'tilewidth' field on map!\n");
		return false;
	}

	if (!get_int_safe(pMap->tileHeight, "tileheight", rootJson))
	{
		MAP_PARSE_ERR;
		KPRINTF("Failed to find 'tileheight' field on map!\n");
		return false;
	}

	if (!get_int_safe(pMap->nextObjectID, "nextobjectid", rootJson))
	{
		MAP_PARSE_ERR;
		KPRINTF("Failed to find 'nextobjectid' field on map!\n");
		return false;
	}

	get_map_properties(rootJson, pMap);

	return true;
}

bool get_int_safe(int32 & value, const string & name, const json & rootJson)
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

void get_map_properties(const json & mapJson, KLevelMap * pMap)
{
	auto property_obj_it = mapJson.find("properties");
	auto property_types_it = mapJson.find("propertytypes");

	if (property_obj_it == mapJson.end() || property_types_it == mapJson.end())
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

		std::wstring Type_Name = sf::String(itPropertyTypes.value().get<string>()).toWideString();
		const KPropertyTypes Type_Enum = get_property_type_by_string(Type_Name); //isolate the data type of this property 
		KLevelMapProperty mapPropertyUnion;

		std::wstring key, value, type;
		switch (Type_Enum)
		{
		case String:
		{
			key = sf::String(itProperties.key()).toWideString();
			value = sf::String(itProperties.value().get<string>()).toWideString();
			//pMap->propertyTypes.emplace(key, )
		}
		break;

		case Int:
			break;

		case Float:
			break;

		case Bool:
			break;

		case HexColour:
		{
			//@Rethink Horrible way of parsing hex value of colours 
			key = sf::String(itProperties.key()).toWideString();
			value = sf::String(itProperties.value().get<string>()).toWideString();

			/*
			Colours are represented by tiled as
			# ALPHA_BITS RED_BITS BLUE_BITS GREEN_BITS
			Example
			#ff2d0044 => # FF 2d 00 44 (or 45,0,68,255 as RGBA)

			To parse this correctly 4 strings contain each channels hex string representation of the value.
			This is then put through a stringstream to allow conversion directly to int
			*/

			std::wstring alphaChannelChars, redChannelHexChars, greenChannelHexChars, blueChannelHexChars;
			alphaChannelChars.push_back(value[1]);
			alphaChannelChars.push_back(value[2]);

			redChannelHexChars.push_back(value[3]);
			redChannelHexChars.push_back(value[4]);

			greenChannelHexChars.push_back(value[5]);
			greenChannelHexChars.push_back(value[6]);

			blueChannelHexChars.push_back(value[7]);
			blueChannelHexChars.push_back(value[8]);

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

			pMap->properties.emplace(key, mapPropertyUnion);
			break;
		}
		case File:
			break;

		default:
			break;
		}

		pMap->propertyTypes.emplace(key, Type_Enum);
		++itProperties;
		++itPropertyTypes;
	}
}

KPropertyTypes get_property_type_by_string(const std::wstring & name)
{
	KPropertyTypes type = KPropertyTypes::String; // default type

	if (name == KTEXT("string"))
	{
		return KPropertyTypes::String;
	}
	else if (name == KTEXT("int"))
	{
		return KPropertyTypes::Int;
	}
	else if (name == KTEXT("float"))
	{
		return KPropertyTypes::Float;
	}
	else if (name == KTEXT("bool"))
	{
		return KPropertyTypes::Bool;
	}
	else if (name == KTEXT("color")) //silly American spelling.. 
	{
		return KPropertyTypes::HexColour;
	}
	else if (name == KTEXT("file"))
	{
		return KPropertyTypes::File;
	}
	return type;
}
