#ifndef KASSET_LOADER_H
#define KASSET_LOADER_H

#include "Krawler.h"
#include <SFML\Graphics\Texture.hpp>
#include <SFML\Graphics\Font.hpp>
#include <SFML\Audio\SoundBuffer.hpp>
#include <SFML\Graphics\Shader.hpp>

#include <unordered_map>

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

		KRAWLER_API sf::Shader* loadShader(const std::wstring& vertShader, const std::wstring& fragShader);

		KRAWLER_API void setRootFolder(const std::wstring& rootFolder) { m_rootFolder = rootFolder; }
		

	private:

		KAssetLoader();

		std::wstring m_rootFolder;
		std::unordered_map <std::wstring, sf::Texture*> m_texturesMap;
		std::unordered_map <std::wstring, sf::Font*> m_fontMap;
		std::unordered_map <std::wstring, sf::SoundBuffer*> m_soundBufferMap;
		std::unordered_map<std::wstring, sf::Shader*> m_shaderMap;

		sf::Texture* loadTextureASYNC(const std::wstring& fileName);

	};
}

#endif 
