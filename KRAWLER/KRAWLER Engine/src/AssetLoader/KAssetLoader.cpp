#include "AssetLoader\KAssetLoader.h"

using namespace sf;
using namespace Krawler;

Krawler::KAssetLoader::~KAssetLoader()
{
}

KRAWLER_API void Krawler::KAssetLoader::cleanupAssetLoader()
{
	for (auto& pair : m_texturesMap)
	{
		KFREE(pair.second);
	}
	m_texturesMap.clear();

	for (auto& pair : m_soundBufferMap)
	{
		KFREE(pair.second);
	}
	m_soundBufferMap.clear();

	for (auto& pair : m_fontMap)
	{
		KFREE(pair.second);
	}
	m_fontMap.clear();
}

sf::Texture * Krawler::KAssetLoader::loadTexture(const std::wstring & fileName)
{
	auto fileIterator = m_texturesMap.find(m_rootFolder + fileName);
	if (fileIterator != m_texturesMap.end())
	{
		return fileIterator->second;
	}

	Texture t;
	sf::String s(m_rootFolder + fileName);

	if (!t.loadFromFile(s.toAnsiString()))
	{
		return nullptr;
	}

	m_texturesMap.emplace(s, new Texture(t));

	return m_texturesMap[m_rootFolder + fileName];
}

sf::SoundBuffer * Krawler::KAssetLoader::loadSoundBuffer(const std::wstring & fileName)
{
	auto fileIterator = m_soundBufferMap.find(m_rootFolder + fileName);
	if (fileIterator != m_soundBufferMap.end())
	{
		return fileIterator->second;
	}

	SoundBuffer sb;
	sf::String s(m_rootFolder + fileName);

	if (!sb.loadFromFile(s.toAnsiString()))
	{
		return nullptr;
	}

	m_soundBufferMap.emplace(s, new SoundBuffer(sb));

	return m_soundBufferMap[m_rootFolder + fileName];
}

sf::Font * Krawler::KAssetLoader::loadFont(const std::wstring & fileName)
{
	auto fileIterator = m_fontMap.find(m_rootFolder + fileName);
	if (fileIterator != m_fontMap.end())
	{
		return fileIterator->second;
	}

	Font f;
	sf::String s(m_rootFolder + fileName);

	if (!f.loadFromFile(s.toAnsiString()))
	{
		return nullptr;
	}

	m_fontMap.emplace(s, new Font(f));

	return m_fontMap[m_rootFolder + fileName];
}
