#include "AssetLoader\KAssetLoader.h"
#include "Animation\KAnimation.h"

#include <future>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <filesystem>

#include <..\rapidxml\rapidxml.hpp>
#include <string.h>
#include <JSON\json.hpp>


using namespace sf;
using namespace Krawler;
using namespace std;
using namespace rapidxml;

using json = nlohmann::json;

constexpr int32 MAX_ANIMATION_FILE_CHARS = 100000;

// HELPER FUNCTIONS

std::wstring FindFilename(std::wstring& str)
{
	std::wstring returnStr;
	uint32 idx = static_cast<unsigned>(str.size() - 1);
	bool bIsFilename = false;
	while (idx != 0)
	{
		if (!bIsFilename)
		{
			if (str[idx] == KTEXT('.'))
			{
				bIsFilename = true;
			}
		}
		else
		{
			if (str[idx] != KTEXT('\\'))
			{
				returnStr.push_back(str[idx]);
			}
			else
			{
				break;
			}
		}
		--idx;
	}

	return std::wstring(returnStr.rbegin(), returnStr.rend());
}

// ASSET LOADER 
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

	for (auto& pair : m_importedLevelsMap)
	{
		TiledImport::cleanupLevelMap(pair.second);
		pair.second = nullptr;
	}
	m_importedLevelsMap.clear();
}

sf::Texture * const KAssetLoader::getTexture(const std::wstring & name)
{
	auto findResult = m_texturesMap.find(name);

	if (findResult == m_texturesMap.end())
	{
		KPrintf(KTEXT("Failed to find texture with name %s\n"), name.c_str());
		return m_texturesMap[KTEXT("missing")];
	}
	
	return findResult->second;
}

sf::SoundBuffer * const KAssetLoader::getSound(const std::wstring & name)
{
	auto findResult = m_soundBufferMap.find(name);
	if (findResult == m_soundBufferMap.end())
	{
		KPrintf(KTEXT("Failed to find sound with name %s\n"), name.c_str());
		return nullptr;
	}
	
	return findResult->second;
}

sf::Shader * const KAssetLoader::getShader(const std::wstring & name)
{
	auto findResult = m_shaderMap.find(name);
	if (findResult == m_shaderMap.end())
	{
		KPrintf(KTEXT("Failed to find shader with name %s\n"), name.c_str());
		return nullptr;
	}
	
	return findResult->second;
}

sf::Font * const KAssetLoader::getFont(const std::wstring & name)
{
	auto findResult = m_fontMap.find(name);
	if (findResult == m_fontMap.end())
	{
		KPrintf(KTEXT("Failed to font shader with name %s\n"), name.c_str());
		return nullptr;
	}
	
	return findResult->second;
}

Animation::KAnimation * const KAssetLoader::getAnimation(const std::wstring & name)
{ 
	auto findResult = m_animationsMap.find(name);
	if (findResult == m_animationsMap.end())
	{
		KPrintf(KTEXT("Failed to find animation with name %s\n"), name.c_str());
		return nullptr;
	}
	
	return findResult->second;
}

TiledImport::KTIMap * const KAssetLoader::getLevelMap(const std::wstring & name)
{
	auto findResult = m_importedLevelsMap.find(name);
	if (findResult == m_importedLevelsMap.end())
	{
		KPrintf(KTEXT("Failed to find level map with name %s\n"), name.c_str());
		return nullptr;
	}

	return findResult->second;
}

void KAssetLoader::loadTexture(const std::wstring & name, const std::wstring & filePath)
{
	Texture* pTex = new Texture;
	sf::String s(filePath);

	if (m_texturesMap.find(name) != m_texturesMap.end())
	{
		KPrintf(KTEXT("Already loaded %s - skipping!"), filePath.c_str());
		return;
	}

	if (!pTex->loadFromFile(s.toAnsiString()))
	{
		KFREE(pTex);
		return;
	}

	if (pTex->getSize().x > 4096 || pTex->getSize().y > 4096)
	{
		KPrintf(KTEXT("Asset load error: Texture %s is larger in dimensions than 4096 px!\n"), name.c_str());
	}

	pTex->generateMipmap();
	pTex->setSmooth(true);
	m_texturesMap.emplace(name, pTex);
}

void KAssetLoader::loadSound(const std::wstring & name, const std::wstring & filePath)
{
	if (m_soundBufferMap.find(name) != m_soundBufferMap.end())
	{
		KPrintf(KTEXT("Already loaded %s - skipping!"), filePath.c_str());
		return;
	}

	SoundBuffer* pSoundBuffer = new SoundBuffer;
	KCHECK(pSoundBuffer);
	sf::String s(filePath);

	if (!pSoundBuffer->loadFromFile(s.toAnsiString()))
	{
		delete pSoundBuffer;
		KPrintf(KTEXT("Failed to load sound %ws\n"), s.toWideString().c_str());
		return;
	}

	m_soundBufferMap.emplace(name, pSoundBuffer);
}

void KAssetLoader::loadFont(const std::wstring& name, const std::wstring& filePath)
{
	if (m_fontMap.find(name) != m_fontMap.end())
	{
		KPrintf(KTEXT("Already loaded %s - skipping!"), filePath.c_str());
		return;
	}

	Font f;
	const sf::String s(filePath);

	if (!f.loadFromFile(s.toAnsiString()))
	{
		return;
	}

	m_fontMap.emplace(name, new Font(f));
}

