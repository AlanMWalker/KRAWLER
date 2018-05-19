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

void KAssetLoader::loadTexture(const std::wstring & name, const std::wstring & filePath)
{
	Texture t;
	sf::String s(m_rootFolder + KTEXT("\\") + filePath);

	if (!t.loadFromFile(s.toAnsiString()))
	{
		return;
	}

	m_texturesMap.emplace(name, new Texture(t));
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
	FILE* pFile = NULL;
	_wfopen_s(&pFile, KTEXT("assets.xml"), KTEXT("r"));

	if (pFile == NULL)
	{
		KPRINTF("Failed to open asset file!");
		return;
	}
	//TODO handle returns of FSEEK for data validity 
	fseek(pFile, 0, SEEK_END);
	const int CHAR_COUNT = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	const int BUFFER_SIZE = sizeof(wchar_t) * CHAR_COUNT + 1;
	wchar_t* pBuffer = (wchar_t*)malloc(BUFFER_SIZE);
	for (int i = 0; i < CHAR_COUNT; ++i)
	{
		pBuffer[i] = fgetwc(pFile);
	}

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
	xml_document<wchar_t> assetsXMLDoc;
	assetsXMLDoc.parse<0>(pBuffer);
	xml_node<wchar_t>* assetNodeVerification = assetsXMLDoc.first_node(KTEXT("assets"));

	if (!assetNodeVerification)
	{
		KPRINTF("No asset node!\n");
		goto cleanup_branch;
	}

	xml_node<wchar_t>* pRootFolderNode = assetNodeVerification->first_node();
	if (!pRootFolderNode)
	{
		KPRINTF("No root folder node specified!\n");
		goto cleanup_branch;
	}

	if (wstring(pRootFolderNode->name()) != KTEXT("root_folder"))
	{
		KPRINTF("No root folder node specified!\n");
		goto cleanup_branch;
	}

	xml_attribute<wchar_t>* pRootFolderName = pRootFolderNode->first_attribute();
	if (!pRootFolderName)
	{
		KPRINTF("No root folder node specified!\n");
		goto cleanup_branch;
	}

	if (wstring(pRootFolderName->name()) != KTEXT("folder_name"))
	{
		KPRINTF("No root folder name specified!\n");
		goto cleanup_branch;
	}

	m_rootFolder = pRootFolderName->value();

	xml_node<wchar_t>* pAssetTypeNodes;
	pAssetTypeNodes = pRootFolderNode->next_sibling();

	while (pAssetTypeNodes)
	{
		KPRINTF_A("%s\n", pAssetTypeNodes->name());

		xml_attribute<wchar_t>* pAttrName = nullptr;
		xml_attribute<wchar_t>* pAttrFilepath = nullptr;
		wstring assetName;
		wstring assetPathA, assetPathB;

		if (wstring(pAssetTypeNodes->name()) == KTEXT("texture"))
		{
			pAttrName = pAssetTypeNodes->first_attribute();
			if (pAttrName)
			{
				if (wstring(pAttrName->name()) == KTEXT("name"))
				{
					assetName = pAttrName->value();
				}
				pAttrFilepath = pAttrName->next_attribute();
				if (pAttrFilepath)
				{
					if (wstring(pAttrFilepath->name()) == KTEXT("file_path"))
					{
						assetPathA = pAttrFilepath->value();
					}
				}
			}
			loadTexture(assetName, assetPathA);
		}
		else if (wstring(pAssetTypeNodes->name()) == KTEXT("shader"))
		{

		}
		else if (wstring(pAssetTypeNodes->name()) == KTEXT("sound"))
		{

		}
		else if (wstring(pAssetTypeNodes->name()) == KTEXT("font"))
		{

		}
		else
		{
			KPRINTF_A("Label %s is not a valid asset type (font, texture, shader or sound)\n", pAssetTypeNodes->name());
		}

		pAssetTypeNodes = pAssetTypeNodes->next_sibling();
	}

cleanup_branch:
	free(pBuffer);
	assetsXMLDoc.clear();

}

