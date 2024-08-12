#pragma once

#if defined(_USRDLL) && defined(BISCUIT_EXPORTS)
#	define BISCUIT_EXPORT_DLL __declspec(dllexport)
#else
#	define BISCUIT_EXPORT_DLL
#endif
