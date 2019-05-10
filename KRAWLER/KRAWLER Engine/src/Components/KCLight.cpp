#include <Components\KCLight.h>

using namespace Krawler; 
using namespace Krawler::Components;

Krawler::Components::KCLight::KCLight(KEntity * pEntity)
	: KCRenderableBase(pEntity)
{

}

KInitStatus KCLight::init()
{
	return KInitStatus();
}

void Krawler::Components::KCLight::tick()
{

}
