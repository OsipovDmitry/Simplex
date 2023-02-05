#ifndef QTGLOBAL_H
#define QTGLOBAL_H

#include <build/importexport.h>

#if defined(QT_LIBRARY)
#define QT_SHARED_EXPORT SIMPEX_DECL_EXPORT
#else
#define QT_SHARED_EXPORT SIMPEX_DECL_IMPORT
#endif

#endif // QTGLOBAL_H
