#ifndef KTILED_MAP_H
#define KTILED_MAP_H

#include "Krawler.h"

#include <SFML\Graphics\VertexArray.hpp>
#include <SFML\Graphics\Transformable.hpp>
#include <SFML\Graphics\Drawable.hpp>
#include <SFML\Graphics\RenderTarget.hpp>

#define MAP_FILE_FORMAT KTEXT(".dat")

namespace Krawler
{
	namespace TiledMap
	{
		enum KTiledMapLoadResult
		{
			Map_Load_Success,
			Map_Load_Fail_No_File_Found = -1,
			Map_Load_Fail_Invalid_Map_File = -2,
			Map_Load_Fail_Out_Of_Memory = -3
		};

		struct KMapTile // data structure of data about tiled map 
		{
			int xPos;
			int yPos;
			bool blocked;
		};

		class KTiledMap : public sf::Drawable, public sf::Transformable
		{

		public:

			KRAWLER_API KTiledMap();
			KRAWLER_API ~KTiledMap() = default;

			KTiledMap(const KTiledMap& map) = delete; //Non-copyable
			KTiledMap(const KTiledMap&& map) = delete; //Non-copyable 

			virtual void draw(sf::RenderTarget& rTarget, sf::RenderStates rStates) const;

			KRAWLER_API void cleanupTiledMap(); // cleanup function
			KRAWLER_API KTiledMapLoadResult setupTiledMap(const  std::wstring& filename);

		private:

			int32 m_mapWidth;
			int32 m_mapHeight;
			int32 m_tileWidth;
			int32 m_tileHeight;

			KMapTile* mp_mapData;

			sf::VertexArray m_vertArray;

		};
	}
}

#endif
