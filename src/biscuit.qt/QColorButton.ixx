module;

#include "biscuit/config.h"
#include "biscuit/macro.h"
#include "verdigris/wobjectcpp.h"
#include "verdigris/wobjectimpl.h"

#include <QtWidgets/QtWidgets>

export module biscuit.qt.QColorButton;
import std;
import biscuit;

using namespace std::literals;

export namespace biscuit::qt {

	class QColorButton : public QPushButton {
		W_OBJECT(QColorButton)
	public:
		using this_t = QColorButton;
		using base_t = QPushButton;

	protected:
		QColor m_crBackground;
		QColor m_crForeground;
	public:
		using base_t::base_t;
		QColorButton(QWidget* parent = nullptr);
		~QColorButton() {}

	public:
		auto GetMainColor() const { return m_crBackground; }
		auto GetMainColorText() const { return m_crForeground; }
		virtual void SetMainColor(QColor colorBackground, std::optional<QColor> colorForeground = {});

	};

	W_OBJECT_IMPL(QColorButton);

	QColorButton::QColorButton(QWidget* parent) : QPushButton(parent) {
		auto p = qApp->palette();
		//SetMainColor(p.button().color(), p.buttonText().color());
	}

	void QColorButton::SetMainColor(QColor colorBackground, std::optional<QColor> colorForeground) {
		m_crBackground = colorBackground;
		if (colorForeground) {
			m_crForeground = *colorForeground;
		}
		else {
			auto p = qApp->palette();
			m_crForeground = p.buttonText().color();
		}

		QPalette pal = palette();
		pal.setColor(QPalette::ColorRole::Button, m_crBackground);
		pal.setColor(QPalette::ColorRole::ButtonText, m_crForeground);
		setPalette(pal);
	}

}	// namespace biscuit::qt

