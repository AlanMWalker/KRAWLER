#include "AssetLoader\KAssetLoader.h"

#include <future>
#include <fstream>
#include <stdio.h>
#include <..\rapidxml\rapidxml.hpp>

using namespace sf;
using namespace Krawler;
using namespace std;
using namespace rapidxml;

KAssetLoader::~KAssetLoader()
{
}

void KAssetLoader::cleanupAssetLoader()
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

sf::Texture * const KAssetLoader::getTexture(const std::wstring & name)
{
	auto findResult = m_texturesMap.find(name);

	if (findResult == m_texturesMap.end())
	{
		return nullptr;
	}

	return findResult->second;
}

sf::SoundBuffer * const KAssetLoader::getSound(const std::wstring & name)
{
	auto findResult = m_soundBufferMap.find(name);
	if (findResult == m_soundBufferMap.end())
	{
		return nullptr;
	}

	return findResult->second;
}

sf::Shader * const KAssetLoader::getShader(const std::wstring & name)
{
	auto findResult = m_shaderMap.find(name);
	if (findResult == m_shaderMap.end())
	{
		return nullptr;
	}

	return findResult->second;
}

sf::Font * const KAssetLoader::getFont(const std::wstring & name)
{
	auto findResult = m_fontMap.find(name);
	if (findResult == m_fontMap.end())
	{
		return nullptr;
	}

	return findResult->second;
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

void KAssetLoader::loadSound(const std::wstring & name, const std::wstring & filePath)
{
	SoundBuffer* sb = new SoundBuffer;
	sf::String s(m_rootFolder + KTEXT("\\") + filePath);

	if (!sb->loadFromFile(s.toAnsiString()))
	{
		delete sb;
		KPrintf(KTEXT("Failed to load sound %ws\n"), s.toWideString().c_str());
		return;
	}

	m_soundBufferMap.emplace(name, sb);
}

void KAssetLoader::loadFont(const std::wstring& name, const std::wstring& filePath)
{
	Font f;
	const sf::String s(m_rootFolder + KTEXT("\\") + filePath);

	if (!f.loadFromFile(s.toAnsiString()))
	{
		return;
	}

	m_fontMap.emplace(name, new Font(f));
}

void KAssetLoader::loadShader(const std::wstring& shaderName, const std::wstring & vertShader, const std::wstring & fragShader)
{
	const sf::String sVert(m_rootFolder + KTEXT("\\") + vertShader);
	const sf::String sFrag(m_rootFolder + KTEXT("\\") + fragShader);
	Shader* const pShader = new Shader();
	KCHECK(pShader);

	if (!pShader->loadFromFile(sVert, sFrag))
	{
		return;
	}

	m_shaderMap.emplace(shaderName, pShader);
}

KAssetLoader::KAssetLoader()
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

	wchar_t* const pBuffer = (wchar_t*)malloc(BUFFER_SIZE);
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
		goto cleanup_fail;
	}

	xml_node<wchar_t>* pRootFolderNode = assetNodeVerification->first_node();
	if (!pRootFolderNode)
	{
		KPRINTF("No root folder node specified!\n");
		goto cleanup_fail;
	}

	if (wstring(pRootFolderNode->name()) != KTEXT("root_folder"))
	{
		KPRINTF("No root folder node specified!\n");
		goto cleanup_fail;
	}

	xml_attribute<wchar_t>* pRootFolderName = pRootFolderNode->first_attribute();
	if (!pRootFolderName)
	{
		KPRINTF("No root folder node specified!\n");
		goto cleanup_fail;
	}

	if (wstring(pRootFolderName->name()) != KTEXT("folder_name"))
	{
		KPRINTF("No root folder name specified!\n");
		goto cleanup_fail;
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

			if (assetName.length() == 0 || assetPathA.size() == 0)
			{
				KPRINTF("Asset attributes length == 0!\n");
				goto cleanup_fail;
			}
			loadTexture(assetName, assetPathA);
		}
		else if (wstring(pAssetTypeNodes->name()) == KTEXT("shader"))
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
					if (wstring(pAttrFilepath->name()) == KTEXT("vert_path"))
					{
						assetPathA = pAttrFilepath->value();
					}
				}

				pAttrFilepath = pAttrFilepath->next_attribute();
				if (pAttrFilepath)
				{
					if (wstring(pAttrFilepath->name()) == KTEXT("frag_path"))
					{
						assetPathB = pAttrFilepath->value();
					}
				}
			}
			if (assetName.length() == 0 || assetPathA.size() == 0 || assetPathB.size() == 0)
			{
				KPRINTF("Asset attributes length == 0!\n");
				goto cleanup_fail;
			}
			loadShader(assetName, assetPathA, assetPathB);
		}
		else if (wstring(pAssetTypeNodes->name()) == KTEXT("sound"))
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
			if (assetName.length() == 0 || assetPathA.size() == 0)
			{
				KPRINTF("Asset attributes length == 0!\n");
				goto cleanup_fail;
			}
			loadSound(assetName, assetPathA);
		}
		else if (wstring(pAssetTypeNodes->name()) == KTEXT("font"))
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
			if (assetName.length() == 0 || assetPathA.size() == 0)
			{
				KPRINTF("Asset attributes length == 0!\n");
				goto cleanup_fail;
			}
			loadFont(assetName, assetPathA);
		}
		else
		{
			KPRINTF_A("Label %s is not a valid asset type (font, texture, shader or sound)\n", pAssetTypeNodes->name());
		}

		pAssetTypeNodes = pAssetTypeNodes->next_sibling();
	}


	goto cleanup_safe;

cleanup_fail:
	free(pBuffer);
	assetsXMLDoc.clear();
	_CrtDbgBreak();
	return;

cleanup_safe:
	free(pBuffer);
	assetsXMLDoc.clear();
	return;


}

