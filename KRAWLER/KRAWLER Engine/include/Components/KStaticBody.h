#pragma once

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