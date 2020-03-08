#pragma once

#include "KComponent.h"
#include "Krawler.h"

class KStaticBody :
	public Krawler::KComponentBase
{
public:	
	KStaticBody(Krawler::KEntity& entity);
	~KStaticBody() = default;
	
private:	

};