void KAssetLoader::loadTilemap(const std::wstring & name, const std::wstring & filePath)
{
	if (m_importedLevelsMap.find(name) != m_importedLevelsMap.end())
	{
		KPrintf(KTEXT("Already loaded %s - skipping!"), filePath.c_str());
		return;
	}

	TiledImport::KTIMap* pMap;
	pMap = TiledImport::loadTiledJSONFile(m_rootFolder + KTEXT("\\") + filePath);
	KCHECK(pMap);
	if (!pMap)
	{
		return;
	}
	m_importedLevelsMap.emplace(name, pMap);
}

void KAssetLoader::loadDefaultShadersFromString()
{
	const std::string fragmentShader = R"(
		uniform sampler2D texture;
		
		void main()
		{
			// lookup the pixel in the texture
			vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
		
			// multiply it by the color
			gl_FragColor = gl_Color * pixel;
		})";


	const std::string vertexShader = R"(
		//#version 110 
		
		void main()
		{
			// transform the vertex position
			gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
		
			// transform the texture coordinates
			gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
		
			// forward the vertex color
			gl_FrontColor = gl_Color; 
		}
	)";

	Shader* pDefaultShader = new Shader();
	KCHECK(pDefaultShader);
	if (!pDefaultShader->loadFromMemory(vertexShader, fragmentShader))
	{
		delete pDefaultShader;
		return;
	}

	m_shaderMap.emplace(KTEXT("default"), pDefaultShader);
}

void KAssetLoader::loadShader(const std::wstring& shaderName, const std::wstring & shaderPath)
{
	if (m_shaderMap.find(shaderName) != m_shaderMap.end())
	{
		KPrintf(KTEXT("Already loaded %s - skipping!"), shaderName.c_str());
		return;
	}
	/*

	TODO:
	- Split shader file int two strings
	- Pass split strings to loadFromMemory
	*/
	string contents;
	ifstream shaderFile;
	shaderFile.open(shaderPath, ios::in | ios::ate);

	if (shaderFile.fail())
	{
		KPrintf(L"Unable to open shader file %s\n", shaderPath.c_str());
	}

	contents.resize(shaderFile.tellg(), '\0');
	auto len = static_cast<long>(shaderFile.tellg());
	shaderFile.seekg(ios::beg);
	shaderFile.read(&contents[0], len);

	auto result = contents.find("#DIVIDE");

	auto strA = string(contents, 0, result);
	auto strB = string(contents, result + strlen("#DIVIDE") + 1, contents.size() - strA.length());
	Shader* const pShader = new Shader();
	KCHECK(pShader);

	if (!pShader->loadFromMemory(strA, strB))
	{
		KPrintf(KTEXT("Failed shader name: %s\n"), shaderName.c_str());
		delete pShader;
		return;
	}

	m_shaderMap.emplace(shaderName, pShader);
}

KAssetLoader::KAssetLoader() : m_rootFolder(KTEXT("res"))
{
	auto path = m_rootFolder + KTEXT("\\") + "engine\\missing.png";
	loadTexture(KTEXT("missing_texture"), path);
	std::thread thread_a(&KAssetLoader::scanFolderLoad, this);
	std::thread thread_b(&KAssetLoader::loadAnimationsXML, this);
	thread_a.join();
	thread_b.join();
	matchAnimationsToTextures();
}

void KAssetLoader::scanFolderLoad()
{
	constexpr uint32 NUM_TEXTURE_TYPES = sizeof(ACCEPTED_TEXTURES) / sizeof(ACCEPTED_TEXTURES[0]);
	constexpr uint32 NUM_AUDIO_TYPES = sizeof(ACCEPTED_AUDIO) / sizeof(ACCEPTED_AUDIO[0]);
	constexpr uint32 NUM_SHADER_TYPES = sizeof(ACCEPTED_SHADERS) / sizeof(ACCEPTED_SHADERS[0]);
	constexpr uint32 NUM_FONT_TYPES = sizeof(ACCEPTED_FONT) / sizeof(ACCEPTED_FONT[0]);


	vector<wstring> filesList;
	list<wstring> shaderFilesList;

	std::wstring path = m_rootFolder + KTEXT("\\");
	for (const auto& entry : filesystem::recursive_directory_iterator(path))
	{
		if (!entry.is_directory())
		{
			filesList.push_back(entry.path());
		}
	}

	for (auto& path : filesList)
	{
		for (auto textureExtension : ACCEPTED_TEXTURES)
		{
			if (path.find(textureExtension) != wstring::npos) // if the extension is found in the string load the image file
			{
				loadTexture(FindFilename(path), path);
			}
		}

		for (auto audioExtension : ACCEPTED_AUDIO)
		{
			if (path.find(audioExtension) != wstring::npos)
			{
				loadSound(FindFilename(path), path);
			}
		}

		for (auto fontExtension : ACCEPTED_FONT)
		{
			if (path.find(fontExtension) != wstring::npos)
			{
				loadFont(FindFilename(path), path);
			}
		}

		for (auto shaderExtension : ACCEPTED_SHADERS)
		{
			if (path.find(shaderExtension) != wstring::npos)
			{
				loadShader(FindFilename(path), path);
			}
		}
	}
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
			KPRINTF("Failed to load animation because too many characters in file.");
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

