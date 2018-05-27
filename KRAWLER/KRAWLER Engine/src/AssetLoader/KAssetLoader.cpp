#include "AssetLoader\KAssetLoader.h"
#include "Animation\KAnimation.h"

#include <future>
#include <fstream>
#include <stdio.h>
#include <string.h>

#include <..\rapidxml\rapidxml.hpp>
#include <JSON\json.hpp>
#include <fstream>

using namespace sf;
using namespace Krawler;
using namespace std;
using namespace rapidxml;

using json = nlohmann::json;

#define MAX_ANIMATION_FILE_CHARS 100000

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

	for (auto& pair : m_animationsMap)
	{
		KFREE(pair.second);
	}
	m_animationsMap.clear();
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

Animation::KAnimation * const KAssetLoader::getAnimation(const std::wstring & name)
{
	auto findResult = m_animationsMap.find(name);
	if (findResult == m_animationsMap.end())
	{
		return nullptr;
	}

	return findResult->second;
}

TiledImport::KLevelMap * const KAssetLoader::getLevelMap(const std::wstring & name)
{
	auto findResult = m_importedLevelsMap.find(name);
	if (findResult == m_importedLevelsMap.end())
	{
		return nullptr;
	}

	return findResult->second;
}

void KAssetLoader::loadTexture(const std::wstring & name, const std::wstring & filePath)
{
	Texture* pTex = new Texture;
	sf::String s(m_rootFolder + KTEXT("\\") + filePath);

	if (!pTex->loadFromFile(s.toAnsiString()))
	{
		KFREE(pTex);
		return;
	}

	m_texturesMap.emplace(name, pTex);
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

void KAssetLoader::loadTilemap(const std::wstring & name, const std::wstring & filePath)
{
	TiledImport::KLevelMap* pMap;
	pMap = TiledImport::loadTiledJSONFile(m_rootFolder + KTEXT("\\") + filePath);
	KCHECK(pMap);
	if (!pMap)
	{
		return;
	}
	m_importedLevelsMap.emplace(name, pMap);
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
	std::thread thread_a(&KAssetLoader::loadAssetsXML, this);
	std::thread thread_b(&KAssetLoader::loadAnimationsXML, this);

	thread_a.join();
	thread_b.join();
	//loadAssetsXML();
	//loadAnimationsXML();
	matchAnimationsToTextures();
}

void KAssetLoader::loadAssetsXML()
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
		else if (wstring(pAssetTypeNodes->name()) == KTEXT("level"))
		{
			pAttrName = pAssetTypeNodes->first_attribute();
			if (pAttrName)
			{
				if (wstring(pAttrName->name()) == KTEXT("name"))
				{
					assetName = pAttrName->value();
				}
			}
			pAttrFilepath = pAttrName->next_attribute();
			if (pAttrFilepath)
			{
				if (wstring(pAttrFilepath->name()) == KTEXT("file_path"))
				{
					assetPathA = pAttrFilepath->value();
				}
			}
			if (assetName.length() == 0 || assetPathA.size() == 0)
			{
				KPRINTF("Asset attributes length == 0!\n");
				goto cleanup_fail;
			}
			loadTilemap(assetName, assetPathA);
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

void KAssetLoader::loadAnimationsXML()
{
	wifstream animationsXMLFile;
	const wstring filePath = m_rootFolder + L"\\animations.xml";
	animationsXMLFile.open(filePath, ios::in);
	animationsXMLFile >> noskipws;

	if (animationsXMLFile.fail())
	{
		KPRINTF("No animations.xml file found!\n");
		return;
	}

	wchar_t textArray[MAX_ANIMATION_FILE_CHARS];
	int32 index = 0;

	while (index < MAX_ANIMATION_FILE_CHARS && !animationsXMLFile.eof())
	{
		animationsXMLFile.get(textArray[index]);
		++index;
		if (index > MAX_ANIMATION_FILE_CHARS && !animationsXMLFile.eof())
		{
			KPRINTF("Failed to load anim");
			KCHECK(false);
		}
	}

	if (index < MAX_ANIMATION_FILE_CHARS)
	{
		textArray[index - 1] = L'\0';
	}
	animationsXMLFile.close();


	xml_document<wchar_t> animationsXMLDoc;
	animationsXMLDoc.parse<0>(textArray);
	xml_node<wchar_t>* animation_node = animationsXMLDoc.first_node(KTEXT("data"))->first_node(KTEXT("animation"));
	Animation::KAnimation animDataStruct;

	if (!animation_node)
	{
		KPRINTF_A("No animation nodes found in %s\n", filePath.c_str());
		goto cleanup_branch_fail;
	}


	xml_attribute<wchar_t> *pAnimationName = nullptr, *pTextureName = nullptr, *pFrameTime = nullptr,
		*pWidth = nullptr, *pHeight = nullptr;

	while (animation_node)
	{
		//Animation name
		{
			if (wstring(animation_node->name()) != KTEXT("animation"))
			{
				KPRINTF_A("Incorrect node name %s\n", animation_node->name());
				goto cleanup_branch_fail;
			}
			pAnimationName = animation_node->first_attribute(KTEXT("name"));
			if (!pAnimationName)
			{
				KPRINTF("No animation name specified\n");
				goto cleanup_branch_fail;
			}

			if (wcslen(pAnimationName->value()) == 0)
			{
				KPRINTF("Invalid animation name, length == 0\n");
				goto cleanup_branch_fail;
			}

			animDataStruct.animationName = pAnimationName->value();
		}

		//Texture name
		{
			pTextureName = animation_node->first_attribute(KTEXT("texture_name"));
			if (!pTextureName)
			{
				KPRINTF_A("No texture name referenced, animation name: %s \n", animDataStruct.animationName.c_str());
				goto cleanup_branch_fail;
			}
			if (wcslen(pTextureName->value()) == 0)
			{
				KPRINTF_A("Invalid texture name! Texture name length == 0! Animation name: %s\n", animDataStruct.animationName.c_str());
				goto cleanup_branch_fail;
			}
			animDataStruct.textureName = pTextureName->value();
		}

		//Frame Time
		{
			pFrameTime = animation_node->first_attribute(KTEXT("fps"));
			if (!pFrameTime)
			{
				KPRINTF_A("No fps referenced, animation name: %s \n", animDataStruct.animationName.c_str());
				goto cleanup_branch_fail;
			}
			if (wcslen(pTextureName->value()) == 0)
			{
				KPRINTF_A("No value given for fps, animation name: %s\n", animDataStruct.animationName.c_str());
				goto cleanup_branch_fail;
			}
			const float denominator = static_cast<float> (_wtof(pFrameTime->value()));
			if (denominator != 0.0f)
			{
				animDataStruct.frameTime = 1.0f / denominator;
			}
		}

		//bounds
		{

			pWidth = animation_node->first_attribute(KTEXT("width"));
			if (!pWidth)
			{
				KPRINTF_A("No width referenced, animation name: %s \n", animDataStruct.animationName.c_str());
				goto cleanup_branch_fail;
			}
			if (wcslen(pWidth->value()) == 0)
			{
				KPRINTF_A("Width property length == 0, animation name: %s \n", animDataStruct.animationName.c_str());
				goto cleanup_branch_fail;
			}

			animDataStruct.bounds.x = (float)_wtoi(pWidth->value());
			pHeight = animation_node->first_attribute(KTEXT("height"));

			if (!pHeight)
			{
				KPRINTF_A("No height referenced, animation name: %s \n", animDataStruct.animationName.c_str());
				goto cleanup_branch_fail;
			}

			if (wcslen(pHeight->value()) == 0)
			{
				KPRINTF_A("height property length == 0, animation name: %s \n", animDataStruct.animationName.c_str());
				goto cleanup_branch_fail;
			}
			animDataStruct.bounds.y = (float)_wtoi(pHeight->value());
		}

		xml_node<wchar_t>* pFrameNode = animation_node->first_node(KTEXT("frame"));
		xml_attribute<wchar_t> * pXPosAttr = nullptr, *pYPosAttr = nullptr;
		Vec2f frameData;
		while (pFrameNode)
		{

			pXPosAttr = pFrameNode->first_attribute(KTEXT("x_pos"));
			if (pXPosAttr)
			{
				if (wcslen(pXPosAttr->value()) > 0)
				{
					frameData.x = (float)_wtoi(pXPosAttr->value());
				}
				else
				{
					KPRINTF_A("Invalid frame x position! Animation name %s\n", animDataStruct.animationName.c_str());
				}
			}
			else
			{
				KPRINTF_A("Invalid frame x position! Animation name %s\n", animDataStruct.animationName.c_str());
			}

			pYPosAttr = pFrameNode->first_attribute(KTEXT("y_pos"));
			if (pYPosAttr)
			{
				if (wcslen(pYPosAttr->value()) > 0)
				{
					frameData.y = (float)_wtoi(pYPosAttr->value());
				}
				else
				{
					KPRINTF_A("Invalid frame y position! Animation name %s\n", animDataStruct.animationName.c_str());
				}
			}
			else
			{
				KPRINTF_A("Invalid frame y position! Animation name %s\n", animDataStruct.animationName.c_str());
			}

			animDataStruct.frameData.push_back(frameData);

			pFrameNode = pFrameNode->next_sibling();
		}
		m_animationsMap.emplace(animDataStruct.animationName, new Animation::KAnimation(animDataStruct));

		animation_node = animation_node->next_sibling();
		animDataStruct.frameData.clear();
	}

cleanup_branch_fail:
	animationsXMLDoc.clear();
	return;

}

void KAssetLoader::matchAnimationsToTextures()
{
	for (auto& anim : m_animationsMap)
	{
		Texture* pTex = getTexture(anim.second->textureName);
		if (!pTex)
		{
			KPrintf(KTEXT("Unable to find texture %s for animation %s\n"), anim.second->textureName.c_str(), anim.first.c_str());
			continue;
		}
		anim.second->pTexture = pTex;

	}

}

