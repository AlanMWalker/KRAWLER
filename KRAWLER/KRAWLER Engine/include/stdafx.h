#ifndef STDAFX_H
#define STDAFX_H

#include <iostream>
#include <string>
#include <assert.h>

#ifdef _MSC_VER
#include <Windows.h>
#endif 

#include <SFML\Graphics.hpp>
#include "Krawler.h"
#include "Utilities\KEngineMacros.h"
#include "Utilities\KDebug.h"

#ifdef KRAWLER_EXPORTS
#define KRAWLER_API __declspec(dllexport)   
#else  
#define KRAWLER_API __declspec(dllimport)   
#endif  

#endif
