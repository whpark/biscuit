module;

#include "biscuit/dependencies_cereal.h"

export module biscuit.cereal_helper;

import std;
import biscuit.aliases;
import biscuit.convert_codepage;

export namespace cereal {
	using namespace biscuit;

	template < typename archive >
	void save(archive& ar, std::wstring const& str) {
		std::u8string u8 = ConvertString<char8_t>(str);
		ar(u8A(u8));
	}

	template < typename archive >
	void load(archive& ar, std::wstring& str) {
		std::string u8;
		ar(u8);
		str = ConvertString<wchar_t>(Au8(u8));
	}

	template < typename archive >
	void save(archive& ar, deg_t const& angle) {
		ar('d', angle.value());
	}
	template < typename archive >
	void save(archive& ar, rad_t const& angle) {
		ar('r', angle.value());
	}

	template < typename archive >
	void load(archive& ar, deg_t& angle) {
		char type{};
		double value{};
		ar(type, value);
		switch (type) {
		case 'd': angle = deg_t(value); break;
		case 'r': angle = rad_t(value); break;
		default:
			angle = {};
			throw std::runtime_error("invalid angle type");
		}
	}
	template < typename archive >
	void load(archive& ar, rad_t& angle) {
		char type{};
		double value{};
		ar(type, value);
		switch (type) {
		case 'd': angle = deg_t(value); break;
		case 'r': angle = rad_t(value); break;
		default:
			angle = {};
			throw std::runtime_error("invalid angle type");
		}
	}

}
