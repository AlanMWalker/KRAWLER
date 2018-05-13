#include "AssetLoader\KAssetLoader.h"

#include <future>
#include <fstream>
#include <stdio.h>
#include <..\rapidxml\rapidxml.hpp>

using namespace sf;
using namespace Krawler;
using namespace std;
using namespace rapidxml;

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
	//auto fileIterator = m_texturesMap.find(m_rootFolder + fileName);
	//if (fileIterator != m_texturesMap.end())
	//{
	//	return fileIterator->second;
	//}
	//
	//Texture t;
	//sf::String s(m_rootFolder + fileName);
	//
	//if (!t.loadFromFile(s.toAnsiString()))
	//{
	//	return nullptr;
	//}
	//
	//m_texturesMap.emplace(s, new Texture(t));

	//return m_texturesMap[m_rootFolder + fileName];
	auto p = std::async(&KAssetLoader::loadTextureASYNC, this, fileName);
	return p.get();
}

sf::SoundBuffer* Krawler::KAssetLoader::loadSoundBuffer(const std::wstring & fileName)
{
	auto fileIterator = m_soundBufferMap.find(m_rootFolder + fileName);
	if (fileIterator != m_soundBufferMap.end())
	{
		return (fileIterator->second);
	}

	SoundBuffer* sb = new SoundBuffer;
	sf::String s(m_rootFolder + fileName);

	if (!sb->loadFromFile(s.toAnsiString()))
	{
		delete sb;
		KPrintf(KTEXT("Failed to load sound %ws\n"), s.toWideString().c_str());
		return nullptr;
	}

	m_soundBufferMap.emplace(s, sb);

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
sf::Shader * KAssetLoader::loadShader(const std::wstring & vertShader, const std::wstring & fragShader)
{
	auto fileIterator = m_shaderMap.find(m_rootFolder + vertShader);
	if (fileIterator != m_shaderMap.end())
	{
		return fileIterator->second;
	}

	sf::String sVert(m_rootFolder + vertShader);
	sf::String sFrag(m_rootFolder + fragShader);
	Shader* pShader = new Shader();
	KCHECK(pShader);
	if (!pShader->loadFromFile(sVert, sFrag))
	{
		return nullptr;
	}

	m_shaderMap.emplace(sVert, pShader);

	return m_shaderMap[sVert];
}

Krawler::KAssetLoader::KAssetLoader()
{
#pragma region RAPIDXML LOADING
	FILE* pFile = NULL;
	fopen_s(&pFile, "assets.xml", "r");
	if (pFile == NULL)
	{
		KPRINTF("Failed to open asset file!");
		return;
	}
	//TODO handle returns of FSEEK for data validity 
	fseek(pFile, 0, SEEK_END);
	const int CHAR_COUNT = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	char* pBuffer = (char*)malloc(sizeof(char) * CHAR_COUNT + 1);
	fread_s(pBuffer, CHAR_COUNT + 1 * sizeof(char), sizeof(char), CHAR_COUNT + 1, pFile);

	int closingXMLIndex = 0;
	for (int i = CHAR_COUNT; i > 0; --i)
	{
		if (pBuffer[i] == '>')
		{
			closingXMLIndex = i;
			break;
		}
	}
	pBuffer[closingXMLIndex + 1] = '\0';

	fclose(pFile);
	pFile = NULL;
	xml_document<> doc;
	doc.parse<0>(pBuffer);
	xml_node<>* pFirstNode = doc.first_node("assets");
#pragma endregion 

	if (!pFirstNode)
	{
		printf("No asset node!\n");
		goto cleanup_branch;
	}

	xml_node<>* pFirstNodeChild = pFirstNode->first_node();

	while (pFirstNodeChild)
	{
		printf("%s\n", pFirstNodeChild->name());

		if (string("texture") == string(pFirstNodeChild->name()))
		{
			string filePath = fp;
			loadTexture();
		}
		else if (string("shader") == string(pFirstNodeChild->name()))
		{

		}
		else if (string("sound") == string(pFirstNodeChild->name()))
		{

		}

		pFirstNodeChild = pFirstNodeChild->next_sibling();
	}

cleanup_branch:
	free(pBuffer);
	doc.clear();

}

sf::Texture * Krawler::KAssetLoader::loadTextureASYNC(const std::wstring & fileName)
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
