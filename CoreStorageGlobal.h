#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(CORESTORAGE_LIB)
#  define CORESTORAGE_EXPORT Q_DECL_EXPORT
# else
#  define CORESTORAGE_EXPORT Q_DECL_IMPORT
# endif
#else
# define CORESTORAGE_EXPORT
#endif
