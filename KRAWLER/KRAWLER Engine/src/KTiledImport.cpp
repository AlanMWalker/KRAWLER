#include "Tiled\KTiledImport.h"

#include <fstream>
#include "JSON\json.hpp"

using namespace Krawler;
using namespace Krawler::TiledImport;
using namespace std;

using json = nlohmann::json;

KLevelMap * Krawler::TiledImport::loadTiledJSONFile(const std::wstring filePath)
{
	KLevelMap* pMap = nullptr;

	ifstream jsonFile;
	jsonFile.open(filePath, ios::in);

	if (jsonFile.fail())
	{
		return nullptr;
	}
	
	json rootObject;

	jsonFile >> rootObject;
	jsonFile.close();

	return pMap;
}
