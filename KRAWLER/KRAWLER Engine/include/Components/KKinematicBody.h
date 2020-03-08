#ifndef KKINEMATIC_BODY_H
#define KKINEMATIC_BODY_H

#include "KBody.h"

namespace Krawler
{
	namespace Components
	{
		class KKinematicBody :
			public KBody
		{
		public:
			KKinematicBody(Krawler::KEntity& entity);
			~KKinematicBody() = default;


		private:

		};

	}
}

#endif