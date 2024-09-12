module;

#include "biscuit/macro.h"
#include "biscuit/dependencies_units.h"

export module biscuit.units;
import std;
import biscuit.aliases;
import biscuit.concepts;

export namespace biscuit::inline literals {
	constexpr units::angle::radian_t operator "" _deg_to_rad(unsigned long long n) {
		return units::angle::radian_t{ units::angle::degree_t((double)n) };
	}
	constexpr units::angle::radian_t operator "" _deg_to_rad(long double x) {
		return units::angle::radian_t{ units::angle::degree_t((double)x) };
	}
	constexpr units::angle::degree_t operator "" _rad_to_deg(unsigned long long n) {
		return units::angle::degree_t{ units::angle::radian_t((double)n) };
	}
	constexpr units::angle::degree_t operator "" _rad_to_deg(long double x) {
		return units::angle::degree_t{ units::angle::radian_t((double)x) };
	}

}
