#pragma once

#include <version>
#include <cstdint>

#include "biscuit/config.h"
#include "biscuit/macro.h"

// depedencies
#include "units.h"

#if !defined(BISCUIT_EXPORTS)
#	pragma comment(lib, "biscuit.lib")
#endif

using namespace std::literals;
using namespace units::literals;


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
