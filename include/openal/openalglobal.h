#ifndef OPENALGLOBAL_H
#define OPENALGLOBAL_H

#include <build/importexport.h>

#if defined(OPENAL_LIBRARY)
#define OPENAL_SHARED_EXPORT SIMPEX_DECL_EXPORT
#else
#define OPENAL_SHARED_EXPORT SIMPEX_DECL_IMPORT
#endif

#endif // OPENALGLOBAL_H
