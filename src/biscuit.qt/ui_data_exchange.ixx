module;

#include <QtCore>
#include <QtGui/QtGui>
#include <QtWidgets/QtWidgets>

export module biscuit.qt.ui_data_exchange;
import std;
import biscuit;
import biscuit.qt.utils;

using namespace std::literals;
using namespace biscuit::literals;

namespace concepts = biscuit::concepts;

#define GTL__QT_UPDATE_WIDGET_VALUE(widget_t, value_t, getter, setter)	\
	inline void UpdateWidgetValue(bool bSaveAndValidate, widget_t* w, value_t& value) {	\
		if (bSaveAndValidate) value = w->getter(); else w->setter(value); \
	}

#define GTL__QT_UPDATE_WIDGET_STRING(widget_t, getter, setter)	\
		GTL__QT_UPDATE_WIDGET_VALUE(widget_t, QString, getter, setter) \
	template < concepts::string_elem tchar_t > \
	inline void UpdateWidgetValue(bool bSaveAndValidate, widget_t* w, std::basic_string<tchar_t>& value) {	\
		if (bSaveAndValidate) value = ToTString<tchar_t>(w->getter()); else w->setter(ToQString(value)); \
	}\
	inline void UpdateWidgetValue(bool bSaveAndValidate, widget_t* w, std::filesystem::path& path) {	\
		if (bSaveAndValidate) path = ToWString(w->getter()); else w->setter(ToQString(path)); \
	}

#define GTL__QT_UPDATE_WIDGET_ARITHMETIC(widget_t, str_getter, str_setter) \
	template < concepts::arithmetic value_t>\
	inline void UpdateWidgetValue(bool bSaveAndValidate, widget_t* w, value_t& value, int base = 0,\
		std::string_view fmt = "{}") {\
		if (bSaveAndValidate) value = biscuit::qt::ToNumber<value_t>(w->str_getter());\
		else w->str_setter(std::vformat(fmt, std::make_format_args(value)).c_str());\
	}



export namespace biscuit::qt {

	//=============================================================================================================================
	// Dynamic Data Exchange

	GTL__QT_UPDATE_WIDGET_VALUE(QCheckBox, bool, isChecked, setChecked);
	//GTL__QT_UPDATE_WIDGET_VALUE(QCheckBox, int, isChecked, setChecked);
	GTL__QT_UPDATE_WIDGET_VALUE(QGroupBox, bool, isChecked, setChecked);
	GTL__QT_UPDATE_WIDGET_STRING(QLineEdit, text, setText);
	GTL__QT_UPDATE_WIDGET_ARITHMETIC(QLineEdit, text, setText);
	GTL__QT_UPDATE_WIDGET_STRING(QPlainTextEdit, toPlainText, setPlainText);
	GTL__QT_UPDATE_WIDGET_VALUE(QSpinBox, int, value, setValue);
	GTL__QT_UPDATE_WIDGET_VALUE(QDoubleSpinBox, double, value, setValue);
	GTL__QT_UPDATE_WIDGET_VALUE(QComboBox, int, currentIndex, setCurrentIndex);
	GTL__QT_UPDATE_WIDGET_STRING(QComboBox, currentText, setCurrentText);

	// enum
	template < typename value_t > requires std::is_enum_v<value_t>
	inline void UpdateWidgetValue(bool bSaveAndValidate, QComboBox* w, value_t& value) {
		if (bSaveAndValidate)
			value = (value_t)w->currentIndex();
		else
			w->setCurrentIndex(std::to_underlying(value));
	}

	// std::duration
	template < typename trep_t, typename tperiod_t, typename twidget_t > requires (requires (twidget_t* w) {
		{ w->value() } -> std::convertible_to<trep_t>;
		w->setValue(trep_t{});
	})
		inline void UpdateWidgetValue(bool bSaveAndValidate, twidget_t* w, std::chrono::duration<trep_t, tperiod_t>& value) {
		using duration_t = std::chrono::duration<trep_t, tperiod_t>;
		if (bSaveAndValidate) {
			value = duration_t{(trep_t)w->value()};
		}
		else {
			w->setValue(value.count());
		}
	}
	template < typename trep_t, typename tperiod_t, typename twidget_t > requires (requires (twidget_t* w) {
		{ w->text() } -> std::convertible_to<QString>;
		w->setText(QString{});
	})
		inline void UpdateWidgetValue(bool bSaveAndValidate, twidget_t* w, std::chrono::duration<trep_t, tperiod_t>& value) {
		using duration_t = std::chrono::duration<trep_t, tperiod_t>;
		if constexpr (std::is_floating_point_v<trep_t>) {
			if (bSaveAndValidate) {
				value = duration_t{ToArithmeticValue<trep_t>(w->text())};
			}
			else {
				w->setText(ToQString(std::format("{:g}", value.count())));
			}
		}
		else if constexpr (std::is_integral_v<trep_t>) {
			if (bSaveAndValidate) {
				value = duration_t{ToArithmeticValue<trep_t>(w->text())};
			}
			else {
				w->setText(ToQString(std::format("{}", value.count())));
			}
		}
	}

#undef GTL__QT_UPDATE_WIDGET_STRING
#undef GTL__QT_UPDATE_WIDGET_VALUE


}	// namespace biscuit::qt

