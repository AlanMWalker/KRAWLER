#ifndef KSTATIC_BODY_H
#define KSTATIC_BODY_H

#include "Krawler.h"
#include "KComponent.h"

namespace Krawler
{
	namespace Components
	{
		class KStaticBody :
			public KComponentBase
		{
		public:
			KStaticBody(Krawler::KEntity& entity);
			~KStaticBody() = default;

		private:

		};
	}
}

#endif