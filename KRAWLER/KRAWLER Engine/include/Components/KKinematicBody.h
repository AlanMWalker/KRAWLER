#pragma once
#include "Krawler.h"
#include "KComponent.h"

namespace Krawler
{
	namespace Components
	{
		class KKinematicBody :
			public KComponentBase
		{
		public:
			KKinematicBody(Krawler::KEntity& entity);
			~KKinematicBody() = default;


		private:

		};

	}
}