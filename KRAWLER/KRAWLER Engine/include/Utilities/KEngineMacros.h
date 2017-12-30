#ifndef KENGINE_MACROS_H
#define KENGINE_MACROS_H

#include <assert.h>

#define KFREE(ptr) if( ptr != nullptr)	\
						{					\
							delete ptr;		\
							ptr = nullptr;	\
						}

#define KFREE_ARR(ptr) if( ptr != nullptr)	\
						{						\
							delete[] ptr;		\
							ptr = nullptr;		\
						}

#define KCHECK(v) assert(v)


#define KCAST(type, expr) static_cast<type> (expr)

#define KTEXT(str) L##str

#ifdef __GNUC__
#define KDEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define KDEPRECATED(func) __declspec(deprecated) func
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define KDEPRECATED(func) func
#endif

#endif 
