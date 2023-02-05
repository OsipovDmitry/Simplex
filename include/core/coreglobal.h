#ifndef COREGLOBAL_H
#define COREGLOBAL_H

#include <build/importexport.h>

#ifdef CORE_LIBRARY
#   define CORE_SHARED_EXPORT SIMPEX_DECL_EXPORT
#else
#   define CORE_SHARED_EXPORT SIMPEX_DECL_IMPORT
#endif

#endif // COREGLOBAL_H
