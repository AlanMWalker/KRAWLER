// KRAWLER.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "vld.h"

#include "Krawler.h"
#include "source.h"
using namespace Krawler;

int main(void)
{
	Krawler::KInitStatus k();
	KINIT_CHECK(k);

	return 0;
}

Krawler::KInitStatus k()
{
	return KInitStatus::Failure;
}

