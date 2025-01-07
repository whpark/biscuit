module;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// EnvironmentVariable.ixx : Declaration of the xEnvironmentVariable
//
// PWH.
//
// 2023-08-08 from mocha
// 2024-10-04. biscuit
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "biscuit/config.h"
#include "biscuit/macro.h"
//#include "biscuit/dependencies_fmt.h"
#include <winreg/WinReg.hpp>

export module biscuit.win.environment_variable;
import std;
import fmt;
import biscuit.aliases;
import biscuit.concepts;

export namespace biscuit::win {

	// Environment Setting
	class xEnvironmentVariable {
	public:
		using this_t = xEnvironmentVariable;
		using key_t = std::wstring;
		using string_t = std::wstring;
		enum class eSCOPE {
			LOCAL_MACHINE,
			CURRENT_USER,
		};

	protected:
		//bool m_bBroadcasted;
		winreg::RegKey m_reg;
	public:
		xEnvironmentVariable(eSCOPE scope = eSCOPE::LOCAL_MACHINE) {
			Open(scope);
		}
		~xEnvironmentVariable() { }

		bool Open(eSCOPE scope = eSCOPE::LOCAL_MACHINE) try {
			if (scope == eSCOPE::LOCAL_MACHINE)
				m_reg.Open(HKEY_LOCAL_MACHINE, L"System\\CurrentControlSet\\Control\\Session Manager\\Environment");
			else
				m_reg.Open(HKEY_CURRENT_USER, L"Environment");
			return true;
		}
		catch (...) {
			return false;
		}

		explicit operator bool() const noexcept { return (bool)m_reg; }
		bool IsValid() const noexcept { return (bool)m_reg; }

		bool Broadcast(std::chrono::milliseconds timeout = std::chrono::milliseconds(3'000)) {
			if (!m_reg)
				return false;
			DWORD_PTR result;
			LPCTSTR strBuf = L"Environment";
			SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)strBuf, SMTO_ABORTIFHUNG, (UINT)timeout.count(), &result);
			return true;
		}

		// Operation
		std::optional<std::wstring> Get(key_t const& key) {
			if (!m_reg)
				return {};
			if (auto r = m_reg.TryGetStringValue(key)) {
				return r.GetValue();
			}
			return {};
		}
		std::unordered_map<std::wstring, std::wstring> GetAll() {
			std::unordered_map<std::wstring, std::wstring> r;
			if (!m_reg)
				return {};
			for (auto&& [key, type] : m_reg.EnumValues()) {
				if (type == REG_SZ)
					r[key] = m_reg.GetStringValue(key);
			}
			return r;
		}
		bool Set(key_t const& key, string_t const& value) {
			if (!m_reg)
				return false;
			auto r = m_reg.TrySetStringValue(key, value);
			return !!r;
		}
		bool Delete(key_t const& key) {
			if (!m_reg)
				return false;
			auto r = m_reg.TryDeleteKey(key, KEY_READ | KEY_WRITE | KEY_WOW64_64KEY);
			return !!r;
		}
	};

};	// namespace biscuit::win

