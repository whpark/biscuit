module;

//////////////////////////////////////////////////////////////////////
//
// mfc_util.h: from Mocha - xUtil.h
//
// PWH
// 2000.????
// 2021.06.10 gtl.
//	2024-10-04. biscuit.
//
///////////////////////////////////////////////////////////////////////////////

#include "biscuit/config.h"
#include "biscuit/macro.h"
#include "biscuit/dependencies_fmt.h"

#define NOMINMAX
#include <Windows.h>

export module biscuit.win.serial_port;
import std;
import biscuit;

using namespace std::literals;
using namespace biscuit::literals;

export namespace biscuit::win {

	/*constexpr*/ std::string const EXT_DEFAULT_IMAGE_FILE	{ ".png"s };
	/*constexpr*/ std::string const FILTER_IMAGE_FILES		{ "Image Files(*.bmp;*.jpg;*.tiff;*.png)|*.bmp;*.jpg;*.tiff;*.png|Bitmap(*.bmp)|*.bmp|PNG File(*.png)|*.png|JPEG File(*.jpg)|*.jpg|All Files(*.*)|*.*||" };
	/*constexpr*/ std::string const IMAGE_FILE_EXTS			{ "*.bmp;*.jpg;*.jpeg;*.tiff;*.png;*.gif" };

	/*constexpr*/ std::string const INVALID_CHAR_FOR_FILE_NAME { "\\/:*?<>|" };

	//GTL__WIN_API gtl::xString GetErrorMessage(CException& e);
	std::wstring GetErrorMessage(DWORD dwLastError) {
		std::wstring str;
		LPVOID lpMsgBuf;
		FormatMessageW(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwLastError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPWSTR) &lpMsgBuf,
			0,
			NULL 
		);

		str = (LPCWSTR)lpMsgBuf;
		biscuit::TrimRight(str);

		// Free the buffer.
		LocalFree( lpMsgBuf );

		return str;
	}

}	// namespace biscuit::win

