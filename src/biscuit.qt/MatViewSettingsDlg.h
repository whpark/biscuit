#pragma once

#include <QDialog>
#include "ui_MatViewSettingsDlg.h"

#include "biscuit/qt/MatView.h"

import std;
import biscuit;

namespace biscuit::qt {

	class xMatViewSettingsDlg : public QDialog {
		Q_OBJECT

	public:
		using this_t = xMatViewSettingsDlg;
		using base_t = QDialog;

		xMatView::S_OPTION m_option;

	public:
		xMatViewSettingsDlg(xMatView::S_OPTION option, QWidget* parent = nullptr);
		~xMatViewSettingsDlg() {}

		bool UpdateData(bool bSaveAndValidate = true);

	protected:
		void OnOK();
		void OnCancel();
		void OnBackgroundColor();

	private:
		Ui::MatViewSettingsDlgClass ui;
	};

}	// namespace biscuit::qt
