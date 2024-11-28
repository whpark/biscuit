#pragma once

#include <QtCore/qglobal.h>

//#ifdef BSC__QT_BUILD_STATIC
#	define BSC__QT_API
//#else
//#	if defined(BSC__QT_EXPORTS)
//#		define BSC__QT_API __declspec(dllexport)
//#	else
//#		define BSC__QT_API __declspec(dllimport)
//#	endif
//#endif
#define BSC__QT_CLASS BSC__QT_API
#define BSC__QT_DATA BSC__QT_API

