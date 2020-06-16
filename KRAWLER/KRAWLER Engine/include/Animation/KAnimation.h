#ifndef KANIMATION_H
#define KANIMAITON_H

#include "Krawler.h"

#include <vector>
#include <SFML\Graphics\Texture.hpp>

namespace Krawler
{
	namespace Animation
	{
		struct KAnimation
		{
			float frameTime; // Amount of time to elapse between frames
			std::wstring animationName; //name of animation to engine
			std::wstring textureName; // name of texture bound to this animation
			sf::Texture* pTexture = nullptr;
			Krawler::Vec2f bounds; //width & height for each sub-image
			std::vector<Krawler::Vec2f> frameData; //vector of all animations
		};

	}
}

#endif 