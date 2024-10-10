#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(BISCUIT_QT_LIB)
#  define BISCUIT_QT_EXPORT Q_DECL_EXPORT
# else
#  define BISCUIT_QT_EXPORT Q_DECL_IMPORT
# endif
#else
# define BISCUIT_QT_EXPORT
#endif
