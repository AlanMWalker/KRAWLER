#ifndef KENGINE_MACROS_H
#define KENGINE_MACROS_H

#define SAFE_FREE(ptr) if( ptr != nullptr)	\
						{					\
							delete ptr;		\
							ptr = nullptr;	\
						}

#define SAFE_FREE_ARR(ptr) if( ptr != nullptr)	\
						{						\
							delete[] ptr;		\
							ptr = nullptr;		\
						}

#define CHECK(v) assert(v)

#endif 
