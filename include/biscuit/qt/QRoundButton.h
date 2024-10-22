#pragma once

#include "biscuit/config.h"
#include "biscuit/macro.h"
#include "biscuit/qt/macro.h"
#include "biscuit/qt/QColorButton.h"

#include <QtWidgets/QtWidgets>

namespace biscuit::qt { class BSC__QT_CLASS QRoundButton; }

class biscuit::qt::QRoundButton : public biscuit::qt::QColorButton {
	Q_OBJECT
public:
	using this_t = QRoundButton;
	using base_t = QColorButton;

public:
	using base_t::base_t;
	QRoundButton(QWidget* parent = nullptr);

public:
	virtual void SetMainColor(QColor colorBackground, std::optional<QColor> colorForeground = {});
};

