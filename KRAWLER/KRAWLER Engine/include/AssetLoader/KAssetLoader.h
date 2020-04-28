#ifndef KASSET_LOADER_H
#define KASSET_LOADER_H

#include "Krawler.h"
#include <SFML\Graphics\Texture.hpp>
#include <SFML\Graphics\Font.hpp>
#include <SFML\Audio\SoundBuffer.hpp>
#include <SFML\Graphics\Shader.hpp>
#include <Tiled\KTiledImport.h>

#include <unordered_map>

#ifndef ASSET
#define ASSET() KAssetLoader::getAssetLoader()
#endif 

namespace Krawler
{
	namespace Animation
	{
		struct KAnimation;
	};

	const std::wstring ACCEPTED_SHADERS[] =
	{
		L".glsl",
		L".frag",
		L".vert"
	};

	const std::wstring ACCEPTED_TEXTURES[] =
	{
		L".png",
		L".bmp",
		L".jpg"
		//L"xml", // to be impl
		//L"json" // not supported yet
	};

	const std::wstring ACCEPTED_AUDIO[]
	{
		L".wav",
		L".ogg"
	};

	const std::wstring ACCEPTED_FONT[]
	{
		L".ttf"
	};

	const std::wstring SHADER_SUFFIXES[]
	{
		L"_v",
		L"_f"
	};

	const std::wstring ACCEPTED_MAP[]
	{
		L".tmx", 
		L".json"
	};

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

		KRAWLER_API sf::Texture* const getTexture(const std::wstring& name);
		KRAWLER_API sf::SoundBuffer* const getSound(const std::wstring& name);
		KRAWLER_API sf::Shader* const getShader(const std::wstring& name);
		KRAWLER_API sf::Font* const getFont(const std::wstring& name);
		KRAWLER_API Animation::KAnimation* const getAnimation(const std::wstring& name);
		KRAWLER_API TiledImport::KTIMap* const getLevelMap(const std::wstring& name);

		KRAWLER_API void setRootFolder(const std::wstring& rootFolder) { m_rootFolder = rootFolder; }


	private:

		KAssetLoader();
		void scanFolderLoad();
		void loadAnimationsXML();
		void matchAnimationsToTextures();

		std::wstring m_rootFolder;

		void loadTexture(const std::wstring& name, const std::wstring& filePath);
		void loadShader(const std::wstring& shaderName, const std::wstring& shaderPath);
		void loadSound(const std::wstring& name, const std::wstring& filePath);
		void loadFont(const std::wstring& name, const std::wstring& filePath);
		void loadTilemap(const std::wstring& name, const std::wstring& filePath);

		void loadDefaultShadersFromString();

		std::unordered_map <std::wstring, sf::Texture*> m_texturesMap;
		std::unordered_map <std::wstring, sf::Font*> m_fontMap;
		std::unordered_map <std::wstring, sf::SoundBuffer*> m_soundBufferMap;
		std::unordered_map <std::wstring, sf::Shader*> m_shaderMap;
		std::unordered_map <std::wstring, Animation::KAnimation*> m_animationsMap;
		std::unordered_map <std::wstring, TiledImport::KTIMap*> m_importedLevelsMap;

	};
}

#endif 
