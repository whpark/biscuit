module;

#include "biscuit/config.h"
#include "biscuit/macro.h"

export module biscuit.axis;
import std;
import biscuit.aliases;
import biscuit.concepts;

namespace concepts = biscuit::concepts;

using namespace std::literals;
using namespace biscuit::literals;

export namespace biscuit {
	//-----------------------------------------------------------------------------
	// axis
	enum class eAXIS : int {
		//eAXIS::NW = -4, eAXIS::NZ, eAXIS::NY, eAXIS::NX, eAXIS::NONE = 0, eAXIS::X, eAXIS::Y, eAXIS::Z, eAXIS::W,

		NONE = 0,
		X =   1,	NX = -X,
		Y = X+1,	NY = -Y,
		Z = Y+1,	NZ = -Z,
		W = Z+1,	NW = -W,

		PX = X,
		PY = Y,
		PZ = Z,
		PW = W,
	};

	BSC__NODISCARD inline bool IsValidCoordSystem(eAXIS eDX, eAXIS eDY) {
		int iDX = std::abs(static_cast<int>(eDX)), iDY = std::abs(static_cast<int>(eDY));
		return static_cast<int>(eDX) && static_cast<int>(eDY) && (static_cast<int>(iDX) != static_cast<int>(iDY));
	}
	BSC__NODISCARD inline bool IsValidCoordSystem(eAXIS eDX, eAXIS eDY, eAXIS eDZ) {
		int iDX = std::abs(static_cast<int>(eDX)), iDY = std::abs(static_cast<int>(eDY)), iDZ = std::abs(static_cast<int>(eDZ));
		return static_cast<int>(eDX) && static_cast<int>(eDY) && static_cast<int>(eDZ)
			&& (static_cast<int>(iDX) != static_cast<int>(iDY))
			&& (static_cast<int>(iDY) != static_cast<int>(iDZ))
			&& (static_cast<int>(iDZ) != static_cast<int>(iDX));
	}

	BSC__NODISCARD inline eAXIS GetOppositeDirection(eAXIS eDirection) { return (eAXIS)(-static_cast<int>(eDirection)); }

	template < concepts::string_elem tchar >
	BSC__NODISCARD constexpr inline eAXIS GetDirection(std::basic_string_view<tchar> svName) {
		signed char eNegative = tszsearch(svName, '-') ? -1 : 1;
		if (svName.find('x') != svName.npos or svName.find('X') != svName.npos) return (eAXIS)(static_cast<int>(eAXIS::X)*eNegative);
		if (svName.find('y') != svName.npos or svName.find('Y') != svName.npos) return (eAXIS)(static_cast<int>(eAXIS::Y)*eNegative);
		if (svName.find('z') != svName.npos or svName.find('Z') != svName.npos) return (eAXIS)(static_cast<int>(eAXIS::Z)*eNegative);
		return eAXIS::NONE;
	}
	BSC__NODISCARD constexpr inline auto GetDirectionName(eAXIS eDirection) -> std::basic_string_view<char> {
		using namespace std::literals;
		switch (eDirection) {
		case eAXIS::NONE : return ""sv;
		case eAXIS::X  : return  "X"sv; case  eAXIS::Y : return  "Y"sv; case eAXIS::Z  : return  "Z"sv;
		case eAXIS::NX : return "-X"sv; case eAXIS::NY : return "-Y"sv; case eAXIS::NZ : return "-Z"sv;
		}
		return ""sv;
	}

}	// namespace biscuit

