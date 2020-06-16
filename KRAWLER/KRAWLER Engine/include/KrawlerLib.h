#ifndef KRAWLER_LIB_H
#define KRAWLER_LIB_H

#ifdef KRAWLERENGINE_EXPORTS
#define KRAWLER_API __declspec(dllexport)   
#else 
#ifdef LINUX
#define KRAWLER_API
#else
#define KRAWLER_API __declspec(dllimport)   
#endif
#endif  

#ifndef LINUX
#define FORCE_INLINE __forceinline
#else
#define FORCE_INLINE __attribute__((always_inline))
#endif

#endif 
