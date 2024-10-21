module;

#include "biscuit/config.h"
#include "biscuit/macro.h"

export module biscuit.utils;
import std;
import biscuit.aliases;
import biscuit.concepts;
import biscuit.string;
import biscuit.misc;

namespace concepts = biscuit::concepts;

export namespace biscuit {

	//-----------------------------------------------------------------------------
	inline bool IsImageExtension(std::filesystem::path const& path) {
		auto ext = ToLower(path.extension().string());
		return IsOneOf(ext, ".bmp", ".jpg", ".jpeg", ".tiff", ".png", ".gif", ".jfif");
	}

	//-----------------------------------------------------------------------------
	BSC__NODISCARD inline bool IsChildPath(std::filesystem::path const& a, std::filesystem::path const& base) {
		auto rel = std::filesystem::relative(a, base);
		return !rel.empty() && rel.is_relative() and !rel.string().starts_with("..");
	};

	BSC__NODISCARD inline bool HasParentPath(std::filesystem::path const& path) {
		return path.has_parent_path() and path != path.parent_path();
	}

	/// @brief Get Project Name from source file path
	/// @param l : don't touch.
	/// @return 
	BSC__NODISCARD /*constexpr*/ auto GetProjectName(std::source_location const& l = std::source_location::current())
		-> std::wstring
	{
		std::filesystem::path path = l.file_name();
		if (path.extension() == ".h")	// CANNOT get project name from header file
			return {};
		while (HasParentPath(path)) {
			auto filename = path.filename();
			path = path.parent_path();
			if (path.filename() == L"src")
				return filename;
		}
		return {};
	}

	BSC__NODISCARD auto GetProjectRootFolder(std::wstring const& strProjectNameToBeRemoved,
		std::filesystem::path path = std::filesystem::current_path()) 
		-> std::filesystem::path
	{
		// Init Temp Folder Names (ex, "Debug", "Release", ...)
		static auto const strsTempFolder = []{
			std::vector<std::wstring> folders;
		#ifdef _DEBUG
			folders.push_back(L"Debug");
			folders.push_back(L"Debug64");
			folders.push_back(L"Debug.v142");
			folders.push_back(L"Debug.v143");
		#else
			folders.push_back(L"Release");
			folders.push_back(L"Release64");
			folders.push_back(L"Release.v142");
			folders.push_back(L"Release.v143");
		#endif
			if constexpr (sizeof(size_t) == sizeof(uint64_t)) {
				folders.push_back(L"x64");
			} else if constexpr (sizeof(size_t) == sizeof(uint32_t)) {
				folders.push_back(L"x86");
				folders.push_back(L"Win32");
			}
			folders.push_back(L"Temp");
			return folders;
		}();

		// Remove Project Folder ( ex, [src]/[ProjectName] )
		if ( HasParentPath(path) and (path.filename() == strProjectNameToBeRemoved) ) {
			path = path.parent_path();
			if (HasParentPath(path) and path.filename() == L"src") {
				path = path.parent_path();
			}
		}

		// Remove Output Dir Folder Name ( ex, Temp/x64/Debug/ )
		for (auto const& strTempFolder : strsTempFolder) {
			if (!HasParentPath(path))
				break;
			if (path.filename() == strTempFolder) {
				path = path.parent_path();
			}
		}
		return path;
	}

	auto SetCurrentPath_ProjectFolder(std::filesystem::path const& pathRelToProjectRoot,
		std::source_location const& l = std::source_location::current())
		-> std::optional<std::filesystem::path>
	{
		// first, Get Root Folder, and then attach [pathRel]
		auto projectName = biscuit::GetProjectName(l);
		auto path = biscuit::GetProjectRootFolder(projectName);
		if (!pathRelToProjectRoot.empty()) {
			if (pathRelToProjectRoot.filename() != path.filename())
				path /= pathRelToProjectRoot;
		}
		std::error_code ec{};
		std::filesystem::create_directories(path, ec);
		ec = {};
		std::filesystem::current_path(path, ec);
		if (ec)
			return {};
		return path;
	}
	inline auto SetCurrentPath_ProjectFolder(std::source_location const& l = std::source_location::current()) {
		auto projectName = biscuit::GetProjectName(l);
		return SetCurrentPath_ProjectFolder(std::filesystem::path(L"src") / projectName, l);
	}
	inline auto SetCurrentPath_BinFolder(std::source_location const& l = std::source_location::current()) {
		return SetCurrentPath_ProjectFolder(L"bin", l);
	}



}	// namespace bisucit;

