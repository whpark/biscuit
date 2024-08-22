#pragma once

#include <version>
#include <cstdint>

import "biscuit/config.h";
import "biscuit/macro.h";

#if !defined(BISCUIT_EXPORTS)
#	pragma comment(lib, "biscuit.lib")
#endif

//=================================================================================================================================
// 
// biscuit naming conventions
//   class name : PascalCase starting with "x"
//   member variable name : camelCase starting with "m_"
//   function name : PascalCase
//   Template class name : PascalCase starting with "T"
//
//  2024-08-12. PWH.
//
//=================================================================================================================================
