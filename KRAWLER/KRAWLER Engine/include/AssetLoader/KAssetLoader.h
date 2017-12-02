#ifndef KASSET_LOADER_H
#define KASSET_LOADER_H

#include "Krawler.h"
#include <SFML\Graphics\Texture.hpp>
#include <SFML\Graphics\Font.hpp>
#include <SFML\Audio\SoundBuffer.hpp>

namespace Krawler
{
	class KAssetLoader
	{
	public:
		KRAWLER_API ~KAssetLoader();

		KRAWLER_API static KAssetLoader& getAssetLoader()
		{
			static KAssetLoader* pApplication = new KAssetLoader();

			return *pApplication;
		}

		KRAWLER_API void cleanupAssetLoader();
		KRAWLER_API sf::Texture* loadTexture(const std::wstring& fileName);
		KRAWLER_API sf::SoundBuffer* loadSoundBuffer(const std::wstring& fileName);
		KRAWLER_API sf::Font* loadFont(const std::wstring& fileName);

		KRAWLER_API void setRootFolder(const std::wstring& rootFolder) { m_rootFolder = rootFolder; }

	private:

		KAssetLoader() = default;

		std::wstring m_rootFolder;
		std::map <std::wstring, sf::Texture*> m_texturesMap;
		std::map <std::wstring, sf::Font*> m_fontMap;
		std::map <std::wstring, sf::SoundBuffer*> m_soundBufferMap;

	};
}

#endif 
