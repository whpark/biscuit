#pragma once

#include <version>
#include <cstdint>

#include "biscuit/config.h"
#include "biscuit/macro.h"
//#include "biscuit/dependencies_eigen.h"
//#include "biscuit/dependencies_fmt.h"
//#include "biscuit/dependencies_glaze.h"
//#include "biscuit/dependencies_iconv.h"
//#include "biscuit/dependencies_simdutf.h"
//#include "biscuit/dependencies_units.h"

#if !defined(BISCUIT_EXPORTS)
#	pragma comment(lib, "biscuit.lib")
#endif

using namespace std::literals;

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
