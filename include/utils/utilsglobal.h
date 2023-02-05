#ifndef UTILSGLOBAL_H
#define UTILSGLOBAL_H

#include <build/importexport.h>

#if defined(UTILS_LIBRARY)
#define UTILS_SHARED_EXPORT SIMPEX_DECL_EXPORT
#else
#define UTILS_SHARED_EXPORT SIMPEX_DECL_IMPORT
#endif

#endif // UTILSGLOBAL_H
