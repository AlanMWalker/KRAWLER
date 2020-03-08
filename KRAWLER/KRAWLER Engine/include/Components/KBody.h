#ifndef KBODY_H
#define KBODY_H

#include "Krawler.h"
#include "KComponent.h"

namespace Krawler
{
	namespace Components
	{

		/* 
		NOTE: 
		The following from Box2D's 'b2FixtureDef' have not been 
		translated into KRAWLER: 
		- isSensor
		- collision filtering 
		- shape
		- userData (may not support)
		*/
		struct KBodyDef
		{
			float density; 
			float friction; 
			float restitution;
		};

		class KBody :
			public KComponentBase
		{
		public: 
			KBody(KEntity& entity);
		
		private:
		};
	}
}

#endif