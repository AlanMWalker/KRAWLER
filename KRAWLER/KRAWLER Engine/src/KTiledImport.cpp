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



#define MAP_PARSE_ERR KPRINTF("JSON PARSE ERROR! ")

static bool is_valid_level_map_type(const json& rootJson); //Is the 
static bool load_level_map_layer(const json& rootJson, KLevelMap* pMap);
static bool get_int_safe(int32& value, const string& name, const json& rootJson);
static void extract_properties_to_map(const json& jsonObj, std::map<std::wstring, KLevelMapProperty> & propMap, std::map<std::wstring, KLevelMapPropertyTypes> &  typeMap);

static KLevelMapPropertyTypes get_property_type_by_string(const std::wstring& name);
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

void Krawler::TiledImport::cleanupLevelMap(KLevelMap * pMap)
{
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

	extract_properties_to_map(rootJson, pMap->properties, pMap->propertyTypes);

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

void extract_properties_to_map(const json& jsonObj, std::map<std::wstring, KLevelMapProperty> & propMap, std::map<std::wstring, KLevelMapPropertyTypes> &  typeMap)
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
		const KLevelMapPropertyTypes Type_Enum = get_property_type_by_string(Type_Name); //isolate the data type of this property 

		KLevelMapProperty mapPropertyUnion{ 0 };
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
			KPRINTF("File property not supported in this parser");
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

KLevelMapPropertyTypes get_property_type_by_string(const std::wstring & name)
{
	KLevelMapPropertyTypes type = KLevelMapPropertyTypes::String; // default type

	if (name == KTEXT("string"))
	{
		return KLevelMapPropertyTypes::String;
	}
	else if (name == KTEXT("int"))
	{
		return KLevelMapPropertyTypes::Int;
	}
	else if (name == KTEXT("float"))
	{
		return KLevelMapPropertyTypes::Float;
	}
	else if (name == KTEXT("bool"))
	{
		return KLevelMapPropertyTypes::Bool;
	}
	else if (name == KTEXT("color")) //silly American spelling.. 
	{
		return KLevelMapPropertyTypes::HexColour;
	}
	else if (name == KTEXT("file"))
	{
		return KLevelMapPropertyTypes::File;
	}
	return type;
}
