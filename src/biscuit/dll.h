#pragma once

#if defined(_USRDLL) && defined(BSC__EXPORTS)
#	define BSC__EXPORT_DLL __declspec(dllexport)
#else
#	define BSC__EXPORT_DLL
#endif
