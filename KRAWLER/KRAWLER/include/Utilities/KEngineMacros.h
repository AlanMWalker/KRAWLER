#ifndef KENGINE_MACROS_H
#define KENGINE_MACROS_H

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

#endif 
